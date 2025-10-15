#pragma once
#include "aliases.h"
#include "EASTL/string.h"
#include "EASTL/vector.h"

namespace Blainn
{
struct IDComponent
{
    uuid ID {0};
};

struct TagComponent
{
    eastl::string Tag;

    TagComponent() = default;
    TagComponent(TagComponent&) = default;
    TagComponent(const eastl::string& tag)
        : Tag(tag) {}

    operator eastl::string&() { return Tag; }
    operator const eastl::string&() const { return Tag; }
};

struct RelationshipComponent
{
    uuid ParentHandle{0};
    eastl::vector<uuid> Children;

    RelationshipComponent() = default;
    RelationshipComponent(RelationshipComponent&) = default;

    RelationshipComponent(uuid parent)
        : ParentHandle(parent) {}
};
}