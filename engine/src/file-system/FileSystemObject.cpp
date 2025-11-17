//
// Created by gorev on 30.09.2025.
//

#include "file-system/FileSystemObject.h"

#include "Engine.h"

namespace Blainn
{


FileSystemObject::FileSystemObject(const Path &absolutPath)
    : m_path(std::filesystem::relative(absolutPath, Engine::GetContentDirectory()))
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
