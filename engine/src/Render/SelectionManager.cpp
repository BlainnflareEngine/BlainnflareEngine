#include "Render/SelectionManager.h"

#include "Input/MouseEvents.h"

namespace Blainn
{
	SelectionManager::SelectionManager()
	{

	}

	void SelectionManager::Init()
    {
        if (!m_bIsMouseClickActionsBinded)
        {
            m_inputEvents.push_back({
                Input::AddEventListener(InputEventType::MouseButtonPressed,
                        [this](const InputEventPointer &event)
                        {
                            const MouseButtonPressedEvent *mouseEvent = static_cast<const MouseButtonPressedEvent*>(event.get());                                 
                            auto mouseButton = mouseEvent->GetMouseButton();
                        }),
                        InputEventType::MouseButtonPressed
                });
            m_bIsMouseClickActionsBinded = true;
        }
    }
	void SelectionManager::Destroy()
	{
        RemoveInputEventListeners();

		m_selectedEntities.clear();
	}

	void SelectionManager::SetupInputEventListeners()
	{

	}
	
	void SelectionManager::RemoveInputEventListeners()
    {
		for (auto& [handle, type] : m_inputEvents)
		{
            Input::RemoveEventListener(type, handle);
		}

        m_inputEvents.clear();
    }
}