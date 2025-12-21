
local listener1Handle = nil
local listener2Handle = nil

function OnStart()
     Log.Info("script start OwningEntity: " .. OwningEntity)

    Log.Info("Test2 onStart called")
    listener1Handle = Input.AddEventListener(InputEventType.KeyHeld,
        function(ev)
        Log.Info("key pressed listener from script: " .. tostring(ev.key))
        end
    )

    listener2Handle = Input.AddEventListener(InputEventType.KeyHeld,
        function(ev)
            Log.Info("script listener lambda OwningEntity: " .. OwningEntity)
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

function OnDestroy()
    Log.Info("Test2 onDestroy called")
    Input.RemoveEventListener(InputEventType.KeyHeld, listener1Handle)
    Input.RemoveEventListener(InputEventType.KeyHeld, listener2Handle)
end

function OnCustomCall(b)
    Log.Info("Test2 onCustomCall called")
    b = b + 3
end