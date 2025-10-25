//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "aliases.h"


#include <filesystem>

namespace Blainn
{
class FileSystemObject
{
public:
    FileSystemObject(const Path & path);
    virtual ~FileSystemObject();

    virtual void Move();
    virtual void Delete();
    virtual void Copy();

    Path GetPath() const;

protected:
    Path m_path;
};
} // namespace Blainn
