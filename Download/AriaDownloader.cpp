#include "AriaDownloader.h"
#include "Aria2Net/AriaClient/AriaClient.h"
#include "DownloadLog.h"

#include <algorithm>

namespace download
{
AriaDownloader::AriaDownloader()
{
}

AriaDownloader::AriaDownloader(std::list<std::string> uris, std::string path, std::string filename)
    : m_uris(std::move(uris))
{
    m_ariaSendOption.dir = std::move(path);
    m_ariaSendOption.out = std::move(filename);
}

AriaDownloader::AriaDownloader(std::list<std::string> uris, const aria2net::AriaSendOption& option)
    : m_ariaSendOption(option)
    , m_uris(std::move(uris))
{
}

void AriaDownloader::start()
{
    aria2net::AriaAddUri result = aria2net::AriaClient::globalClient().AddUriAsync(m_uris, m_ariaSendOption);
    if (result.error.code != 0)
    {
        std::string res;
        std::for_each(m_uris.begin(), m_uris.end(), [&res](const std::string& url) {
            res += url;
        });
        DOWNLOAD_LOG_ERROR("AriaDownloader start error: {}, uri: {}", result.error.message, res);
        m_status = Error;
        return;
    }

    m_gid = result.result;
    m_status = Downloading;
}

void AriaDownloader::stop()
{
    const auto result = aria2net::AriaClient::globalClient().RemoveAsync(m_gid);
    if (result.error.code != 0)
    {
        DOWNLOAD_LOG_ERROR("AriaDownloader stop error: {}, gid: {}", result.error.message, m_gid);
    }
    m_status = Stopped;
    DOWNLOAD_LOG_INFO("AriaDownloader stop gid: {}", m_gid);
}

void AriaDownloader::pause()
{
    const auto result = aria2net::AriaClient::globalClient().PauseAsync(m_gid);
    if (result.error.code != 0)
    {
        DOWNLOAD_LOG_ERROR("AriaDownloader pause error: {}, gid: {}", result.error.message, m_gid);
    }
    m_status = Paused;
    DOWNLOAD_LOG_INFO("AriaDownloader pause gid: {}", m_gid);
}

void AriaDownloader::resume()
{
    const auto result = aria2net::AriaClient::globalClient().UnpauseAsync(m_gid);
    if (result.error.code != 0)
    {
        DOWNLOAD_LOG_ERROR("AriaDownloader resume error: {}, gid: {}", result.error.message, m_gid);
    }
    m_status = Downloading;
    DOWNLOAD_LOG_INFO("AriaDownloader resume gid: {}", m_gid);
}

void AriaDownloader::downloadStatus()
{
    m_downloadTellStatus = aria2net::AriaClient::globalClient().TellStatus(m_gid);
    if (m_downloadTellStatus.error.code != 0 || (!m_downloadTellStatus.result.errorCode.empty() && m_downloadTellStatus.result.errorCode != "0"))
    {
        std::string res;
        std::for_each(m_uris.begin(), m_uris.end(), [&res](const std::string& url) {
            res += url;
        });
        DOWNLOAD_LOG_ERROR("AriaDownloader start error: {}, uri: {}", m_downloadTellStatus.error.message, res);
        m_status = Error;
        return;
    }

    DownloadInfo info;
    try
    {
        info.complete = std::stoull(m_downloadTellStatus.result.completedLength);
        info.speed = std::stoull(m_downloadTellStatus.result.downloadSpeed);
        info.total = std::stoull(m_downloadTellStatus.result.totalLength);
    }
    catch (const std::exception& e)
    {
        DOWNLOAD_LOG_ERROR("AriaDownloader parse status error: {}, gid: {}", e.what(), m_gid);
        return;
    }

    setInfo(info);
    m_fileSize = info.total;

    if (info.complete == info.total && m_downloadTellStatus.result.status == "complete")
    {
        m_status = Finished;
    }
}

void AriaDownloader::finish()
{
}

const aria2net::AriaSendOption& AriaDownloader::sendOption() const
{
    return m_ariaSendOption;
}

void AriaDownloader::setAriaSendOption(const aria2net::AriaSendOption& option)
{
    m_ariaSendOption = option;
}

const std::string& AriaDownloader::path() const
{
    return m_ariaSendOption.dir;
}

void AriaDownloader::setPath(std::string path)
{
    m_ariaSendOption.dir = std::move(path);
}

const std::string& AriaDownloader::filename() const
{
    return m_ariaSendOption.out;
}

void AriaDownloader::setFilename(std::string filename)
{
    m_ariaSendOption.out = std::move(filename);
}

void AriaDownloader::setUris(const std::list<std::string>& uris)
{
    m_uris = uris;
}

const std::list<std::string>& AriaDownloader::uris() const
{
    return m_uris;
}

const std::string& AriaDownloader::gid() const
{
    return m_gid;
}

const std::size_t& AriaDownloader::fileSize() const
{
    return m_fileSize;
}

const aria2net::AriaTellStatus& AriaDownloader::nowStatus()
{
    return m_downloadTellStatus;
}

}  // namespace download
