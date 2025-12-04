
local testsPassed = 0
local testsFailed = 0

local function ok(name, cond)
    if cond then
        Log.Info("PASS: " .. name)
        testsPassed = testsPassed + 1
    else
        Log.Info("FAIL: " .. name)
        testsFailed = testsFailed + 1
    end
end

function OnStart()
    Log.Info("Test2 onStart called")
    Input.AddEventListener(InputEventType.KeyPressed,
        function(ev)
        Log.Info("key pressed listener from script: " .. tostring(ev.key))
        end
    )

    Input.AddEventListener(InputEventType.KeyPressed,
        function(ev)
            local scene = Engine.GetActiveScene()
            local e = scene:GetEntityWithUUID(OwningEntity)
            if not e:HasTransformComponent() then
                Log.Error(e:GetTagComponent().Tag)
                Log.Error("The entity doesn't have a transform component")
                return
            end
            local t = e:GetTransformComponent()
            local oldX = t.Translation.x
            t.Translation.x = oldX + 0.1
        end
    )

end

function OnUpdate(deltaTime)
    value = 0
    for i = 1, 100 do
        value = value + i
    end
end

function OnCustomCall(b)
    Log.Info("Test2 onCustomCall called")
    b = b + 3
end