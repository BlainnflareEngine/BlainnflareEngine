//
// Created by gorev on 30.09.2025.
//

#pragma once
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

    std::filesystem::path GetPath() const;

protected:
    std::filesystem::path m_path;
};
} // namespace Blainn
