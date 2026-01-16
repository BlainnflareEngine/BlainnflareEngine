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
    luaState.new_enum<InputEventType, true>("InputEventType", {
                                                {"MouseMoved",        InputEventType::MouseMoved},
                                                {"MouseButtonPressed",InputEventType::MouseButtonPressed},
                                                {"MouseButtonReleased",InputEventType::MouseButtonReleased},
                                                {"MouseButtonHeld",   InputEventType::MouseButtonHeld},
                                                {"MouseScrolled",     InputEventType::MouseScrolled},
                                                {"KeyPressed",        InputEventType::KeyPressed},
                                                {"KeyReleased",       InputEventType::KeyReleased},
                                                {"KeyHeld",           InputEventType::KeyHeld}
                                            });

    luaState.new_enum<MouseButton, true>("MouseButton", {
                                             {"Left",   MouseButton::Left},
                                             {"Right",  MouseButton::Right},
                                             {"Middle", MouseButton::Middle},
                                             {"X1",     MouseButton::X1},
                                             {"X2",     MouseButton::X2}
                                         });

    // Register Key codes
    luaState.new_enum<Key, true>("Key", {
    {"CtrlBrkPrcs",    KeyCode::CtrlBrkPrcs},
    {"BackSpace",      KeyCode::BackSpace},
    {"Tab",            KeyCode::Tab},
    {"Clear",          KeyCode::Clear},
    {"Enter",          KeyCode::Enter},
    {"Shift",          KeyCode::Shift},
    {"Control",        KeyCode::Control},
    {"Alt",            KeyCode::Alt},
    {"Pause",          KeyCode::Pause},
    {"CapsLock",       KeyCode::CapsLock},
    {"Kana",           KeyCode::Kana},
    {"Hangeul",        KeyCode::Hangeul},
    {"Hangul",         KeyCode::Hangul},
    {"Junju",          KeyCode::Junju},
    {"Final",          KeyCode::Final},
    {"Hanja",          KeyCode::Hanja},
    {"Kanji",          KeyCode::Kanji},
    {"Escape",         KeyCode::Escape},
    {"Convert",        KeyCode::Convert},
    {"NonConvert",     KeyCode::NonConvert},
    {"Accept",         KeyCode::Accept},
    {"ModeChange",     KeyCode::ModeChange},
    {"Space",          KeyCode::Space},
    {"PageUp",         KeyCode::PageUp},
    {"PageDown",       KeyCode::PageDown},
    {"End",            KeyCode::End},
    {"Home",           KeyCode::Home},
    {"LeftArrow",      KeyCode::LeftArrow},
    {"UpArrow",        KeyCode::UpArrow},
    {"RightArrow",     KeyCode::RightArrow},
    {"DownArrow",      KeyCode::DownArrow},
    {"Select",         KeyCode::Select},
    {"Print",          KeyCode::Print},
    {"Execute",        KeyCode::Execute},
    {"PrintScreen",    KeyCode::PrintScreen},
    {"Inser",          KeyCode::Inser},
    {"Delete",         KeyCode::Delete},
    {"Help",           KeyCode::Help},
    {"Num0",           KeyCode::Num0},
    {"Num1",           KeyCode::Num1},
    {"Num2",           KeyCode::Num2},
    {"Num3",           KeyCode::Num3},
    {"Num4",           KeyCode::Num4},
    {"Num5",           KeyCode::Num5},
    {"Num6",           KeyCode::Num6},
    {"Num7",           KeyCode::Num7},
    {"Num8",           KeyCode::Num8},
    {"Num9",           KeyCode::Num9},
    {"A",              KeyCode::A},
    {"B",              KeyCode::B},
    {"C",              KeyCode::C},
    {"D",              KeyCode::D},
    {"E",              KeyCode::E},
    {"F",              KeyCode::F},
    {"G",              KeyCode::G},
    {"H",              KeyCode::H},
    {"I",              KeyCode::I},
    {"J",              KeyCode::J},
    {"K",              KeyCode::K},
    {"L",              KeyCode::L},
    {"M",              KeyCode::M},
    {"N",              KeyCode::N},
    {"O",              KeyCode::O},
    {"P",              KeyCode::P},
    {"Q",              KeyCode::Q},
    {"R",              KeyCode::R},
    {"S",              KeyCode::S},
    {"T",              KeyCode::T},
    {"U",              KeyCode::U},
    {"V",              KeyCode::V},
    {"W",              KeyCode::W},
    {"X",              KeyCode::X},
    {"Y",              KeyCode::Y},
    {"Z",              KeyCode::Z},
    {"LeftWin",        KeyCode::LeftWin},
    {"RightWin",       KeyCode::RightWin},
    {"Apps",           KeyCode::Apps},
    {"Sleep",          KeyCode::Sleep},
    {"Numpad0",        KeyCode::Numpad0},
    {"Numpad1",        KeyCode::Numpad1},
    {"Numpad2",        KeyCode::Numpad2},
    {"Numpad3",        KeyCode::Numpad3},
    {"Numpad4",        KeyCode::Numpad4},
    {"Numpad5",        KeyCode::Numpad5},
    {"Numpad6",        KeyCode::Numpad6},
    {"Numpad7",        KeyCode::Numpad7},
    {"Numpad8",        KeyCode::Numpad8},
    {"Numpad9",        KeyCode::Numpad9},
    {"Multiply",       KeyCode::Multiply},
    {"Add",            KeyCode::Add},
    {"Separator",      KeyCode::Separator},
    {"Subtract",       KeyCode::Subtract},
    {"Decimal",        KeyCode::Decimal},
    {"Divide",         KeyCode::Divide},
    {"F1",             KeyCode::F1},
    {"F2",             KeyCode::F2},
    {"F3",             KeyCode::F3},
    {"F4",             KeyCode::F4},
    {"F5",             KeyCode::F5},
    {"F6",             KeyCode::F6},
    {"F7",             KeyCode::F7},
    {"F8",             KeyCode::F8},
    {"F9",             KeyCode::F9},
    {"F10",            KeyCode::F10},
    {"F11",            KeyCode::F11},
    {"F12",            KeyCode::F12},
    {"F13",            KeyCode::F13},
    {"F14",            KeyCode::F14},
    {"F15",            KeyCode::F15},
    {"F16",            KeyCode::F16},
    {"F17",            KeyCode::F17},
    {"F18",            KeyCode::F18},
    {"F19",            KeyCode::F19},
    {"F20",            KeyCode::F20},
    {"F21",            KeyCode::F21},
    {"F22",            KeyCode::F22},
    {"F23",            KeyCode::F23},
    {"F24",            KeyCode::F24},
    {"NavigationView", KeyCode::NavigationView},
    {"NavigationMenu", KeyCode::NavigationMenu},
    {"NavigationUp",   KeyCode::NavigationUp},
    {"NavigationDown", KeyCode::NavigationDown},
    {"NavigationLeft", KeyCode::NavigationLeft},
    {"NavigationRight",KeyCode::NavigationRight},
    {"NavigationAccept",KeyCode::NavigationAccept},
    {"NavigationCancel",KeyCode::NavigationCancel},
    {"NumLock",        KeyCode::NumLock},
    {"ScrollLock",     KeyCode::ScrollLock},
    {"NumpadEqual",    KeyCode::NumpadEqual},
    {"FJ_Jisho",       KeyCode::FJ_Jisho},
    {"FJ_Masshou",     KeyCode::FJ_Masshou},
    {"FJ_Touroku",     KeyCode::FJ_Touroku},
    {"FJ_Loya",        KeyCode::FJ_Loya},
    {"FJ_Roya",        KeyCode::FJ_Roya},
    {"LeftShift",      KeyCode::LeftShift},
    {"RightShift",     KeyCode::RightShift},
    {"LeftCtrl",       KeyCode::LeftCtrl},
    {"RightCtrl",      KeyCode::RightCtrl},
    {"LeftMenu",       KeyCode::LeftMenu},
    {"RightMenu",      KeyCode::RightMenu},
    {"BrowserBack",    KeyCode::BrowserBack},
    {"BrowserForward", KeyCode::BrowserForward},
    {"BrowserRefresh", KeyCode::BrowserRefresh},
    {"BrowserStop",    KeyCode::BrowserStop},
    {"BrowserSearch",  KeyCode::BrowserSearch},
    {"BrowserFavorites",KeyCode::BrowserFavorites},
    {"BrowserHome",    KeyCode::BrowserHome},
    {"VolumeMute",     KeyCode::VolumeMute},
    {"VolumeDown",     KeyCode::VolumeDown},
    {"VolumeUp",       KeyCode::VolumeUp},
    {"NextTrack",      KeyCode::NextTrack},
    {"PrevTrack",      KeyCode::PrevTrack},
    {"Stop",           KeyCode::Stop},
    {"PlayPause",      KeyCode::PlayPause},
    {"Mail",           KeyCode::Mail},
    {"MediaSelect",    KeyCode::MediaSelect},
    {"App1",           KeyCode::App1},
    {"App2",           KeyCode::App2},
    {"OEM1",           KeyCode::OEM1},
    {"Plus",           KeyCode::Plus},
    {"Comma",          KeyCode::Comma},
    {"Minus",          KeyCode::Minus},
    {"Period",         KeyCode::Period},
    {"OEM2",           KeyCode::OEM2},
    {"OEM3",           KeyCode::OEM3},
    {"Gamepad_A",      KeyCode::Gamepad_A},
    {"Gamepad_B",      KeyCode::Gamepad_B},
    {"Gamepad_X",      KeyCode::Gamepad_X},
    {"Gamepad_Y",      KeyCode::Gamepad_Y},
    {"GamepadRightBumper", KeyCode::GamepadRightBumper},
    {"GamepadLeftBumper",  KeyCode::GamepadLeftBumper},
    {"GamepadLeftTrigger", KeyCode::GamepadLeftTrigger},
    {"GamepadRightTrigger",KeyCode::GamepadRightTrigger},
    {"GamepadDPadUp",      KeyCode::GamepadDPadUp},
    {"GamepadDPadDown",    KeyCode::GamepadDPadDown},
    {"GamepadDPadLeft",    KeyCode::GamepadDPadLeft},
    {"GamepadDPadRight",   KeyCode::GamepadDPadRight},
    {"GamepadMenu",        KeyCode::GamepadMenu},
    {"GamepadView",        KeyCode::GamepadView},
    {"GamepadLeftStickBtn",KeyCode::GamepadLeftStickBtn},
    {"GamepadRightStickBtn",KeyCode::GamepadRightStickBtn},
    {"GamepadLeftStickUp", KeyCode::GamepadLeftStickUp},
    {"GamepadLeftStickDown",KeyCode::GamepadLeftStickDown},
    {"GamepadLeftStickRight",KeyCode::GamepadLeftStickRight},
    {"GamepadLeftStickLeft",KeyCode::GamepadLeftStickLeft},
    {"GamepadRightStickUp",KeyCode::GamepadRightStickUp},
    {"GamepadRightStickDown",KeyCode::GamepadRightStickDown},
    {"GamepadRightStickRight",KeyCode::GamepadRightStickRight},
    {"GamepadRightStickLeft",KeyCode::GamepadRightStickLeft},
    {"OEM4",           KeyCode::OEM4},
    {"OEM5",           KeyCode::OEM5},
    {"OEM6",           KeyCode::OEM6},
    {"OEM7",           KeyCode::OEM7},
    {"OEM8",           KeyCode::OEM8},
    {"OEMAX",          KeyCode::OEMAX},
    {"OEM102",         KeyCode::OEM102},
    {"ICOHelp",        KeyCode::ICOHelp},
    {"ICO00",          KeyCode::ICO00},
    {"ProcessKey",     KeyCode::ProcessKey},
    {"OEMCLEAR",       KeyCode::OEMCLEAR},
    {"Packet",         KeyCode::Packet},
    {"OEMReset",       KeyCode::OEMReset},
    {"OEMJump",        KeyCode::OEMJump},
    {"OEMPA1",         KeyCode::OEMPA1},
    {"OEMPA2",         KeyCode::OEMPA2},
    {"OEMPA3",         KeyCode::OEMPA3},
    {"OEMWSCtrl",      KeyCode::OEMWSCtrl},
    {"OEMCusel",       KeyCode::OEMCusel},
    {"OEMAttn",        KeyCode::OEMAttn},
    {"OEMFinish",      KeyCode::OEMFinish},
    {"OEMCopy",        KeyCode::OEMCopy},
    {"OEMAuto",        KeyCode::OEMAuto},
    {"OEMEnlw",        KeyCode::OEMEnlw},
    {"OEMBackTab",     KeyCode::OEMBackTab},
    {"Attn",           KeyCode::Attn},
    {"CrSel",          KeyCode::CrSel},
    {"ExSel",          KeyCode::ExSel},
    {"EraseEOF",       KeyCode::EraseEOF},
    {"Play",           KeyCode::Play},
    {"Zoom",           KeyCode::Zoom},
    {"NoName",         KeyCode::NoName},
    {"PA1",            KeyCode::PA1},
    {"OEMClear",       KeyCode::OEMClear}
    });


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