//
// Created by gorev on 26.01.2026.
//

#include "EditorRegistry.h"

namespace editor
{
eastl::vector<eastl::pair<entt::id_type, WidgetFactoryFn>> &GetWidgetRegistry()
{
    static eastl::vector<eastl::pair<entt::id_type, WidgetFactoryFn>> widgetRegistry;
    return widgetRegistry;
}
}
