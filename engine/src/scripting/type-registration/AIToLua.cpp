#include "pch.h"

#include "aliases.h"
#include "scripting/TypeRegistration.h"
#include "ai/BTBuilder.h"

#include "components/PerceptionComponent.h"
#include "components/StimulusComponent.h"
#include "subsystems/PerceptionSubsystem.h"
#include "ai/Blackboard.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

namespace
{
    PerceptionComponent* GetPerception(Blackboard* bb)
    {
        if (!bb) return nullptr;
        return bb->Get<PerceptionComponent*>("_perception");
    }
    
    eastl::vector<PerceivedStimulus*> GetStimuliByType(Blackboard* bb, StimulusType type)
    {
        eastl::vector<PerceivedStimulus*> result;
        auto* perception = GetPerception(bb);
        if (!perception) return result;
        
        for (auto& stimulus : perception->perceivedStimuli)
        {
            if (stimulus.type == type && stimulus.successfullySensed)
                result.push_back(&stimulus);
        }
        return result;
    }
    
    eastl::vector<PerceivedStimulus*> GetStimuliByTag(Blackboard* bb, const eastl::string& tag)
    {
        eastl::vector<PerceivedStimulus*> result;
        auto* perception = GetPerception(bb);
        if (!perception) return result;
        
        for (auto& stimulus : perception->perceivedStimuli)
        {
            if (stimulus.tag == tag && stimulus.successfullySensed)
                result.push_back(&stimulus);
        }
        return result;
    }
}

void Blainn::RegisterAITypes(sol::state &luaState)
{
    // Expose BTType to Lua
    luaState.new_enum<true>("BTType", 
        "Sequence", BTType::Sequence, 
        "Selector", BTType::Selector, 
        "Action", BTType::Action, 
        "Negate", BTType::Negate, 
        "Condition", BTType::Condition, 
        "Error", BTType::Error
    );
    
    // Expose StimulusType enum
    luaState.new_enum<true>("StimulusType",
        "Sight", StimulusType::Sight,
        "Sound", StimulusType::Sound,
        "Touch", StimulusType::Touch,
        "Damage", StimulusType::Damage,
        "Custom", StimulusType::Custom
    );
    
    // Expose PerceivedStimulus
    auto PerceivedStimulusType = luaState.new_usertype<PerceivedStimulus>("PerceivedStimulus",sol::no_constructor);

    PerceivedStimulusType.set_function("SourceEntity", &PerceivedStimulus::sourceEntity);
    PerceivedStimulusType.set_function("Type", &PerceivedStimulus::type);
    PerceivedStimulusType.set_function("Location", &PerceivedStimulus::location);
    PerceivedStimulusType.set_function("Age", &PerceivedStimulus::age);
    PerceivedStimulusType.set_function("Strength", &PerceivedStimulus::strength);
    PerceivedStimulusType.set_function("SuccessfullySensed", &PerceivedStimulus::successfullySensed);
    PerceivedStimulusType.set_function("StimulusTag", [](PerceivedStimulus &perceived) { return perceived.tag.c_str(); });
    
    // Expose PerceptionComponent
    auto PerceptionComponentType = luaState.new_usertype<PerceptionComponent>("PerceptionComponent", sol::no_constructor);

    PerceptionComponentType.set_function("EnableSight", &PerceptionComponent::enableSight);
    PerceptionComponentType.set_function("SightRange", &PerceptionComponent::sightRange);
    PerceptionComponentType.set_function("SightFOV", &PerceptionComponent::sightFOV);
    PerceptionComponentType.set_function("SightForgetTime", &PerceptionComponent::sightForgetTime);
    PerceptionComponentType.set_function("SightRequireLOS", &PerceptionComponent::sightRequireLOS);
    PerceptionComponentType.set_function("SightLOSCheckInterval", &PerceptionComponent::sightLOSCheckInterval);

    PerceptionComponentType.set_function("EnableSound", &PerceptionComponent::enableSound);
    PerceptionComponentType.set_function("SoundRange", &PerceptionComponent::soundRange);
    PerceptionComponentType.set_function("SoundForgetTime", &PerceptionComponent::soundForgetTime);
    PerceptionComponentType.set_function("SoundMinStrength", &PerceptionComponent::soundMinStrength);

    PerceptionComponentType.set_function("EnableTouch", &PerceptionComponent::enableTouch);
    PerceptionComponentType.set_function("TouchForgetTime", &PerceptionComponent::touchForgetTime);

    PerceptionComponentType.set_function("EnableDamage", &PerceptionComponent::enableDamage);
    PerceptionComponentType.set_function("DamageForgetTime", &PerceptionComponent::damageForgetTime);

    PerceptionComponentType.set_function("UpdateInterval", &PerceptionComponent::updateInterval);
    PerceptionComponentType.set_function("MaxUpdateDistance", &PerceptionComponent::maxUpdateDistance);

    PerceptionComponentType.set_function("Enabled", &PerceptionComponent::enabled);

    PerceptionComponentType.set_function("IgnoreTags", 
                                        [&luaState](PerceptionComponent &perception) 
                                        { 
                                            sol::table tbl = luaState.create_table();
                                            int idx = 1;
                                            for (const auto &tag : perception.ignoreTags)
                                                tbl[idx++] = tag.c_str();
                                            return tbl;
                                        });
    
    PerceptionComponentType.set_function("PriorityTags", 
                                    [&luaState](PerceptionComponent &perception) 
                                    { 
                                        sol::table tbl = luaState.create_table();
                                        int idx = 1;
                                        for (const auto &tag : perception.priorityTags)
                                            tbl[idx++] = tag.c_str();
                                        return tbl;
                                    });
    
    // Expose StimulusComponent
    auto StimulusComponentType = luaState.new_usertype<StimulusComponent>("StimulusComponent", sol::no_constructor);

    StimulusComponentType.set_function("EnableSight", &PerceptionComponent::enableSight);
    StimulusComponentType.set_function("EnableSound", &StimulusComponent::enableSound);
    StimulusComponentType.set_function("EnableTouch", &StimulusComponent::enableTouch);
    StimulusComponentType.set_function("EnableDamage", &StimulusComponent::enableDamage);
    StimulusComponentType.set_function("SightRadius", &StimulusComponent::sightRadius);
    StimulusComponentType.set_function("SoundRadius", &StimulusComponent::soundRadius);
    StimulusComponentType.set_function("Enabled", &StimulusComponent::enabled);

    StimulusComponentType.set_function("StimulusTag", [](StimulusComponent &stimulusComponent) { return stimulusComponent.tag.c_str(); });
    
    luaState.set_function("HasEnemyInSight", 
        [](Blackboard* bb) -> bool
        {
            auto* perception = GetPerception(bb);
            if (!perception) return false;
            
            auto stimuli = GetStimuliByType(bb, StimulusType::Sight);
            for (auto* stimulus : stimuli)
            {
                if (perception->IsPriorityTag(stimulus->tag) && stimulus->age < 1.0f)
                    return true;
            }
            return false;
        }
    );
    
    luaState.set_function("GetNearestEnemy",
        [&luaState](Blackboard* bb) -> sol::object
        {
            auto* perception = GetPerception(bb);
            if (!perception)
                return sol::nil;
            
            PerceivedStimulus* nearest = nullptr;
            float minAge = FLT_MAX;
            
            auto stimuli = GetStimuliByType(bb, StimulusType::Sight);
            for (auto* stimulus : stimuli)
            {
                if (perception->IsPriorityTag(stimulus->tag) && stimulus->age < minAge)
                {
                    minAge = stimulus->age;
                    nearest = stimulus;
                }
            }
            
            if (nearest)
            {
                sol::state_view lua(luaState);
                return sol::make_object(lua, *nearest);
            }
            return sol::nil;
        }
    );
    
    luaState.set_function("GetLastHeardSound",
        [&luaState](Blackboard* bb) -> sol::object
        {
            auto* perception = GetPerception(bb);
            if (!perception)
                return sol::nil;
            
            PerceivedStimulus* newest = nullptr;
            float minAge = FLT_MAX;
            
            auto stimuli = GetStimuliByType(bb, StimulusType::Sound);
            for (auto* stimulus : stimuli)
            {
                if (stimulus->age < minAge)
                {
                    minAge = stimulus->age;
                    newest = stimulus;
                }
            }
            
            if (newest)
            {
                sol::state_view lua(luaState);
                return sol::make_object(lua, *newest);
            }
            return sol::nil;
        }
    );
    
    luaState.set_function("GetStimuliByType",
        [&luaState](Blackboard* bb, int typeInt) -> sol::table
        {
            sol::state_view lua(luaState);
            sol::table result = lua.create_table();
            
            StimulusType type = static_cast<StimulusType>(typeInt);
            auto stimuli = GetStimuliByType(bb, type);
            
            for (size_t i = 0; i < stimuli.size(); ++i)
            {
                result[i + 1] = *stimuli[i];
            }
            
            return result;
        }
    );
    
    luaState.set_function("GetStimuliByTag",
        [&luaState](Blackboard* bb, const eastl::string& tag) -> sol::table
        {
            sol::state_view lua(luaState);
            sol::table result = lua.create_table();
            
            auto stimuli = GetStimuliByTag(bb, tag);
            
            for (size_t i = 0; i < stimuli.size(); ++i)
            {
                result[i + 1] = *stimuli[i];
            }
            
            return result;
        }
    );
    
    luaState.set_function("WasDamaged",
        [](Blackboard* bb, float timeWindow) -> bool
        {
            auto* perception = GetPerception(bb);
            if (!perception) return false;
            
            auto stimuli = GetStimuliByType(bb, StimulusType::Damage);
            for (auto* stimulus : stimuli)
            {
                if (stimulus->age < timeWindow)
                    return true;
            }
            return false;
        }
    );
    
    luaState.set_function("WasTouched",
        [](Blackboard* bb, float timeWindow) -> bool
        {
            auto* perception = GetPerception(bb);
            if (!perception) return false;
            
            auto stimuli = GetStimuliByType(bb, StimulusType::Touch);
            for (auto* stimulus : stimuli)
            {
                if (stimulus->age < timeWindow)
                    return true;
            }
            return false;
        }
    );
    
    luaState.set_function("GetPerceptionsCount",
        [](Blackboard* bb) -> int
        {
            auto* perception = GetPerception(bb);
            if (!perception) return 0;
            
            return static_cast<int>(perception->perceivedStimuli.size());
        }
    );
    
    luaState.set_function("RegisterSoundStimulus",
        [](uuid entity, const Vec3& loc, float radius, const eastl::string& tag)
        {
            PerceptionSubsystem::GetInstance().RegisterStimulus(
                entity, StimulusType::Sound, loc, radius, tag
            );
        }
    );
    
    luaState.set_function("RegisterDamageStimulus",
        [](uuid entity, const Vec3& loc, const eastl::string& tag)
        {
            PerceptionSubsystem::GetInstance().RegisterStimulus(
                entity, StimulusType::Damage, loc, 0.0f, tag
            );
        }
    );
    
    luaState.set_function("RegisterTouchStimulus",
        [](uuid entity, const Vec3& loc, const eastl::string& tag)
        {
            PerceptionSubsystem::GetInstance().RegisterStimulus(
                entity, StimulusType::Touch, loc, 0.0f, tag
            );
        }
    );
}
#endif