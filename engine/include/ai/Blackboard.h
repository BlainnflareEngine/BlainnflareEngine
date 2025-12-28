#pragma once
#include <string>
#include <unordered_map>
#include <memory>

namespace Blainn
{
class Blackboard 
{
public:
    struct BaseValue { virtual ~BaseValue() = default; };
    bool btAbortRequested = false;

    template<typename T>
    struct Value : BaseValue
    {
        T data;
        Value(const T& d) : data(d) {}
    };

    template<typename T>
    void Set(const std::string& key, const T& value)
    {
        values[key] = std::make_shared<Value<T>>(value);
    }

    template<typename T>
    T Get(const std::string& key) const
    {
        auto it = values.find(key);
        if (it == values.end())
            return T{};
        return static_cast<Value<T>*>(it->second.get())->data;
    }

    bool Has(const std::string& key) const 
    {
        return values.count(key) > 0;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<BaseValue>> values;

};
} // namespace Blainn