#include "pch.h"

#include "aliases.h"
#include "scripting/TypeRegistration.h"
#include "Engine.h"
#include "ai/BTBuilder.h"
#include "ai/Blackboard.h"

#include "components/AIControllerComponent.h"
#include "components/PerceptionComponent.h"
#include "components/StimulusComponent.h"
#include "subsystems/PerceptionSubsystem.h"


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
    auto BlackboardType = luaState.new_usertype<Blackboard>("Blackboard", sol::no_constructor);

    BlackboardType.set_function("Set", 
        [](Blackboard* bb, const std::string& key, sol::object value)
        {
            if (!bb) return;
            bb->Set(key.c_str(), value);
        }
    );
    
    BlackboardType.set_function("GetInt", 
        [](Blackboard *bb, const std::string &key) -> int
        {
            if (!bb) return 0;
            return bb->Get<int>(key.c_str());
        }
    );
    
    BlackboardType.set_function("GetFloat", 
        [](Blackboard *bb, const std::string &key) -> float
        {
            if (!bb) return 0.0f;
            return bb->Get<float>(key.c_str());
        }
    );
    
    BlackboardType.set_function("GetDouble", 
        [](Blackboard *bb, const std::string &key) -> double
        {
            if (!bb) return 0.0;
            return bb->Get<double>(key.c_str());
        }
    );
    
    BlackboardType.set_function("GetBool", 
        [](Blackboard *bb, const std::string &key) -> bool
        {
            if (!bb) return false;
            return bb->Get<bool>(key.c_str());
        }
    );
    
    BlackboardType.set_function("GetString", 
        [](Blackboard *bb, const std::string &key) -> std::string
        {
            if (!bb) return "";
            return bb->Get<eastl::string>(key.c_str()).c_str();
        }
    );
    
    BlackboardType.set_function("Has", 
        [](Blackboard *bb, const std::string &key) -> bool
        {
            if (!bb) return false;
            return bb->Has(key.c_str());
        }
    );

    // Expose BTType to Lua
    luaState.new_enum<BTType, true>(
    "BTType", 
    {
        {"Sequence", BTType::Sequence},
        {"Selector", BTType::Selector},
        {"Action", BTType::Action},
        {"Negate", BTType::Negate},
        {"Condition", BTType::Condition},
        {"Error", BTType::Error}
    }
    );

    luaState.new_enum<BTStatus, true>(
    "BTStatus",
    {
        {"Success", BTStatus::Success},
        {"Failure", BTStatus::Failure},
        {"Running", BTStatus::Running},
        {"Aborted", BTStatus::Aborted},
        {"Error", BTStatus::Error}
    }
    );
    
    // Expose StimulusType enum
    luaState.new_enum<StimulusType, true>(
    "StimulusType",
    {
        {"Sight", StimulusType::Sight}, 
        {"Sound", StimulusType::Sound}, 
        {"Touch", StimulusType::Touch},
        {"Damage", StimulusType::Damage}, 
        {"Custom", StimulusType::Custom}
    }
    );

    // Expose AIControllerComponent

    auto AIControllerComponentType = luaState.new_usertype<AIControllerComponent>("AIControllerComponent", sol::no_constructor);

    AIControllerComponentType.set_function("GetMovementSpeed", [](AIControllerComponent &comp) { return comp.MovementSpeed; });
    AIControllerComponentType.set_function("GetStoppingDistance", [](AIControllerComponent &comp) { return comp.StoppingDistance; });
        AIControllerComponentType.set_function("SetMovementSpeed", [](AIControllerComponent &comp, float newMovementSpeed) { comp.MovementSpeed = newMovementSpeed; });
    AIControllerComponentType.set_function("SetStoppingDistance", [](AIControllerComponent &comp, float newStopDist) { comp.StoppingDistance = newStopDist; });
    
    // Expose PerceivedStimulus
    auto PerceivedStimulusType = luaState.new_usertype<PerceivedStimulus>("PerceivedStimulus",sol::no_constructor);

    PerceivedStimulusType.set_function("GetSourceEntity", [](PerceivedStimulus &perceived) { return perceived.sourceEntity; });
    PerceivedStimulusType.set_function("GetType", [](PerceivedStimulus &perceived) { return perceived.type; });
    PerceivedStimulusType.set_function("GetLocation", [](PerceivedStimulus &perceived) { return perceived.location; });
    PerceivedStimulusType.set_function("GetAge", [](PerceivedStimulus &perceived) { return perceived.age; });
    PerceivedStimulusType.set_function("GetStrength", [](PerceivedStimulus &perceived) { return perceived.strength; });
    PerceivedStimulusType.set_function("GetSuccessfullySensed",[](PerceivedStimulus &perceived) { return perceived.successfullySensed; });
    PerceivedStimulusType.set_function("GetStimulusTag", [](PerceivedStimulus &perceived) { return perceived.tag.c_str(); });
    
    PerceivedStimulusType.set_function("SetSourceEntity", [](PerceivedStimulus &perceived, uuid ID) { perceived.sourceEntity = ID; });
    PerceivedStimulusType.set_function("SetType", [](PerceivedStimulus &perceived, StimulusType type) { perceived.type = type; });
    PerceivedStimulusType.set_function("SetLocation", [](PerceivedStimulus &perceived, Vec3 newLoc) { perceived.location = newLoc; });
    PerceivedStimulusType.set_function("SetAge", [](PerceivedStimulus &perceived, float newAge) { perceived.age = newAge; });
    PerceivedStimulusType.set_function("SetStrength", [](PerceivedStimulus &perceived, float newStrength) { perceived.strength = newStrength; });
    PerceivedStimulusType.set_function("SetSuccessfullySensed",[](PerceivedStimulus &perceived, bool res) { perceived.successfullySensed = res; });
    PerceivedStimulusType.set_function("SetStimulusTag", [](PerceivedStimulus &perceived, const std::string &tag) { perceived.tag = tag.c_str(); });
    
    // Expose PerceptionComponent
    auto PerceptionComponentType = luaState.new_usertype<PerceptionComponent>("PerceptionComponent", sol::no_constructor);

    PerceptionComponentType.set_function("GetEnableSight", [](PerceptionComponent &perceived) { return perceived.enableSight; });
    PerceptionComponentType.set_function("GetSightRange", [](PerceptionComponent &perceived) { return perceived.sightRange; });
    PerceptionComponentType.set_function("GetSightFOV", [](PerceptionComponent &perceived) { return perceived.sightFOV; });
    PerceptionComponentType.set_function("GetSightForgetTime", [](PerceptionComponent &perceived) { return perceived.sightForgetTime; });
    PerceptionComponentType.set_function("GetSightRequireLOS", [](PerceptionComponent &perceived) { return perceived.sightRequireLOS; });
    PerceptionComponentType.set_function("GetSightLOSCheckInterval", [](PerceptionComponent &perceived) { return perceived.sightLOSCheckInterval; });

    PerceptionComponentType.set_function("GetEnableSound", [](PerceptionComponent &perceived) { return perceived.enableSound; });
    PerceptionComponentType.set_function("GetSoundRange", [](PerceptionComponent &perceived) { return perceived.soundRange; });
    PerceptionComponentType.set_function("GetSoundForgetTime", [](PerceptionComponent &perceived) { return perceived.soundForgetTime; });
    PerceptionComponentType.set_function("GetSoundMinStrength", [](PerceptionComponent &perceived) { return perceived.soundMinStrength; });

    PerceptionComponentType.set_function("GetEnableTouch", [](PerceptionComponent &perceived) { return perceived.enableTouch; });
    PerceptionComponentType.set_function("GetTouchForgetTime", [](PerceptionComponent &perceived) { return perceived.touchForgetTime; });

    PerceptionComponentType.set_function("GetEnableDamage", [](PerceptionComponent &perceived) { return perceived.enableDamage; });
    PerceptionComponentType.set_function("GetDamageForgetTime", [](PerceptionComponent &perceived) { return perceived.damageForgetTime; });

    PerceptionComponentType.set_function("GetUpdateInterval", [](PerceptionComponent &perceived) { return perceived.updateInterval; });
    PerceptionComponentType.set_function("GetMaxUpdateDistance", [](PerceptionComponent &perceived) { return perceived.maxUpdateDistance; });

    PerceptionComponentType.set_function("GetEnabled", [](PerceptionComponent &perceived) { return perceived.enabled; });

    PerceptionComponentType.set_function("GetIgnoreTags", 
                                        [&luaState](PerceptionComponent &perception) 
                                        { 
                                            sol::table tbl = luaState.create_table();
                                            int idx = 1;
                                            for (const auto &tag : perception.ignoreTags)
                                                tbl[idx++] = tag.c_str();
                                            return tbl;
                                        });
    
    PerceptionComponentType.set_function("GetPriorityTags", 
                                    [&luaState](PerceptionComponent &perception) 
                                    { 
                                        sol::table tbl = luaState.create_table();
                                        int idx = 1;
                                        for (const auto &tag : perception.priorityTags)
                                            tbl[idx++] = tag.c_str();
                                        return tbl;
                                    });
    
    PerceptionComponentType.set_function("SetEnableSight", [](PerceptionComponent &perception, bool enable) { perception.enableSight = enable; });
    PerceptionComponentType.set_function("SetSightRange", [](PerceptionComponent &perception, float range){ perception.sightRange = range; });
    PerceptionComponentType.set_function("SetSightFOV",[](PerceptionComponent &perception, float fov) { perception.sightFOV = fov; });
    PerceptionComponentType.set_function("SetSightForgetTime", [](PerceptionComponent &perception, float time) { perception.sightForgetTime = time; });
    PerceptionComponentType.set_function("SetSightRequireLOS", [](PerceptionComponent &perception, bool require) { perception.sightRequireLOS = require; });
    PerceptionComponentType.set_function("SetSightLOSCheckInterval", [](PerceptionComponent &perception, float interval) { perception.sightLOSCheckInterval = interval; });

    PerceptionComponentType.set_function("SetEnableSound", [](PerceptionComponent &perception, bool enable) { perception.enableSound = enable; });
    PerceptionComponentType.set_function("SetSoundRange", [](PerceptionComponent &perception, float range) { perception.soundRange = range; });
    PerceptionComponentType.set_function("SetSoundForgetTime", [](PerceptionComponent &perception, float time) { perception.soundForgetTime = time; });
    PerceptionComponentType.set_function("SetSoundMinStrength", [](PerceptionComponent &perception, float strength) { perception.soundMinStrength = strength; });

    PerceptionComponentType.set_function("SetEnableTouch", [](PerceptionComponent &perception, bool enable) { perception.enableTouch = enable; });
    PerceptionComponentType.set_function("SetTouchForgetTime", [](PerceptionComponent &perception, float time) { perception.touchForgetTime = time; });

    PerceptionComponentType.set_function("SetEnableDamage", [](PerceptionComponent &perception, bool enable) { perception.enableDamage = enable; });
    PerceptionComponentType.set_function("SetDamageForgetTime", [](PerceptionComponent &perception, float time) { perception.damageForgetTime = time; });

    PerceptionComponentType.set_function("SetUpdateInterval", [](PerceptionComponent &perception, float interval) { perception.updateInterval = interval; });
    PerceptionComponentType.set_function("SetMaxUpdateDistance", [](PerceptionComponent &perception, float distance) { perception.maxUpdateDistance = distance; });

    PerceptionComponentType.set_function("SetEnabled", [](PerceptionComponent &perception, bool enabled) { perception.enabled = enabled; });

    PerceptionComponentType.set_function("SetIgnoreTags",
                                         [](PerceptionComponent &perception, sol::table tags)
                                         {
                                             perception.ignoreTags.clear();
                                             for (const auto &pair : tags)
                                             {
                                                 if (pair.second.is<std::string>())
                                                     perception.ignoreTags.push_back(
                                                         pair.second.as<std::string>().c_str());
                                             }
                                         });

    PerceptionComponentType.set_function("SetPriorityTags",
                                         [](PerceptionComponent &perception, sol::table tags)
                                         {
                                             perception.priorityTags.clear();
                                             for (const auto &pair : tags)
                                             {
                                                 if (pair.second.is<std::string>())
                                                     perception.priorityTags.push_back(
                                                         pair.second.as<std::string>().c_str());
                                             }
                                         });

    PerceptionComponentType.set_function("AddIgnoreTag", [](PerceptionComponent &perception, const std::string &tag) { perception.ignoreTags.push_back(tag.c_str()); });
    PerceptionComponentType.set_function("AddPriorityTag", [](PerceptionComponent &perception, const std::string &tag) { perception.priorityTags.push_back(tag.c_str()); });
    PerceptionComponentType.set_function("ClearIgnoreTags", [](PerceptionComponent &perception) { perception.ignoreTags.clear(); });
    PerceptionComponentType.set_function("ClearPriorityTags", [](PerceptionComponent &perception) { perception.priorityTags.clear(); });

    // Expose StimulusComponent
    auto StimulusComponentType = luaState.new_usertype<StimulusComponent>("StimulusComponent", sol::no_constructor);
    
    StimulusComponentType.set_function("GetEnableSight", [](StimulusComponent &stimulus) { return stimulus.enableSight; });
    StimulusComponentType.set_function("GetEnableSound", [](StimulusComponent &stimulus) { return stimulus.enableSound; });
    StimulusComponentType.set_function("GetEnableTouch", [](StimulusComponent &stimulus) { return stimulus.enableTouch; });
    StimulusComponentType.set_function("GetEnableDamage", [](StimulusComponent &stimulus) { return stimulus.enableDamage; });
    StimulusComponentType.set_function("GetSightRadius", [](StimulusComponent &stimulus) { return stimulus.sightRadius; });
    StimulusComponentType.set_function("GetSoundRadius", [](StimulusComponent &stimulus) { return stimulus.soundRadius; });
    StimulusComponentType.set_function("GetEnabled", [](StimulusComponent &stimulus) { return stimulus.enabled; });
    StimulusComponentType.set_function("GetStimulusTag", [](StimulusComponent &stimulusComponent) { return stimulusComponent.tag.c_str(); });
    
    StimulusComponentType.set_function("SetEnableSight", [](StimulusComponent &stimulus, bool enable) { stimulus.enableSight = enable; });
    StimulusComponentType.set_function("SetEnableSound", [](StimulusComponent &stimulus, bool enable) { stimulus.enableSound = enable; });
    StimulusComponentType.set_function("SetEnableTouch", [](StimulusComponent &stimulus, bool enable) { stimulus.enableTouch = enable; });
    StimulusComponentType.set_function("SetEnableDamage", [](StimulusComponent &stimulus, bool enable) { stimulus.enableDamage = enable; });
    StimulusComponentType.set_function("SetSightRadius", [](StimulusComponent &stimulus, float radius) { stimulus.sightRadius = radius; });
    StimulusComponentType.set_function("SetSoundRadius", [](StimulusComponent &stimulus, float radius) { stimulus.soundRadius = radius; });
    StimulusComponentType.set_function("SetEnabled", [](StimulusComponent &stimulus, bool enabled) { stimulus.enabled = enabled; });
    StimulusComponentType.set_function("SetStimulusTag", [](StimulusComponent &stimulus, const std::string& tag) { stimulus.tag = tag.c_str(); });

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

    luaState.set_function("GetClosestStimulus",
                          [&luaState](Blackboard *bb, StimulusType type) -> sol::object
                          {
                              auto *perception = GetPerception(bb);
                              if (!perception) return sol::nil;

                              uuid selfEntity = bb->Get<uuid>("selfEntity");
                              Scene *scene = Engine::GetActiveScene().get();
                              if (!scene) return sol::nil;

                              Entity self = scene->GetEntityWithUUID(selfEntity);
                              if (!self.IsValid()) return sol::nil;

                              Vec3 selfPos = scene->GetWorldSpaceTransform(self).GetTranslation();

                              PerceivedStimulus *closest = nullptr;
                              float minDistance = FLT_MAX;

                              auto stimuli = GetStimuliByType(bb, type);
                              for (auto *stimulus : stimuli)
                              {
                                  float distance = (stimulus->location - selfPos).Length();
                                  if (distance < minDistance)
                                  {
                                      minDistance = distance;
                                      closest = stimulus;
                                  }
                              }

                              if (closest)
                              {
                                  sol::state_view lua(luaState);
                                  return sol::make_object(lua, *closest);
                              }
                              return sol::nil;
                          });
    
    luaState.set_function("GetStimulusInRadius",
                          [&luaState](Blackboard *bb, StimulusType type, float radius) -> sol::table
                          {
                              sol::state_view lua(luaState);
                              sol::table result = lua.create_table();

                              auto *perception = GetPerception(bb);
                              if (!perception) return result;

                              uuid selfEntity = bb->Get<uuid>("selfEntity");
                              Scene *scene = Engine::GetActiveScene().get();
                              if (!scene) return result;

                              Entity self = scene->GetEntityWithUUID(selfEntity);
                              if (!self.IsValid()) return result;

                              Vec3 selfPos = scene->GetWorldSpaceTransform(self).GetTranslation();

                              auto stimuli = GetStimuliByType(bb, type);
                              int idx = 1;
                              for (auto *stimulus : stimuli)
                              {
                                  float distance = (stimulus->location - selfPos).Length();
                                  if (distance <= radius)
                                  {
                                      result[idx++] = *stimulus;
                                  }
                              }

                              return result;
                          });

    luaState.set_function("HasStimulusOfType",
                          [](Blackboard *bb, StimulusType type) -> bool
                          {
                              auto *perception = GetPerception(bb);
                              if (!perception) return false;

                              for (auto &stimulus : perception->perceivedStimuli)
                              {
                                  if (stimulus.type == type && stimulus.successfullySensed) return true;
                              }
                              return false;
                          });

    luaState.set_function("ClearStimuliByType",
                          [](Blackboard *bb, StimulusType type)
                          {
                              auto *perception = GetPerception(bb);
                              if (!perception) return;

                              perception->perceivedStimuli.erase(eastl::remove_if(perception->perceivedStimuli.begin(),
                                                                                  perception->perceivedStimuli.end(),
                                                                                  [type](const PerceivedStimulus &s)
                                                                                  { return s.type == type; }),
                                                                 perception->perceivedStimuli.end());
                          });
}
#endif