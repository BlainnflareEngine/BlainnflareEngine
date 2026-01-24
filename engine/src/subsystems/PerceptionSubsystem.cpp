#include "pch.h"
#include "subsystems/PerceptionSubsystem.h"

#include "Engine.h"
#include "scene/Scene.h"
#include "scene/TransformComponent.h"
#include "scene/BasicComponents.h"
#include "components/CameraComponent.h"
#include "components/PerceptionComponent.h"
#include "components/StimulusComponent.h"
#include "components/PhysicsComponent.h"
#include "ai/PerceptionEvents.h"
#include "subsystems/PhysicsSubsystem.h"
#include "physics/RayCastResult.h"
#include "physics/PhysicsEvents.h"

namespace Blainn
{

PerceptionSubsystem &PerceptionSubsystem::GetInstance()
{
    static PerceptionSubsystem instance;
    return instance;
}

void PerceptionSubsystem::Init()
{
    BF_INFO("PerceptionSubsystem Init");
}

void PerceptionSubsystem::Init(Settings &settings)
{
    settings.enableLOD = true;
    settings.lodNearDistance = 200.0f;
    settings.lodMidDistance = 500.0f;
    settings.lodFarDistance = 1000.0f;
    settings.lodNearUpdateInterval = 0.0f;
    settings.lodMidUpdateInterval = 0.1f;
    settings.lodFarUpdateInterval = 0.5f;

    SetSettings(settings);
    BF_INFO("PerceptionSubsystem Init");
}

void PerceptionSubsystem::Destroy()
{
    BF_INFO("PerceptionSubsystem Destroy");
    m_temporaryStimuli.clear();
    m_losCache.clear();
}

void PerceptionSubsystem::Update(float dt)
{
    Scene &scene = *Engine::GetActiveScene();

    for (auto it = m_temporaryStimuli.begin(); it != m_temporaryStimuli.end();)
    {
        it->age += dt;
        if (it->age >= it->lifetime) it = m_temporaryStimuli.erase(it);
        else ++it;
    }

    if (m_settings.enableLOD)
    {
        UpdateLOD();
    }

    ProcessSightStimuli(dt);
    ProcessSoundStimuli(dt);
    ProcessTouchStimuli();
    UpdateStimuliAge(dt);

    ProcessEvents();
}

void PerceptionSubsystem::ProcessSightStimuli(float dt)
{
    Scene &scene = *Engine::GetActiveScene();

    auto observers = scene.GetAllEntitiesWith<IDComponent, TransformComponent, PerceptionComponent, PhysicsComponent>();
    auto stimuliSources = scene.GetAllEntitiesWith<IDComponent, TransformComponent, StimulusComponent, PhysicsComponent>();

    for (const auto &[observerEntityHandle, observerID, observerTransform, perception, observerPhysics] : observers.each())
    {
        if (!perception.enabled || !perception.enableSight) continue;

        if (!ShouldUpdatePerception(perception, dt)) continue;

        Entity observerEntity = scene.GetEntityWithUUID(observerID.ID);
        Vec3 observerPos = scene.GetWorldSpaceTransform(observerEntity).GetTranslation();
        Quat observerRot = scene.GetWorldSpaceTransform(observerEntity).GetRotation();

        for (const auto &[sourceEntityHandle, sourceID, sourceTransform, stimulus, sourcePhysics] : stimuliSources.each())
        {
            if (observerEntityHandle == sourceEntityHandle) continue;

            if (!stimulus.enabled || !stimulus.enableSight) continue;

            if (perception.ShouldIgnoreTag(stimulus.tag)) continue;

            Entity sourceEntity = scene.GetEntityWithUUID(sourceID.ID);
            Vec3 sourcePos = scene.GetWorldSpaceTransform(sourceEntity).GetTranslation();

            Vec3 toTarget = sourcePos - observerPos;
            float distance = toTarget.Length();

            float effectiveRange = stimulus.sightRadius > 0.0f ? stimulus.sightRadius : perception.sightRange;

            if (distance > effectiveRange) continue;

            if (!IsInFieldOfView(observerPos, observerRot, sourcePos, perception.sightFOV)) continue;

            if (perception.sightRequireLOS)
            {
                uint64_t cacheKey =
                    (static_cast<uint64_t>(observerEntityHandle) << 32) | static_cast<uint64_t>(sourceEntityHandle);

                auto &cache = m_losCache[cacheKey];
                cache.timeSinceLastCheck += dt;

                if (cache.timeSinceLastCheck >= perception.sightLOSCheckInterval)
                {
                    cache.hasLineOfSight = CheckLineOfSight(observerPos, sourcePos);
                    cache.timeSinceLastCheck = 0.0f;
                }

                if (!cache.hasLineOfSight) continue;
            }

            bool found = false;
            for (auto &perceived : perception.perceivedStimuli)
            {
                if (perceived.sourceEntity == sourceID.ID && perceived.type == StimulusType::Sight)
                {
                    perceived.location = sourcePos;
                    perceived.age = 0.0f;
                    perceived.strength = 1.0f - (distance / effectiveRange);
                    perceived.successfullySensed = true;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                PerceivedStimulus newStimulus;
                newStimulus.sourceEntity = sourceID.ID;
                newStimulus.type = StimulusType::Sight;
                newStimulus.location = sourcePos;
                newStimulus.age = 0.0f;
                newStimulus.strength = 1.0f - (distance / effectiveRange);
                newStimulus.tag = stimulus.tag;
                newStimulus.successfullySensed = true;

                perception.perceivedStimuli.push_back(newStimulus);

                auto event = eastl::make_shared<PerceptionEvent>();
                event->type = PerceptionEventType::StimulusPerceived;
                event->observerEntity = observerID.ID;
                event->stimulusEntity = sourceID.ID;
                event->stimulusType = StimulusType::Sight;
                event->location = sourcePos;
                s_perceptionEventQueue.enqueue(event);

                if (perception.IsPriorityTag(stimulus.tag))
                {
                    auto priorityEvent = eastl::make_shared<PerceptionEvent>();
                    priorityEvent->type = PerceptionEventType::EnemySpotted;
                    priorityEvent->observerEntity = observerID.ID;
                    priorityEvent->stimulusEntity = sourceID.ID;
                    priorityEvent->stimulusType = StimulusType::Sight;
                    priorityEvent->location = sourcePos;
                    s_perceptionEventQueue.enqueue(priorityEvent);
                }
            }
        }
    }
}

void PerceptionSubsystem::ProcessSoundStimuli(float dt)
{
    Scene &scene = *Engine::GetActiveScene();

    auto observers = scene.GetAllEntitiesWith<IDComponent, TransformComponent, PerceptionComponent>();
    auto stimuliSources = scene.GetAllEntitiesWith<IDComponent, TransformComponent, StimulusComponent>();

    for (const auto &[observerEntityHandle, observerID, observerTransform, perception] : observers.each())
    {
        if (!perception.enabled || !perception.enableSound) continue;

        if (!ShouldUpdatePerception(perception, dt)) continue;

        Entity observerEntity = scene.GetEntityWithUUID(observerID.ID);
        Vec3 observerPos = scene.GetWorldSpaceTransform(observerEntity).GetTranslation();

        // Постоянные звуки
        for (const auto &[sourceEntityHandle, sourceID, sourceTransform, stimulus] : stimuliSources.each())
        {
            if (observerEntityHandle == sourceEntityHandle) continue;

            if (!stimulus.enabled || !stimulus.enableSound) continue;

            Entity sourceEntity = scene.GetEntityWithUUID(sourceID.ID);
            Vec3 sourcePos = scene.GetWorldSpaceTransform(sourceEntity).GetTranslation();

            float distance = (sourcePos - observerPos).Length();

            float effectiveRange = stimulus.soundRadius > 0.0f ? stimulus.soundRadius : perception.soundRange;

            if (distance > effectiveRange) continue;

            float strength = 1.0f - (distance / effectiveRange);

            if (strength < perception.soundMinStrength) continue;

            bool found = false;
            for (auto &perceived : perception.perceivedStimuli)
            {
                if (perceived.sourceEntity == sourceID.ID && perceived.type == StimulusType::Sound)
                {
                    perceived.location = sourcePos;
                    perceived.age = 0.0f;
                    perceived.strength = strength;
                    perceived.successfullySensed = true;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                PerceivedStimulus newStimulus;
                newStimulus.sourceEntity = sourceID.ID;
                newStimulus.type = StimulusType::Sound;
                newStimulus.location = sourcePos;
                newStimulus.age = 0.0f;
                newStimulus.strength = strength;
                newStimulus.tag = stimulus.tag;
                newStimulus.successfullySensed = true;

                perception.perceivedStimuli.push_back(newStimulus);
            }
        }

        // Временные звуки
        for (const auto &tempStimulus : m_temporaryStimuli)
        {
            if (tempStimulus.type != StimulusType::Sound) continue;

            float distance = (tempStimulus.location - observerPos).Length();

            if (distance > tempStimulus.radius) continue;

            float strength = 1.0f - (distance / tempStimulus.radius);

            if (strength < perception.soundMinStrength) continue;

            PerceivedStimulus newStimulus;
            newStimulus.sourceEntity = tempStimulus.sourceEntity;
            newStimulus.type = StimulusType::Sound;
            newStimulus.location = tempStimulus.location;
            newStimulus.age = 0.0f;
            newStimulus.strength = strength;
            newStimulus.tag = tempStimulus.tag;
            newStimulus.successfullySensed = true;

            perception.perceivedStimuli.push_back(newStimulus);
        }
    }
}

void PerceptionSubsystem::ProcessTouchStimuli()
{
    Scene &scene = *Engine::GetActiveScene();
    auto observers = scene.GetAllEntitiesWith<IDComponent, TransformComponent, PerceptionComponent>();

    for (const auto &[observerEntityHandle, observerID, observerTransform, perception] : observers.each())
    {
        if (!perception.enabled/* || !perception.enableTouch*/) continue; // FIXME: enableTouch всегда false

        Entity observerEntity = scene.GetEntityWithUUID(observerID.ID);
        Vec3 observerPos = scene.GetWorldSpaceTransform(observerEntity).GetTranslation();

        for (const auto &tempStimulus : m_temporaryStimuli)
        {
            if (tempStimulus.type != StimulusType::Touch) continue;

            if (perception.ShouldIgnoreTag(tempStimulus.tag)) continue;

            float distance = (tempStimulus.location - observerPos).Length();

            bool found = false;
            for (auto &perceived : perception.perceivedStimuli)
            {
                if (perceived.type == StimulusType::Touch && perceived.age < 0.1f)
                {
                    perceived.location = tempStimulus.location;
                    perceived.age = 0.0f;
                    perceived.strength = 1.0f;
                    perceived.successfullySensed = true;
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                PerceivedStimulus newStimulus;
                newStimulus.sourceEntity = tempStimulus.sourceEntity;
                newStimulus.type = StimulusType::Touch;
                newStimulus.location = tempStimulus.location;
                newStimulus.age = 0.0f;
                newStimulus.strength = 1.0f;
                newStimulus.tag = tempStimulus.tag;
                newStimulus.successfullySensed = true;

                perception.perceivedStimuli.push_back(newStimulus);

                auto event = eastl::make_shared<PerceptionEvent>();
                event->type = PerceptionEventType::StimulusPerceived;
                event->observerEntity = observerID.ID;
                event->stimulusEntity = tempStimulus.sourceEntity;
                event->stimulusType = StimulusType::Touch;
                event->location = tempStimulus.location;
                s_perceptionEventQueue.enqueue(event);

                if (perception.IsPriorityTag(tempStimulus.tag))
                {
                    auto priorityEvent = eastl::make_shared<PerceptionEvent>();
                    priorityEvent->type = PerceptionEventType::EnemySpotted;
                    priorityEvent->observerEntity = observerID.ID;
                    priorityEvent->stimulusEntity = tempStimulus.sourceEntity;
                    priorityEvent->stimulusType = StimulusType::Touch;
                    priorityEvent->location = tempStimulus.location;
                    s_perceptionEventQueue.enqueue(priorityEvent);
                }
            }
        }
    }
}

void PerceptionSubsystem::TouchListener(const eastl::shared_ptr<PhysicsEvent> &event)
{
    Scene &scene = *Engine::GetActiveScene();
    auto entity1 = scene.GetEntityWithUUID(event->entity1);
    auto entity2 = scene.GetEntityWithUUID(event->entity2);

    if (!entity1.IsValid() || !entity2.IsValid()) return;

    Vec3 pos1 = scene.GetWorldSpaceTransform(entity1).GetTranslation();
    Vec3 pos2 = scene.GetWorldSpaceTransform(entity2).GetTranslation();

    eastl::string tag1 = "Unknown";
    eastl::string tag2 = "Unknown";

    if (entity1.HasComponent<StimulusComponent>()) tag1 = entity1.GetComponent<StimulusComponent>().tag;
    if (entity2.HasComponent<StimulusComponent>()) tag2 = entity2.GetComponent<StimulusComponent>().tag;

    GetInstance().RegisterStimulus(entity2.GetUUID(), StimulusType::Touch, pos1, 0.0f, tag2);
    GetInstance().RegisterStimulus(entity1.GetUUID(), StimulusType::Touch, pos2, 0.0f, tag1);
}

void PerceptionSubsystem::UpdateStimuliAge(float dt)
{
    Scene &scene = *Engine::GetActiveScene();
    auto view = scene.GetAllEntitiesWith<IDComponent, PerceptionComponent>();

    for (const auto &[entityHandle, idComp, perception] : view.each())
    {
        for (auto it = perception.perceivedStimuli.begin(); it != perception.perceivedStimuli.end();)
        {
            it->age += dt;

            float forgetTime = 5.0f;
            switch (it->type)
            {
            case StimulusType::Sight:
                forgetTime = perception.sightForgetTime;
                break;
            case StimulusType::Sound:
                forgetTime = perception.soundForgetTime;
                break;
            case StimulusType::Touch:
                forgetTime = perception.touchForgetTime;
                break;
            case StimulusType::Damage:
                forgetTime = perception.damageForgetTime;
                break;
            default:
                break;
            }

            if (it->age >= forgetTime)
            {
                auto event = eastl::make_shared<PerceptionEvent>();
                event->type = PerceptionEventType::StimulusForgotten;
                event->observerEntity = idComp.ID;
                event->stimulusEntity = it->sourceEntity;
                event->stimulusType = it->type;
                event->location = it->location;
                s_perceptionEventQueue.enqueue(event);

                it = perception.perceivedStimuli.erase(it);
                continue;
            }

            ++it;
        }
    }
}

void PerceptionSubsystem::UpdateLOD()
{
    Scene &scene = *Engine::GetActiveScene();

    Vec3 cameraPos{0.0f};
    auto cameras = scene.GetAllEntitiesWith<IDComponent, TransformComponent, CameraComponent>();

    for (const auto &[entityHandle, idComp, transform, camera] : cameras.each())
    {
        if (camera.CameraPriority == 0)
        {
            Entity cameraEntity = scene.GetEntityWithUUID(idComp.ID);
            cameraPos = scene.GetWorldSpaceTransform(cameraEntity).GetTranslation();
            break;
        }
    }

    auto view = scene.GetAllEntitiesWith<IDComponent, TransformComponent, PerceptionComponent>();

    for (const auto &[entityHandle, idComp, transform, perception] : view.each())
    {
        Entity entity = scene.GetEntityWithUUID(idComp.ID);
        Vec3 entityPos = scene.GetWorldSpaceTransform(entity).GetTranslation();

        float distance = (entityPos - cameraPos).Length();
        perception.cachedDistanceToCamera = distance;

        perception.updateInterval = CalculateUpdateInterval(distance);
    }
}

bool PerceptionSubsystem::ShouldUpdatePerception(PerceptionComponent &perception, float dt)
{
    if (!m_settings.enableLOD) return true;

    if (perception.updateInterval <= 0.0f) return true;

    perception.timeSinceLastUpdate += dt;

    if (perception.timeSinceLastUpdate >= perception.updateInterval)
    {
        perception.timeSinceLastUpdate = 0.0f;
        return true;
    }

    return false;
}

float PerceptionSubsystem::CalculateUpdateInterval(float distanceToCamera)
{
    if (distanceToCamera < m_settings.lodNearDistance) return m_settings.lodNearUpdateInterval;
    else if (distanceToCamera < m_settings.lodMidDistance) return m_settings.lodMidUpdateInterval;
    else if (distanceToCamera < m_settings.lodFarDistance) return m_settings.lodFarUpdateInterval;
    else return 1.0f; // Очень далеко 1 секунда
}

bool PerceptionSubsystem::CheckLineOfSight(const Vec3 &from, const Vec3 &to)
{
    Vec3 direction = to - from;
    float distance = direction.Length();

    if (distance < 0.01f) return true;

    eastl::optional<RayCastResult> result = PhysicsSubsystem::CastRay(from, direction); // TODO: убрать попадание в себя

    if (!result) return true; // Если ничего не попало то есть видимость

    // Проверка что попали в цель и не раньше
    RayCastResult rayCastResult = result.value();
    return rayCastResult.distance >= (distance - 0.1f);
}

bool PerceptionSubsystem::IsInFieldOfView(const Vec3 &observerPos, const Quat &observerRotation, const Vec3 &targetPos,
                                          float fovAngle)
{
    Vec3 forward = GetForwardVector(observerRotation);
    Vec3 toTarget = targetPos - observerPos;

    if (toTarget.LengthSquared() < 0.01f) return true;

    toTarget.Normalize();

    float dotProduct = forward.Dot(toTarget);
    float angleInRadians = acosf(dotProduct);
    float angleInDegrees = DirectX::XMConvertToDegrees(angleInRadians);

    return angleInDegrees <= fovAngle;
}

Vec3 PerceptionSubsystem::GetForwardVector(const Quat &rotation)
{
    Vec3 forward(0.0f, 0.0f, 1.0f);
    return Vec3::Transform(forward, rotation);
}

void PerceptionSubsystem::RegisterStimulus(uuid sourceEntity, StimulusType type, const Vec3 &location, float radius,
                                           const eastl::string &tag)
{
    TemporaryStimulus stimulus;
    stimulus.sourceEntity = sourceEntity;
    stimulus.type = type;
    stimulus.location = location;
    stimulus.radius = radius;
    stimulus.tag = tag;
    stimulus.lifetime = 0.1f; // Один кадр
    stimulus.age = 0.0f;

    m_temporaryStimuli.push_back(stimulus);
}

void PerceptionSubsystem::CreateAttachPerceptionComponent(Entity entity)
{
    PerceptionComponent *componentPtr = entity.TryGetComponent<PerceptionComponent>();
    if (componentPtr)
    {
        BF_ERROR("Perception component error: entity " + entity.GetUUID().str() + " already has PerceptionComponent");
        return;
    }

    entity.AddComponent<PerceptionComponent>();
}

void PerceptionSubsystem::DestroyPerceptionComponent(Entity entity)
{
    PerceptionComponent *componentPtr = entity.TryGetComponent<PerceptionComponent>();
    if (!componentPtr) return;

    entity.RemoveComponent<PerceptionComponent>();
}

void PerceptionSubsystem::CreateAttachStimulusComponent(Entity entity)
{
    StimulusComponent *componentPtr = entity.TryGetComponent<StimulusComponent>();
    if (componentPtr)
    {
        BF_ERROR("Stimulus component error: entity " + entity.GetUUID().str() + " already has StimulusComponent");
        return;
    }

    entity.AddComponent<StimulusComponent>();
}

void PerceptionSubsystem::DestroyStimulusComponent(Entity entity)
{
    StimulusComponent *componentPtr = entity.TryGetComponent<StimulusComponent>();
    if (!componentPtr) return;

    entity.RemoveComponent<StimulusComponent>();
}

PerceptionSubsystem::PerceptionEventHandle PerceptionSubsystem::AddEventListener(
    const PerceptionEventType eventType, eastl::function<void(const PerceptionEventPointer &)> listener)
{
    return s_perceptionEventQueue.appendListener(eventType, listener);
}

void PerceptionSubsystem::RemoveEventListener(const PerceptionEventType eventType, const PerceptionEventHandle &handle)
{
    s_perceptionEventQueue.removeListener(eventType, handle);
}

void PerceptionSubsystem::ProcessEvents()
{
    s_perceptionEventQueue.process();
}

} // namespace Blainn