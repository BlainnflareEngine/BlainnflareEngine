//
// WhoLeb
//
#include "pch.h"

#include "scene/Entity.h"
#include "scene/Scene.h"

using namespace Blainn;

bool Entity::IsValid() const
{
    return m_EntityHandle != entt::null && m_Scene && m_Scene->m_Registry.valid(m_EntityHandle);
}


Entity::operator bool() const
{
    return IsValid();
}


uuid Entity::GetSceneUUID() const
{
    return m_Scene->m_SceneID;
}


Entity Entity::GetParent() const
{
    return m_Scene->TryGetEntityWithUUID(GetParentUUID());
}


void Entity::SetParent(Entity parent)
{
    Entity currentParent = GetParent();
    if (currentParent == parent) return;

    if (currentParent) currentParent.RemoveChild(*this);

    if (parent)
    {
        SetParentUUID(parent.GetUUID());
        auto &parentChildren = parent.Children();
        uuid _uuid = GetUUID();
        if (eastl::find(parentChildren.begin(), parentChildren.end(), _uuid) == parentChildren.end())
            parentChildren.emplace_back(GetUUID());
    }
    else
    {
        SetParentUUID(uuid{});
    }

    m_Scene->ReportEntityReparent(*this);
}


bool Entity::RemoveChild(Entity child)
{
    uuid childId = child.GetUUID();
    auto &children = Children();
    auto it = eastl::find(children.begin(), children.end(), childId);
    if (it)
    {
        children.erase(it);
        m_Scene->ReportEntityReparent(child);
        return true;
    }
    return false;
}


bool Entity::IsAncestorOf(Entity entity)
{
    const auto &children = Children();
    uuid childId = entity.GetUUID();

    if (children.empty()) return false;

    for (uuid child : children)
        if (child == childId) return true;

    for (uuid child : children)
        if (m_Scene->GetEntityWithUUID(child).IsAncestorOf(entity)) return true;

    return false;
}
