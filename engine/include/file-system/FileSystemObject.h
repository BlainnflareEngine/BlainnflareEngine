//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "aliases.h"

namespace Blainn
{
class FileSystemObject
{
public:
    FileSystemObject() = default;
    FileSystemObject(const Path &relativePath);
    virtual ~FileSystemObject();

    virtual void Move();
    virtual void Delete();
    virtual void Copy();

    Path GetPath() const;

protected:
    Path m_path;
};
} // namespace Blainn
