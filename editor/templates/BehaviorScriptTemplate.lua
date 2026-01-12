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

    -- Condition example
    -- {
        --  type = 5, -- Condition
        -- fn = function(bb)
        -- ...
        -- end
    -- }

Blackboard = 
{
    target_point = { 1.0, 2.0, 1.0 },
    cooldown = 0.5,
    enemy_visible = false,
    playerTag = "Player"
}

BehaviourTrees = 
{
    Idle = 
    {
        name = "Idle",
        type = 1, -- Sequence
        children =
        {
            {
                type = 3, -- Action
                fn = function (Blackboard)
                    if Blackboard.abort_requested then
                        return "aborted"
                    end

                    print("Idle")
                    if Blackboard.enemy_visible == true then
                        return "success"
                    end
                    return "running"
                end,

                onReset = function (Blackboard)
                    return true
                end
            }
        }
    },

    Chase =
    {
        name = "Chase",
        type = 2, -- Selector
        children =
        {
            {
                type = 3, -- Action
                fn = function (Blackboard)
                    if Blackboard.abort_requested then
                        return "aborted"
                    end

                    print("Chase player")
                    local entity = 
                    GetEntitiesWithTagComponent(Blackboard.playerTag)
                    MoveTo(entity)
                    if Blackboard.enemy_visible == false then
                        return "success"
                    end
                    return "running"
                end,

                onReset = function (Blackboard)
                    MoveTo(Blackboard.target_point)
                    return true
                end
            }
        }

    }
}

UtilityAI =
{
    normalize = true,
    hysteresis = 0.2,

    decisions =
    {
        {
            name = "Idle",
            bt   = "Idle",

            score = function(bb)
                return 0.1
            end
        },

        {
            name = "Chase",
            bt   = "Chase",

            cooldown = 1.0,
            score = function(bb)
                return bb.enemy_visible and 1.0 or 0.0
            end
        }
    }
}