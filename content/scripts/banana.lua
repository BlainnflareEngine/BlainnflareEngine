-- создайте камеру в движке и повесьте на нее скрипт
-- готово, вы банан, умеете прыгать и бегать

local savedDeltaTime = 0.0167

local listenerHeldHandle = nil
local listenerPressedHandle = nil

-- movement settings
local MOVE_SPEED = 5.0
local JUMP_IMPULSE = 5000.0

function OnStart()
   local scene = Engine.GetActiveScene()
    local e = scene:TryGetEntityWithUUID(OwningEntity)
    if not e:IsValid() then
        Log.Error("aaa banana invalid")
        return
    end

    if e:HasTagComponent() then
        e:RemoveTagComponent()
    end
    e:AddTagComponent("PlayerEntity")

    if not Physics.HasPhysicsComponent(e) then
        Physics.CreateAttachPhysicsComponent(e, ComponentShapeType.Box)
        Log.Info("Physics component created and attached to entity: " .. e:GetTagComponent().Tag)
    end

    if e:HasCameraComponent() then
        Log.Warn("i have camera")
        local cc = e:GetCameraComponent()
        -- cc:SetFarPlane(1.0)
        -- cc:SetNearPlane(1.0)
        -- cc:GetPriority()
        -- cc:SetFovDegrees(1.0)
    end

    listenerHeldHandle = Input.AddEventListener(InputEventType.KeyHeld,
        function(event)

            if not e then return end

            if not e:HasTransformComponent() then
                return
            end

            local tc = e:GetTransformComponent()

            local pos = tc:GetTranslation()

            local dt = savedDeltaTime or 0.0167
            local speed = MOVE_SPEED * dt

            local forward = tc:GetForwardVector()
            local right = tc:GetRightVector()

            local deltaPos = Vec3:new(0,0,0)

            if event.key == Key.W then
                deltaPos = deltaPos + forward * speed
            end
            if event.key == Key.S then
                deltaPos = deltaPos - forward * speed
            end
            if event.key == Key.A then
                deltaPos = deltaPos - right * speed
            end
            if event.key == Key.D then
                deltaPos = deltaPos + right * speed
            end
            if event.key == Key.Q then
                local rotationDelta = Vec3:new(0.0, -1.0 * speed, 0.0)
                tc:Rotate(rotationDelta)
            end
            if event.key == Key.E then
                local rotationDelta = Vec3:new(0.0, 1.0 * speed, 0.0)
                tc:Rotate(rotationDelta)
            end

            pos = pos + deltaPos
            tc:SetTranslation(pos)
        end
    )

    listenerPressedHandle = Input.AddEventListener(InputEventType.KeyPressed,
        function(event)

            local scene = Engine.GetActiveScene()
            local e = scene:TryGetEntityWithUUID(OwningEntity)
            if not e:IsValid() then
                Log.Error("aaa banana invalid 2")
                return
            end

            if event.key == Key.Space then
                local updater = Physics.GetBodyUpdater(e)
                updater.AddImpulse(Vec3:new(0.0, JUMP_IMPULSE, 0.0))
            end

            if event.key == Key.G then
                if (Engine.GetActiveScene():GetName() == "Scene.scene")
                then
                    AssetManager.OpenScene("Navmesh.scene")
                else
                    AssetManager.OpenScene("Scene.scene")
                end
            end
        end
    )
end

function OnUpdate(deltaTime)
    savedDeltaTime = deltaTime

    local scene = Engine.GetActiveScene()
    local e = scene:TryGetEntityWithTag("knopka")
    if not e:IsValid() then
        return
    end

    local scriptUuid = Scripting.ListScripts(e)[1]
    local val = Scripting.GetValueFromScript(scriptUuid, "abobus")
    if val == nil then
        Log.Warn("abobus is nil for script " .. tostring(scriptUuid))
    else
        Log.Warn(tostring(val))
    end
    Scripting.SetValueInScript(scriptUuid,"abobus", "sobaka")
end

function OnDestroy()
    if listenerHeldHandle ~= nil then
    Input.RemoveEventListener(InputEventType.KeyHeld, listenerHeldHandle)
    end
    if listenerPressedHandle ~= nil then
    Input.RemoveEventListener(InputEventType.KeyPressed, listenerPressedHandle)
    end
end
