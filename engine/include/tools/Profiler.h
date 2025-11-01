//
// Created by WhoLeb on 21-Sep-25.
//

#pragma once

#define BLAINN_ENABLE_PROFILING 0

#if BLAINN_ENABLE_PROFILING
#include <tracy/Tracy.hpp>
#endif

#if BLAINN_ENABLE_PROFILING
/*
 * This is used to mark that a frame has been finished
 */
#define BLAINN_PROFILE_MARK_FRAME           FrameMark


/*
 * These macros are used to include the profilers
 *
 * BLAINN_PROFILE_FUNC should ONLY be used in the beginning of the function
 *
 * BLAINN_PROFILE_SCOPE is for tracing scopes
 *
 * BLAINN_PROFILE_SCOPE_DYNAMIC to trace scopes and add a name
 * (Cherno uses only this vertion)
 *
 * BLAINN_PROFILE_THREAD assigns a name to a thread
 */
#define BLAINN_PROFILE_FUNC(...)            //ZoneScoped##__VA_OPT__(N(__VA_ARGS__))
#define BLAINN_PROFILE_SCOPE(...)           //BLAINN_PROFILE_FUNC(__VA_ARGS__)
#define BLAINN_PROFILE_SCOPE_DYNAMIC(NAME)  //ZoneScoped; ZoneName(NAME, strlen(NAME))
#define BLAINN_PROFILE_THREAD(...)          //tracy::SetThreadName(__VA_ARGS__);

#else

#define BLAINN_PROFILE_MARK_FRAME
#define BLAINN_PROFILE_FUNC(...)
#define BLAINN_PROFILE_SCOPE(...)
#define BLAINN_PROFILE_SCOPE_DYNAMIC(NAME)
#define BLAINN_PROFILE_THREAD(...)

#endif
