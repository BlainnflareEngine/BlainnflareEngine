
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
            local scene = Engine:GetActiveScene()
            local e = scene:TryGetEntityWithUUID(OwningEntity)
            if not e:HasTransformComponent() then
                Log.Error(e:GetTagComponent().Tag)
                Log.Error("The entity doesn't have a transform component")
                return
            end
            local direction = 0
            if ev.key == Key.A then 
                direction = -1
            end
            if ev.key == Key.D then
                direction = 1
            end
            local t = e:GetTransformComponent():GetTranslation()
            local oldX = t.x
            t.x = oldX + (0.1 * direction)
            e:GetTransformComponent():SetTranslation(t)
        end
    )

end

function OnUpdate(deltaTime)
end

function OnDestroy()
    Log.Info("Test2 onDestroy called")
    Input.RemoveEventListener(InputEventType.KeyHeld, listener1Handle)
    Input.RemoveEventListener(InputEventType.KeyHeld, listener2Handle)
end
