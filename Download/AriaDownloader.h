#pragma once

#include "Download/FileDownloader.h"
#include "AbstractDownloader.h"
#include "Aria2Net/AriaClient/AriaApi.h"

namespace download
{

class AriaDownloader : public FileDownloader
{
public:
    AriaDownloader();
    AriaDownloader(std::list<std::string> uris, std::string path = "", std::string filename = "");
    explicit AriaDownloader(std::list<std::string> uris, const aria2net::AriaSendOption& option);
    ~AriaDownloader() = default;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    void downloadStatus() override;
    void finish() override;

    const aria2net::AriaSendOption& sendOption() const;
    void setAriaSendOption(const aria2net::AriaSendOption& option);
    void setUris(const std::list<std::string>& uris);
    const std::list<std::string>& uris() const;
    const std::string& gid() const;
    const std::size_t& fileSize() const;
    const aria2net::AriaTellStatus& nowStatus();

    void setPath(std::string path) override;
    const std::string& path() const override;
    void setFilename(std::string filename) override;
    const std::string& filename() const override;

private:
    std::string m_gid;
    aria2net::AriaSendOption m_ariaSendOption;
    std::list<std::string> m_uris;
    std::size_t m_fileSize{};
    aria2net::AriaTellStatus m_downloadTellStatus;
};

}  // namespace download
