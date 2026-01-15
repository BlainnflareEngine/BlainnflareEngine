#include "pch.h"

#include "aliases.h"

#include "scripting/TypeRegistration.h"

using namespace Blainn;

#ifdef BLAINN_REGISTER_LUA_TYPES
void Blainn::RegisterUselessSimpleMathTypes(sol::state &luaState)
{
    // Register Plane
    sol::usertype<Plane> PlaneType =
        luaState.new_usertype<Plane>("Plane", sol::constructors<Plane()>(), sol::meta_function::equal_to,
                                     [](const Plane &a, const Plane &b) { return a == b; });

    // Bind Plane properties via the accessor methods
    PlaneType["Normal"] = [](const Plane &p) { return p.Normal(); };
    PlaneType["SetNormal"] = [](Plane &p, const Vec3 &v) { p.Normal(v); };
    PlaneType["D"] = [](const Plane &p) { return p.D(); };
    PlaneType["SetD"] = [](Plane &p, float d) { p.D(d); };
    PlaneType[sol::meta_function::call] = []() { return Plane(); };

    // Register Rect
    sol::usertype<Rect> RectType =
        luaState.new_usertype<Rect>("Rect", sol::constructors<Rect(), Rect(float, float, float, float)>(),
                                    sol::meta_function::equal_to, [](const Rect &a, const Rect &b) { return a == b; });

    // Bind Rect fields (SimpleMath Rectangle members are x,y,width,height)
    RectType["x"] = &Rect::x;
    RectType["y"] = &Rect::y;
    RectType["width"] = &Rect::width;
    RectType["height"] = &Rect::height;
    RectType[sol::meta_function::call] = [](float x, float y, float w, float h) { return Rect(x, y, w, h); };

    // Register Ray
    sol::usertype<Ray> RayType =
        luaState.new_usertype<Ray>("Ray", sol::constructors<Ray(), Ray(const Vec3 &, const Vec3 &)>(),
                                   sol::meta_function::equal_to, [](const Ray &a, const Ray &b) { return a == b; });

    // Ray fields: position and direction
    RayType["position"] = &Ray::position;
    RayType["direction"] = &Ray::direction;
    RayType[sol::meta_function::call] = [](const Vec3 &p, const Vec3 &d) { return Ray(p, d); };

    // Register Viewport
    sol::usertype<Viewport> ViewportType = luaState.new_usertype<Viewport>(
        "Viewport", sol::constructors<Viewport(), Viewport(int, int, int, int)>(), sol::meta_function::equal_to,
        [](const Viewport &a, const Viewport &b) { return a == b; });

    // Viewport fields: x,y,width,height,minDepth,maxDepth
    ViewportType["x"] = &Viewport::x;
    ViewportType["y"] = &Viewport::y;
    ViewportType["width"] = &Viewport::width;
    ViewportType["height"] = &Viewport::height;
    ViewportType["minDepth"] = &Viewport::minDepth;
    ViewportType["maxDepth"] = &Viewport::maxDepth;
    ViewportType[sol::meta_function::call] = [](int x, int y, int w, int h) { return Viewport(x, y, w, h); };

        // Register Color
    sol::usertype<Color> ColorType = luaState.new_usertype<Color>(
        "Color", sol::constructors<Color(), Color(float, float, float, float)>(), sol::meta_function::equal_to,
        [](const Color &a, const Color &b) { return a == b; });

    // Color accessors and helpers
    ColorType["R"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::R), static_cast<void (Color::*)(float)>(&Color::R));
    ColorType["G"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::G), static_cast<void (Color::*)(float)>(&Color::G));
    ColorType["B"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::B), static_cast<void (Color::*)(float)>(&Color::B));
    ColorType["A"] =
        sol::property(static_cast<float (Color::*)() const>(&Color::A), static_cast<void (Color::*)(float)>(&Color::A));
    ColorType["ToVector3"] = static_cast<Vec3 (Color::*)() const>(&Color::ToVector3);
    ColorType["ToVector4"] = static_cast<Vec4 (Color::*)() const>(&Color::ToVector4);
    ColorType[sol::meta_function::call] = [](float r, float g, float b, float a) { return Color(r, g, b, a); };
}
#endif