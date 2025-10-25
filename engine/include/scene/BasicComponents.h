#pragma once
#include "EASTL/string.h"
#include "EASTL/vector.h"
#include "aliases.h"

namespace Blainn
{
struct IDComponent
{
    uuid ID{};
};

struct TagComponent
{
    eastl::string Tag;

    TagComponent() = default;
    TagComponent(TagComponent &) = default;
    TagComponent(const eastl::string &tag)
        : Tag(tag)
    {
    }

    operator eastl::string &()
    {
        return Tag;
    }
    operator const eastl::string &() const
    {
        return Tag;
    }
};

struct RelationshipComponent
{
    uuid ParentHandle{};
    eastl::vector<uuid> Children;

    RelationshipComponent() = default;
    RelationshipComponent(RelationshipComponent &) = default;

    RelationshipComponent(uuid parent)
        : ParentHandle(parent)
    {
    }
};
} // namespace Blainn