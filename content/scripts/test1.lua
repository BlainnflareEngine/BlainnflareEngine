function OnStart()
    print("Test1 onStart called")
    local a = Vec2:new(3, 4)
    local b = Vec2:new(1, 2)
    print("vector a in lua:", a.x, a.y)
 
    local result = a + b
    result:Normalize() 

    print("normalized vector sum in lua:", result.x, result.y)
end

function OnDestroy()
    print("Test1 onDestroy called")
end