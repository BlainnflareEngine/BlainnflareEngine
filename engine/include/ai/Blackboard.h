#pragma once
#include "EASTL/string.h"
#include "EASTL/unordered_map.h"
#include "EASTL/shared_ptr.h"
#include <variant>

#ifdef BLAINN_REGISTER_LUA_TYPES
#include <sol/sol.hpp>
#endif

namespace Blainn
{

using BlackboardValue = std::variant<int, float, double, bool, eastl::string, std::string, Vec2, Vec3>;

class Blackboard
{
public:
    struct BaseValue
    {
        virtual ~BaseValue() = default;
    };
    bool btAbortRequested = false;

    template <typename T> struct Value : BaseValue
    {
        T data;
        Value(const T &d) : data(d) {}
    };

    template <typename T> void Set(const eastl::string &key, const T &value)
    {
        values[key] = eastl::make_shared<Value<T>>(value);
    }

#ifdef BLAINN_REGISTER_LUA_TYPES
    void Set(const eastl::string &key, const sol::object &value)
    {
        if (!value.valid())
        {
            BF_WARN("Trying to set invalid sol::object to blackboard key: " + key);
            return;
        }

        if (value.is<bool>())
        {
            values[key] = eastl::make_shared<Value<bool>>(value.as<bool>());
        }
        else if (value.is<int>())
        {
            values[key] = eastl::make_shared<Value<int>>(value.as<int>());
        }
        else if (value.is<float>())
        {
            values[key] = eastl::make_shared<Value<float>>(value.as<float>());
        }
        else if (value.is<double>())
        {
            values[key] = eastl::make_shared<Value<double>>(value.as<double>());
        }
        else if (value.is<std::string>())
        {
            values[key] = eastl::make_shared<Value<eastl::string>>(eastl::string(value.as<std::string>().c_str()));
        }
        else if (value.is<const char *>())
        {
            values[key] = eastl::make_shared<Value<eastl::string>>(eastl::string(value.as<const char *>()));
        }
        //else if (value.is<eastl::string>())
        //{
        //    values[key] = eastl::make_shared<Value<eastl::string>>(value.as<eastl::string>());
        //}
        else if (value.is<Vec2>())
        {
            values[key] = eastl::make_shared<Value<Vec2>>(value.as<Vec2>());
        }
        else if (value.is<Vec3>())
        {
            values[key] = eastl::make_shared<Value<Vec3>>(value.as<Vec3>());
        }
        else
        {
            BF_ERROR("Blackboard: Unsupported type for key '" + key
                     + "'. Supported types: bool, int, float, double, string");
        }
    }
#endif

    template <typename T> T Get(const eastl::string &key) const
    {
        auto it = values.find(key);
        if (it == values.end()) return T{};

        auto *valuePtr = static_cast<Value<T> *>(it->second.get());
        if (!valuePtr) return T{};

        return valuePtr->data;
    }

    template <typename T> bool TryGet(const eastl::string &key, T &outValue) const
    {
        auto it = values.find(key);
        if (it == values.end()) return false;

        auto *valuePtr = dynamic_cast<Value<T> *>(it->second.get());
        if (!valuePtr) return false;

        outValue = valuePtr->data;
        return true;
    }

    bool Has(const eastl::string &key) const
    {
        return values.count(key) > 0;
    }

    void Remove(const eastl::string &key)
    {
        values.erase(key);
    }

    void Clear()
    {
        values.clear();
        btAbortRequested = false;
    }

private:
    eastl::unordered_map<eastl::string, eastl::shared_ptr<BaseValue>> values;
};

} // namespace Blainn