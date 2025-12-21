#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

#include "Input/InputEvent.h"
#include "Input/InputSubsystem.h"
#include "Input/KeyCodes.h"
#include "Input/KeyboardEvents.h"
#include "Input/MouseEvents.h"
#include "subsystems/ScriptingSubsystem.h"
#include <atomic>
#include <functional>
#include <unordered_map>

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES

// Listener handle storage for Lua -> Input event listeners
static inline std::unordered_map<uint64_t, std::function<void()>> s_inputListenerRemovers;
static inline std::atomic<uint64_t> s_inputNextListenerId{1};

void Blainn::RegisterInputTypes(sol::state &luaState)
{
    luaState.new_enum<true>("InputEventType", "MouseMoved", InputEventType::MouseMoved, "MouseButtonPressed",
                            InputEventType::MouseButtonPressed, "MouseButtonReleased", InputEventType::MouseButtonHeld,
                            "MouseButtonHeld", InputEventType::MouseButtonReleased, "MouseScrolled",
                            InputEventType::MouseScrolled, "KeyPressed", InputEventType::KeyPressed, "KeyReleased",
                            InputEventType::KeyReleased, "KeyHeld", InputEventType::KeyHeld);

    luaState.new_enum<true>("MouseButton", "Left", MouseButton::Left, "Right", MouseButton::Right, "Middle",
                            MouseButton::Middle, "X1", MouseButton::X1, "X2", MouseButton::X2);

    sol::table inputTable = luaState.create_table();

    auto make_event_table = [](const InputEventPointer &ev, sol::state_view lua)
    {
        sol::table tbl = lua.create_table();
        tbl["type"] = static_cast<int>(ev->GetEventType());

        switch (ev->GetEventType())
        {
        case InputEventType::MouseMoved:
        {
            MouseMovedEvent *me = dynamic_cast<MouseMovedEvent *>(ev.get());
            if (me)
            {
                tbl["x"] = me->GetX();
                tbl["y"] = me->GetY();
            }
            break;
        }
        case InputEventType::MouseScrolled:
        {
            MouseScrolledEvent *se = dynamic_cast<MouseScrolledEvent *>(ev.get());
            if (se)
            {
                tbl["xOffset"] = se->GetXOffset();
                tbl["yOffset"] = se->GetYOffset();
            }
            break;
        }
        case InputEventType::MouseButtonPressed:
        case InputEventType::MouseButtonReleased:
        case InputEventType::MouseButtonHeld:
        {
            MouseButtonEvent *mbe = dynamic_cast<MouseButtonEvent *>(ev.get());
            if (mbe)
            {
                tbl["button"] = static_cast<int>(mbe->GetMouseButton());
            }
            break;
        }
        case InputEventType::KeyPressed:
        case InputEventType::KeyReleased:
        case InputEventType::KeyHeld:
        {
            KeyboardEvent *kbe = dynamic_cast<KeyboardEvent *>(ev.get());
            if (kbe)
            {
                tbl["key"] = static_cast<int>(kbe->GetKey());
            }
            break;
        }
        default:
            break;
        }

        return tbl;
    };

    auto add_listener_func = [&luaState, make_event_table](int eventTypeInt, sol::function listener) -> uint64_t
    {
        InputEventType eventType = static_cast<InputEventType>(eventTypeInt);
        sol::function luaListener = listener;
        uint64_t id = s_inputNextListenerId.fetch_add(1);

        auto handle =
            Blainn::Input::AddEventListener(eventType,
                                            [&luaState, luaListener, make_event_table](const InputEventPointer &ev)
                                            {
                                                sol::state_view lua(luaState);
                                                sol::table tbl = make_event_table(ev, lua);

                                                sol::protected_function pfunc = luaListener;
                                                sol::protected_function_result result = pfunc(tbl);
                                                if (!result.valid())
                                                {
                                                    sol::error err = result;
                                                    BF_ERROR("Lua input listener error: " + eastl::string(err.what()));
                                                }
                                            });
        // store remover lambda capturing handle
        s_inputListenerRemovers[id] = [eventType, handle]() { Blainn::Input::RemoveEventListener(eventType, handle); };
        return id;
    };

    inputTable.set_function("AddEventListener", add_listener_func);

    inputTable.set_function("RemoveEventListener",
                            [](int eventTypeInt, uint64_t id)
                            {
                                auto it = s_inputListenerRemovers.find(id);
                                if (it == s_inputListenerRemovers.end()) return;
                                // call stored remover
                                it->second();
                                s_inputListenerRemovers.erase(it);
                            });
    luaState["Input"] = inputTable;
}

#endif