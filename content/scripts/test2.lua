

function OnStart()
    print("Test2 onStart called")
    Input.AddEventListener(InputEventType.KeyPressed, function(ev)
    print("key pressed listener from script:", ev.key)
    end)
end

function OnUpdate(deltaTime)
    --print("Test2 onUpdate called, delta time is:", deltaTime)
    --print(InputEventType["KeyPressed"])
    value = 0
    for i = 1, 100 do
        value = value + i
    end
end

function OnCustomCall(b)
    print("Test2 onCustomCall called")
    b = b + 3
end