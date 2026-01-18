//
// Created by gorev on 21.11.2025.
//

#pragma once
#include "EASTL/shared_ptr.h"
#include "EASTL/unique_ptr.h"
#include "EntityNode.h"
#include "SceneMeta.h"

namespace editor
{
class EntityNodeComparator
{
public:
    EntityNodeComparator(const eastl::shared_ptr<SceneMeta> &meta)
        : m_meta(meta)
    {
    }

    EntityNodeComparator(const QString &metaPath)
        : m_meta(eastl::make_shared<SceneMeta>(metaPath))
    {
    }

    bool operator()(EntityNode *a, EntityNode *b) const
    {
        if (!m_meta) return false;
        if (!a->GetEntity().IsValid() || !b->GetEntity().IsValid()) return false;

        int posA = m_meta->GetPositionInEditorOrder(a->GetEntity().GetUUID());
        int posB = m_meta->GetPositionInEditorOrder(b->GetEntity().GetUUID());

        if (posA != -1 && posB != -1)
        {
            return posA < posB;
        }

        if (posA != -1) return true;

        if (posB != -1) return false;

        return false;
    }

private:
    eastl::shared_ptr<SceneMeta> m_meta;
};
} // namespace editor
