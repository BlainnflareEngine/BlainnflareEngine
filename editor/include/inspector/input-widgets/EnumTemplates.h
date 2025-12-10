//
// Created by gorev on 07.12.2025.
//

#pragma once
#include "physics/Layers.h"
#include "physics/PhysicsTypes.h"

#include <QString>

namespace editor
{
template <typename T> std::vector<std::pair<T, QString>> enumToStrings();

template <>
inline std::vector<std::pair<Blainn::ComponentShapeType, QString>> enumToStrings<Blainn::ComponentShapeType>()
{
    return {
        {Blainn::ComponentShapeType::Sphere, "Sphere"},   {Blainn::ComponentShapeType::Box, "Box"},
        {Blainn::ComponentShapeType::Capsule, "Capsule"}, {Blainn::ComponentShapeType::Cylinder, "Cylinder"},
        {Blainn::ComponentShapeType::Empty, "Empty"},
    };
}


template <> inline std::vector<std::pair<Blainn::ObjectLayer, QString>> enumToStrings<Blainn::ObjectLayer>()
{
    return {
        {Blainn::Layers::UNUSED1, "Custom 1"},      {Blainn::Layers::UNUSED2, "Custom 2"},
        {Blainn::Layers::UNUSED3, "Custom 3"},      {Blainn::Layers::UNUSED4, "Custom 4"},
        {Blainn::Layers::NON_MOVING, "Non moving"}, {Blainn::Layers::MOVING, "Moving"},
        {Blainn::Layers::DEBRIS, "Debris"},         {Blainn::Layers::SENSOR, "Sensor"},
    };
}

template <>
inline std::vector<std::pair<Blainn::PhysicsComponentMotionType, QString>> enumToStrings<
    Blainn::PhysicsComponentMotionType>()
{
    return {
        {Blainn::PhysicsComponentMotionType::Dynamic, "Dynamic"},
        {Blainn::PhysicsComponentMotionType::Kinematic, "Kinematic"},
        {Blainn::PhysicsComponentMotionType::Static, "Static"},
    };
}
} // namespace editor
