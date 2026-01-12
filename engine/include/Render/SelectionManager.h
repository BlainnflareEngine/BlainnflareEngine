#pragma once

#include "Render/DXHelpers.h"

#include "Input/InputSubsystem.h"

#include "aliases.h"
#include "helpers.h"

#include <EASTL/vector.h>

namespace Blainn
{
	class SelectionManager
	{
    public:
        SelectionManager();
        NO_COPY_DEFAULT_MOVE(SelectionManager);

        void Init();
        void Destroy();

    private:
        void SetupInputEventListeners();
        void RemoveInputEventListeners();

    private:
		eastl::vector<uuid> m_selectedEntities;
        eastl::vector<eastl::pair<Input::EventHandle, InputEventType>> m_inputEvents;

        bool m_bIsMouseClickActionsBinded = false;
	};
}