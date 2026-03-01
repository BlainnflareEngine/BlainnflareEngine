function OnStart()

end

function OnUpdate(deltaTime)
    local owningEntity = OwningEntity
    if not owningEntity or not owningEntity:IsValid() then
        return
    end

    if not owningEntity:HasTransformComponent() then
        return
    end

    local transform = owningEntity:GetTransformComponent()
    local rotation = transform:GetRotationEuler()
    rotation.y = rotation.y + (deltaTime * 0.001)
    transform:SetRotationEuler(rotation)
end

function OnDestroy()
    --
end
