//
// Created by gorev on 30.09.2025.
//

#include "file-system/FileSystemObject.h"

namespace Blainn
{


FileSystemObject::FileSystemObject(const Path &path)
    : m_path(path)
{
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
