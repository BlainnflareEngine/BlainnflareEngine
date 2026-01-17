function ConfigurePerception(perception)
    perception:SetEnableSight(true)
    perception:SetSightRange(1000.0)
end

function ConfigureStimulus(stimulus)
    stimulus:SetEnableSight(true)
    stimulus:SetStimulusTag("TestAI")
end

Blackboard =
{
    target_point = { 1.0, 2.0, 1.0 },
    cooldown = 0.5,
    enemy_visible = false,
    playerTag = "Player"
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
                    if Blackboard.btAbortRequested then
                        return "aborted"
                    end

                    Log.Info("Patrolling...")
                    return "success"  -- Success
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
                fn = function(Blackboard)
                    -- Проверяем условие
                    return true
                end,
                children = {
                    {
                        type = BTType.Action,
                        fn = function(bb)
                            if Blackboard.btAbortRequested then
                                return "aborted"
                            end
                            
                            if bb.enemy_visible then
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

            score = function(Blackboard)
                Log.Info("Utility patrol score")
                return 0.1
            end
        },

        {
            name = "Chase",
            bt   = "Chase",

            cooldown = 1.0,
            score = function(Blackboard)
                Log.Info("Utility chase score")
                return Blackboard.enemy_visible and 1.0 or 0.0
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