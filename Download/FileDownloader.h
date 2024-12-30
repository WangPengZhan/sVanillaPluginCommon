#pragma once
#include "AbstractDownloader.h"

namespace download
{

class FileDownloader : public AbstractDownloader
{
public:
    FileDownloader() = default;
    virtual ~FileDownloader() = default;

    virtual void setPath(std::string path) = 0;
    virtual const std::string& path() const = 0;
    virtual void setFilename(std::string filename) = 0;
    virtual const std::string& filename() const = 0;
};

}  // namespace download
