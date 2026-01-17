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

            local x = pos.x
            local y = pos.y
            local z = pos.z

            if event.key == Key.W then
                z = z + speed
            end
            if event.key == Key.S then
                z = z - speed
            end
            if event.key == Key.A then
                x = x - speed
            end
            if event.key == Key.D then
                x = x + speed
            end
            tc:SetTranslation(Vec3:new(x, y, z))
        end
    )

    listenerPressedHandle = Input.AddEventListener(InputEventType.KeyPressed,
        function(event)
            if event.key ~= Key.Space then return end

            local scene = Engine.GetActiveScene()
            local e = scene:TryGetEntityWithUUID(OwningEntity)
            if not e:IsValid() then
                Log.Error("aaa banana invalid 2")
                return
            end

            local updater = Physics.GetBodyUpdater(e)
            updater.AddImpulse(Vec3:new(0.0, JUMP_IMPULSE, 0.0))
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
