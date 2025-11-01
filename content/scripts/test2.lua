
function OnStart()
    print("Test2 onStart called")
end

function OnUpdate(deltaTime)
    print("Test2 onUpdate called, delta time is:", deltaTime)

end

function OnCustomCall(b)
    print("Test2 onCustomCall called")
    b = b + 3
end