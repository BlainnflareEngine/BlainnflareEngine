//
// Created by gorev on 26.01.2026.
//

#include "EditorRegistry.h"

namespace editor
{
eastl::vector<eastl::pair<entt::id_type, WidgetFactoryFn>> g_widgetRegistry;
}