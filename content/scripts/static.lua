
function OnStart()
	local scene = Engine.GetActiveScene()
	if not scene then return end

	local e = scene:GetEntityWithUUID(OwningEntity)
	if not e then return end

	-- Ensure Transform component exists
	if not e:HasTransformComponent() then
		e:AddTransformComponent()
	end

	local tc = e:GetTransformComponent()
	if tc then
		-- Set translation y = -3
		local pos = tc:GetTranslation()
		local x = pos.x or pos[1] or 0
		local y = -3
		local z = pos.z or pos[3] or 0

		tc:SetTranslation(Vec3:new(x, y, z))
		tc:SetScale(Vec3:new(10.0, 1.0, 10.0))

	end

	-- Ensure Physics component exists and make it a static box via body updater
	if not Physics.HasPhysicsComponent(e) then
		Physics.CreateAttachPhysicsComponent(e, ComponentShapeType.Box)
	end

	if Physics.HasPhysicsComponent(e) then
		local updater = Physics.GetBodyUpdater(e)
		if updater then

			updater.SetBoxShapeSettings(Vec3:new(5.0, 0.5, 5.0))
            
			-- Make static and don't activate
			updater.SetMotionType(PhysicsComponentMotionType.Static, EActivation.Activate)
			-- Align physics body with transform
			if tc then
				updater.SetPosition(tc:GetTranslation(), EActivation.Activate)
				updater.SetScale(tc:GetScale(), tc:GetScale())
			end
		end
	end
end

function OnUpdate(dt)
	-- no-op
end

