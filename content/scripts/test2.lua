
a = 20

function OnStart()
    Log.Info("Test2 onStart called")
    Input.AddEventListener(InputEventType.KeyPressed, 
        function(ev)
        Log.Info("key pressed listener from script: " .. tostring(ev.key))
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