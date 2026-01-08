//
// Created by gorev on 30.09.2025.
//

#include "file-system/FileSystemObject.h"

#include "Engine.h"

namespace Blainn
{


FileSystemObject::FileSystemObject(const Path &relativePath)
    : m_path(relativePath)
{
    assert(m_path.is_relative());
}


FileSystemObject::~FileSystemObject()
{
}


void FileSystemObject::Move()
{
}


void FileSystemObject::Delete()
{
}


void FileSystemObject::Copy()
{
}


std::filesystem::path FileSystemObject::GetPath() const
{
    return m_path;
}

} // namespace Blainn
