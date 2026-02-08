function ConfigurePerception(perception)
    perception:SetEnableSight(true)
    perception:SetSightRange(1000.0)
end

function ConfigureStimulus(stimulus)
    stimulus:SetEnableSight(true)
    stimulus:SetStimulusTag("TestAI")
end

local scene = Engine.GetActiveScene()
local ai = GetAIController(OwningEntity:GetUUID())

if not OwningEntity:IsValid() then
    Log.Error("AI is invalid")
    return
end
local tc = OwningEntity:GetTransformComponent()

Blackboard =
{
    target_point = Vec3:new( 1.0, 2.0, 1.0 ),
    cooldown = 0.5,
    enemy_visible = false,
    playerTag = "Player",
    startedPatroling = false
}

BehaviourTrees = 
{
    Patrol = {
        name = "Patrol",
        type = BTType.Sequence,
        children = {
            {
                type = BTType.Action,
                fn = function(bb)

                    if bb:GetBool("btAbortRequested") then
                        return "aborted"
                    end
                    
                    if bb:GetBool("startedPatroling") and ai:IsMoving() then

                        Log.Info("Is moving")
                        return "running"

                    elseif not bb:GetBool("startedPatroling") and not ai:IsMoving() then

                        bb:Set("startedPatroling", true)
                        local point = Vec3:new()
                        local isFound, point = Navigation.FindRandomPointOnNavMeshInRadius(point, tc:GetTranslation(), 2)

                        if not isFound then
                            Log.Warn("No random point was found")
                            return "failure"
                        end

                        Log.Info("Patroling to " .. point.x .. " " .. point.y .. " " .. point.z)
                        ai:MoveTo(point)
                        return "running"

                    end
                    
                    bb:Set("startedPatroling", false)
                    bb:Set("enemy_visible", true)

                    Log.Info("Enemy visible " .. tostring(bb:GetBool("enemy_visible")))
                    
                    return "success"
                end,

                onReset = function()
                    Log.Info("Patrol reset")
                end
            }
        }
    },

    -- Дерево с Condition
    Chase = {
        name = "Chase",
        type = BTType.Sequence,
        children = {
            -- Condition как обычный узел
            {
                type = BTType.Condition,
                fn = function(bb)
                    -- Проверяем условие
                    return true
                end,
                children = {
                    {
                        type = BTType.Action,
                        fn = function(bb)
                            if bb:GetBool("btAbortRequested") then
                                return "aborted"
                            end
                            
                            if not bb:GetBool("enemy_visible") then
                                return "failure"  -- Failure
                            end
                            
                            Log.Info("Chasing enemy!")
                            return "running"  -- Running
                        end,
                        onReset = function()
                            Log.Info("Chase reset")
                        end
                    }
                }
            }
        }
    },
}

Utility =
{
    normalize = true,
    hysteresis = 0.2,

    decisions =
    {
        {
            name = "Patrol",
            bt   = "Patrol",

            score = function(bb)
                Log.Info("Utility patrol score")
                return 0.1
            end
        },

        {
            name = "Chase",
            bt   = "Chase",

            cooldown = 1.0,
            score = function(bb)
                Log.Info("Utility chase score")
                return bb:GetBool("enemy_visible") and 1.0 or 0.0
            end
        }
    }
}

    -- Sequence  = 1,
    -- Selector  = 2,
    -- Action    = 3,
    -- Negate    = 4,
    -- Condition = 5,
    -- Error     = 6,

    -- Action return
    --  integer: 0=Failure, 1=Success, 2=Running, 3=Aborted OR
    --  string: "failure"/"success"/"running"/"aborted", OR
    --  nothing => treated as Error