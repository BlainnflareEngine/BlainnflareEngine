//
// Created by WhoLeb on 02-Nov-25.
// Copyright (c) 2025 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>

namespace Blainn
{
// Key codes for windows keycodes.
typedef enum class KeyCode : uint16_t
{
    CtrlBrkPrcs = 0x03, //Control-break processing

    //0x07 : reserved

    BackSpace = 0x08, //Backspace key
    Tab = 0x09, //Tab key

    //0x0A - 0x0B : reserved

    Clear = 0x0C, //Clear key
    Enter = 0x0D, //Enter or Return key

    //0x0E - 0x0F : unassigned

    Shift = 0x10, //Shift key
    Control = 0x11, //Ctrl key
    Alt = 0x12, //Alt key
    Pause = 0x13, //Pause key
    CapsLock = 0x14, //Caps lock key

    Kana = 0x15, //Kana input mode
    Hangeul = 0x15, //Hangeul input mode
    Hangul = 0x15, //Hangul input mode

    //0x16 : unassigned

    Junju = 0x17, //Junja input method
    Final = 0x18, //Final input method
    Hanja = 0x19, //Hanja input method
    Kanji = 0x19, //Kanji input method

    //0x1A : unassigned

    Escape = 0x1B, //Esc key

    Convert = 0x1C, //IME convert
    NonConvert = 0x1D, //IME Non convert
    Accept = 0x1E, //IME accept
    ModeChange = 0x1F, //IME mode change

    Space = 0x20, //Space bar
    PageUp = 0x21, //Page up key
    PageDown = 0x22, //Page down key
    End = 0x23, //End key
    Home = 0x24, //Home key
    LeftArrow = 0x25, //Left arrow key
    UpArrow = 0x26, //Up arrow key
    RightArrow = 0x27, //Right arrow key
    DownArrow = 0x28, //Down arrow key
    Select = 0x29, //Select key
    Print = 0x2A, //Print key
    Execute = 0x2B, //Execute key
    PrintScreen = 0x2C, //Print screen key
    Inser = 0x2D, //Insert key
    Delete = 0x2E, //Delete key
    Help = 0x2F, //Help key

    Num0 = 0x30, //Top row 0 key (Matches '0')
    Num1 = 0x31, //Top row 1 key (Matches '1')
    Num2 = 0x32, //Top row 2 key (Matches '2')
    Num3 = 0x33, //Top row 3 key (Matches '3')
    Num4 = 0x34, //Top row 4 key (Matches '4')
    Num5 = 0x35, //Top row 5 key (Matches '5')
    Num6 = 0x36, //Top row 6 key (Matches '6')
    Num7 = 0x37, //Top row 7 key (Matches '7')
    Num8 = 0x38, //Top row 8 key (Matches '8')
    Num9 = 0x39, //Top row 9 key (Matches '9')

    //0x3A - 0x40 : unassigned

    A = 0x41, //A key (Matches 'A')
    B = 0x42, //B key (Matches 'B')
    C = 0x43, //C key (Matches 'C')
    D = 0x44, //D key (Matches 'D')
    E = 0x45, //E key (Matches 'E')
    F = 0x46, //F key (Matches 'F')
    G = 0x47, //G key (Matches 'G')
    H = 0x48, //H key (Matches 'H')
    I = 0x49, //I key (Matches 'I')
    J = 0x4A, //J key (Matches 'J')
    K = 0x4B, //K key (Matches 'K')
    L = 0x4C, //L key (Matches 'L')
    M = 0x4D, //M key (Matches 'M')
    N = 0x4E, //N key (Matches 'N')
    O = 0x4F, //O key (Matches 'O')
    P = 0x50, //P key (Matches 'P')
    Q = 0x51, //Q key (Matches 'Q')
    R = 0x52, //R key (Matches 'R')
    S = 0x53, //S key (Matches 'S')
    T = 0x54, //T key (Matches 'T')
    U = 0x55, //U key (Matches 'U')
    V = 0x56, //V key (Matches 'V')
    W = 0x57, //W key (Matches 'W')
    X = 0x58, //X key (Matches 'X')
    Y = 0x59, //Y key (Matches 'Y')
    Z = 0x5A, //Z key (Matches 'Z')

    LeftWin = 0x5B, //Left windows key
    RightWin = 0x5C, //Right windows key
    Apps = 0x5D, //Applications key

    //0x5E : reserved

    Sleep = 0x5F, //Computer sleep key

    Numpad0 = 0x60, //Numpad 0
    Numpad1 = 0x61, //Numpad 1
    Numpad2 = 0x62, //Numpad 2
    Numpad3 = 0x63, //Numpad 3
    Numpad4 = 0x64, //Numpad 4
    Numpad5 = 0x65, //Numpad 5
    Numpad6 = 0x66, //Numpad 6
    Numpad7 = 0x67, //Numpad 7
    Numpad8 = 0x68, //Numpad 8
    Numpad9 = 0x69, //Numpad 9
    Multiply = 0x6A, //Multiply key
    Add = 0x6B, //Add key
    Separator = 0x6C, //Separator key
    Subtract = 0x6D, //Subtract key
    Decimal = 0x6E, //Decimal key
    Divide = 0x6F, //Divide key
    F1 = 0x70, //F1
    F2 = 0x71, //F2
    F3 = 0x72, //F3
    F4 = 0x73, //F4
    F5 = 0x74, //F5
    F6 = 0x75, //F6
    F7 = 0x76, //F7
    F8 = 0x77, //F8
    F9 = 0x78, //F9
    F10 = 0x79, //F10
    F11 = 0x7A, //F11
    F12 = 0x7B, //F12
    F13 = 0x7C, //F13
    F14 = 0x7D, //F14
    F15 = 0x7E, //F15
    F16 = 0x7F, //F16
    F17 = 0x80, //F17
    F18 = 0x81, //F18
    F19 = 0x82, //F19
    F20 = 0x83, //F20
    F21 = 0x84, //F21
    F22 = 0x85, //F22
    F23 = 0x86, //F23
    F24 = 0x87, //F24

    //0x88 - 0x8F : UI navigation

    NavigationView = 0x88, //reserved
    NavigationMenu = 0x89, //reserved
    NavigationUp = 0x8A, //reserved
    NavigationDown = 0x8B, //reserved
    NavigationLeft = 0x8C, //reserved
    NavigationRight = 0x8D, //reserved
    NavigationAccept = 0x8E, //reserved
    NavigationCancel = 0x8F, //reserved

    NumLock = 0x90, //Num lock key
    ScrollLock = 0x91, //Scroll lock key

    NumpadEqual = 0x92, //Numpad =

    FJ_Jisho = 0x92, //Dictionary key
    FJ_Masshou = 0x93, //Unregister word key
    FJ_Touroku = 0x94, //Register word key
    FJ_Loya = 0x95, //Left OYAYUBI key
    FJ_Roya = 0x96, //Right OYAYUBI key

    //0x97 - 0x9F : unassigned

    LeftShift = 0xA0, //Left shift key
    RightShift = 0xA1, //Right shift key
    LeftCtrl = 0xA2, //Left control key
    RightCtrl = 0xA3, //Right control key
    LeftMenu = 0xA4, //Left menu key
    RightMenu = 0xA5, //Right menu

    BrowserBack = 0xA6, //Browser back button
    BrowserForward = 0xA7, //Browser forward button
    BrowserRefresh = 0xA8, //Browser refresh button
    BrowserStop = 0xA9, //Browser stop button
    BrowserSearch = 0xAA, //Browser search button
    BrowserFavorites = 0xAB, //Browser favorites button
    BrowserHome = 0xAC, //Browser home button

    VolumeMute = 0xAD, //Volume mute button
    VolumeDown = 0xAE, //Volume down button
    VolumeUp = 0xAF, //Volume up button
    NextTrack = 0xB0, //Next track media button
    PrevTrack = 0xB1, //Previous track media button
    Stop = 0xB2, //Stop media button
    PlayPause = 0xB3, //Play/pause media button
    Mail = 0xB4, //Launch mail button
    MediaSelect = 0xB5, //Launch media select button
    App1 = 0xB6, //Launch app 1 button
    App2 = 0xB7, //Launch app 2 button

    //0xB8 - 0xB9 : reserved

    OEM1 = 0xBA, //;: key for US or misc keys for others
    Plus = 0xBB, //Plus key
    Comma = 0xBC, //Comma key
    Minus = 0xBD, //Minus key
    Period = 0xBE, //Period key
    OEM2 = 0xBF, //? for US or misc keys for others
    OEM3 = 0xC0, //~ for US or misc keys for others

    //0xC1 - 0xC2 : reserved

    Gamepad_A = 0xC3, //Gamepad A button
    Gamepad_B = 0xC4, //Gamepad B button
    Gamepad_X = 0xC5, //Gamepad X button
    Gamepad_Y = 0xC6, //Gamepad Y button
    GamepadRightBumper = 0xC7, //Gamepad right bumper
    GamepadLeftBumper = 0xC8, //Gamepad left bumper
    GamepadLeftTrigger = 0xC9, //Gamepad left trigger
    GamepadRightTrigger = 0xCA, //Gamepad right trigger
    GamepadDPadUp = 0xCB, //Gamepad DPad up
    GamepadDPadDown = 0xCC, //Gamepad DPad down
    GamepadDPadLeft = 0xCD, //Gamepad DPad left
    GamepadDPadRight = 0xCE, //Gamepad DPad right
    GamepadMenu = 0xCF, //Gamepad menu button
    GamepadView = 0xD0, //Gamepad view button
    GamepadLeftStickBtn = 0xD1, //Gamepad left stick button
    GamepadRightStickBtn = 0xD2, //Gamepad right stick button
    GamepadLeftStickUp = 0xD3, //Gamepad left stick up
    GamepadLeftStickDown = 0xD4, //Gamepad left stick down
    GamepadLeftStickRight = 0xD5, //Gamepad left stick right
    GamepadLeftStickLeft = 0xD6, //Gamepad left stick left
    GamepadRightStickUp = 0xD7, //Gamepad right stick up
    GamepadRightStickDown = 0xD8, //Gamepad right stick down
    GamepadRightStickRight = 0xD9, //Gamepad right stick right
    GamepadRightStickLeft = 0xDA, //Gamepad right stick left

    OEM4 = 0xDB, //[ for US or misc keys for others
    OEM5 = 0xDC, //\ for US or misc keys for others
    OEM6 = 0xDD, //] for US or misc keys for others
    OEM7 = 0xDE, //' for US or misc keys for others
    OEM8 = 0xDF, //Misc keys for others

    //0xE0 : reserved

    OEMAX = 0xE1, //AX key on Japanese AX keyboard
    OEM102 = 0xE2, //"<>" or "\|" on RT 102-key keyboard
    ICOHelp = 0xE3, //Help key on ICO
    ICO00 = 0xE4, //00 key on ICO

    ProcessKey = 0xE5, //Process key input method
    OEMCLEAR = 0xE6, //OEM specific
    Packet = 0xE7, //IDK man try to google it

    //0xE8 : unassigned

    OEMReset = 0xE9, //OEM reset button
    OEMJump = 0xEA, //OEM jump button
    OEMPA1 = 0xEB, //OEM PA1 button
    OEMPA2 = 0xEC, //OEM PA2 button
    OEMPA3 = 0xED, //OEM PA3 button
    OEMWSCtrl = 0xEE, //OEM WS Control button
    OEMCusel = 0xEF, //OEM CUSEL button
    OEMAttn = 0xF0, //OEM ATTN button
    OEMFinish = 0xF1, //OEM finish button
    OEMCopy = 0xF2, //OEM copy button
    OEMAuto = 0xF3, //OEM auto button
    OEMEnlw = 0xF4, //OEM ENLW
    OEMBackTab = 0xF5, //OEM back tab

    Attn = 0xF6, //Attn
    CrSel = 0xF7, //CrSel
    ExSel = 0xF8, //ExSel
    EraseEOF = 0xF9, //Erase EOF key
    Play = 0xFA, //Play key
    Zoom = 0xFB, //Zoom key
    NoName = 0xFC, //No name
    PA1 = 0xFD, //PA1 key
    OEMClear = 0xFE, //OEM Clear key
} Key;

enum class KeyState : int8_t
{
    None = -1,
    Pressed = 0,
    Held = 1,
    Released = 2
};

enum class CursorMode
{
    Normal = 0,
    Hidden = 1,
    Locked = 2
};

typedef enum class MouseButton : uint16_t
{
    LeftMouseBtn = 0, //Left mouse button
    RightMouseBtn = 1, //Right mouse button
    MidMouseBtn = 2, //Middle mouse button

    ThumbForward = 3, //Thumb button back on mouse aka X1
    ThumbBack = 4, //Thumb button forward on mouse aka X2

    Left = LeftMouseBtn,
    Right = RightMouseBtn,
    Middle = MidMouseBtn,
    X1 = ThumbForward,
    X2 = ThumbBack
} Button;

enum class ButtonState : int8_t
{
    None					=-1,
    Pressed					= 0,
    Held					= 1,
    Released				= 2
};

	/*
	inline std::ostream& operator<<(std::ostream& os, KeyCode key)
	{
		os << static_cast<int32_t>(key);
		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, MouseButton button)
	{
		os << static_cast<int32_t>(button);
		return os;
	}
	*/

}