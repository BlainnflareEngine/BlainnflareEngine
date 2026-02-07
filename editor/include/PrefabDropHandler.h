//
// Created by gorev on 06.02.2026.
//

#pragma once
#include "Log.h"
#include "MimeFormats.h"


#include <QMimeData>

namespace editor
{
inline void HandlePrefabDrop(const QMimeData *data)
{
    if (data->hasFormat(MIME_PREFAB))
    {
        BF_DEBUG("Has prefab mime format!");

        auto prefabs = eastl::move(DecodePrefabData(data));

        for (auto prefab : prefabs)
        {
            YAML::Node prefabData = YAML::LoadFile(ToString(prefab.absolutePath));
            Blainn::Engine::GetSceneManager().GetActiveScene()->CreatePrefabEntity(prefabData["Entities"]);
        }
    }
}
} // namespace editor