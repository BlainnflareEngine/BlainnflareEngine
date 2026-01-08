
function OnStart()
    local scene = Engine.GetActiveScene()
    local e = scene:GetEntityWithUUID(OwningEntity)

    -- ensure transform exists
    if not e:HasTransformComponent() then
        e:AddTransformComponent()
    end

    -- ensure physics component exists (so collisions will fire)
    if not Physics.HasPhysicsComponent(e) then
        Physics.CreateAttachPhysicsComponent(e, ComponentShapeType.Box)
        local updater = Physics.GetBodyUpdater(e)
        if updater then
            updater.SetBoxShapeSettings(Vec3:new(0.5, 0.5, 0.5))
            updater.SetMotionType(PhysicsComponentMotionType.Static, EActivation.DontActivate)
            updater.SetPosition(e:GetTransformComponent():GetTranslation(), EActivation.DontActivate)
        end
    end
end


local spawnedEntity = nil

function OnCollisionStarted(event)

    local scene = Engine.GetActiveScene()

    local e1uuid = event.entity1
    local e2uuid = event.entity2
    e1 = scene:GetEntityWithUUID(e1uuid)
    e2 = scene:GetEntityWithUUID(e2uuid)

    local other = nil

    if e1uuid == OwningEntity then other = e2 
    elseif e2uuid == OwningEntity then other = e1 
    end

    if not other then return end

    -- If other has PlayerEntity tag, spawn new entity if not existing
    if other:HasTagComponent() and other:GetTagComponent().Tag == "PlayerEntity" then
        if spawnedEntity == nil then
            local spawn = scene:CreateEntity("ButtonSpawned", true)
            -- add transform and set to owner position + (0,0,2)
            spawn:AddTransformComponent()
            local owner = scene:GetEntityWithUUID(OwningEntity)
            local ox, oy, oz = 0, 0, 0
            if owner and owner:HasTransformComponent() then
                local opt = owner:GetTransformComponent():GetTranslation()
                ox = opt.x or opt[1] or 0
                oy = opt.y or opt[2] or 0
                oz = opt.z or opt[3] or 0
            end
            local sx = ox
            local sy = oy
            local sz = oz + 2
            spawn:GetTransformComponent():SetTranslation(Vec3:new(sx, sy, sz))

            -- attach a simple mesh (use engine example path)
            pcall(function()
                scene:CreateAttachMeshComponent(spawn, "Models/Cube.fbx")
            end)

            spawnedEntity = spawn
        end
    end
end

function OnCollisionEnded(event)
    local scene = Engine.GetActiveScene()
    local e1uuid = event.entity1
    local e2uuid = event.entity2
    local e1 = scene:GetEntityWithUUID(e1uuid)
    local e2 = scene:GetEntityWithUUID(e2uuid)

    local other = nil
    if e1uuid == OwningEntity then other = e2
    elseif e2uuid == OwningEntity then other = e1
    end

    if not other then return end

    if other:HasTagComponent() and other:GetTagComponent().Tag == "PlayerEntity" then
        if spawnedEntity ~= nil then
            scene:DestroyEntity(spawnedEntity)
            spawnedEntity = nil
        end
    end
end

function OnDestroy()
    if spawnedEntity ~= nil then
        local scene = Engine.GetActiveScene()
        if scene then scene:DestroyEntity(spawnedEntity) end
        spawnedEntity = nil
    end
end