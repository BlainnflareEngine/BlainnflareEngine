function VectorTest()
    local a = Vec2:new(3, 4)
    local b = Vec2:new(1, 2)
    print("vector a in lua:", a.x, a.y)
 
    local result = a + b
    result:Normalize() 

    print("normalized vector sum in lua:", result.x, result.y)
end

function OnStart()
    print("Test1 onStart called")
    VectorTest()
end

function OnUpdate(deltaTime)
    print(a)
--     VectorTest()
--     value = 0
--     for i = 1, 100 do
--         value = value + i
--     end
end

function OnDestroy()
    print("Test1 onDestroy called")
end