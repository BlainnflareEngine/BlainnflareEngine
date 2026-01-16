function OnStart()

end

function OnUpdate(deltaTime)
    local scene = Engine.GetActiveScene()
    local owningEntity = scene:TryGetEntityWithUUID(OwningEntity)

    if not owningEntity then
        return
    end 

    if not owningEntity:HasTransformComponent() then 
        return
    end

    transform = owningEntity:GetTransformComponent()
    rotation = transform:GetRotationEuler()
    rotation.x = rotation.x + deltaTime * 1
    transform:SetRotationEuler(rotation)
end

function OnDestroy()
    --
end