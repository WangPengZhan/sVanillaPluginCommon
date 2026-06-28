#include "AbstractDownloader.h"

namespace download
{
namespace
{
constexpr char statusWaiting[] = "Waiting";
constexpr char statusReady[] = "Ready";
constexpr char statusDownloading[] = "Downloading";
constexpr char statusPause[] = "Pause";
constexpr char statusPaused[] = "Paused";
constexpr char statusResumed[] = "Resumed";
constexpr char statusStopped[] = "Stopped";
constexpr char statusFinished[] = "Finished";
constexpr char statusError[] = "Error";
}  // namespace

const DownloadInfo& AbstractDownloader::info() const
{
    return m_info;
}

const std::string& AbstractDownloader::stage() const
{
    return m_stage;
}

std::string AbstractDownloader::guid() const
{
    return m_guid;
}

void AbstractDownloader::setStatus(Status status)
{
    m_status.store(status);
}

AbstractDownloader::Status AbstractDownloader::status() const
{
    return m_status.load();
}

std::string statusToString(AbstractDownloader::Status status)
{
    switch (status)
    {
    case AbstractDownloader::Waiting:
        return statusWaiting;
    case AbstractDownloader::Ready:
        return statusReady;
    case AbstractDownloader::Downloading:
        return statusDownloading;
    case AbstractDownloader::Pause:
        return statusPause;
    case AbstractDownloader::Paused:
        return statusPaused;
    case AbstractDownloader::Resumed:
        return statusResumed;
    case AbstractDownloader::Stopped:
        return statusStopped;
    case AbstractDownloader::Finished:
        return statusFinished;
    case AbstractDownloader::Error:
        return statusError;
    default:
        break;
    }
    return {};
}

AbstractDownloader::Status statusFromString(const std::string& status)
{
    if (status == statusWaiting)
    {
        return AbstractDownloader::Waiting;
    }
    if (status == statusReady)
    {
        return AbstractDownloader::Ready;
    }
    if (status == statusDownloading)
    {
        return AbstractDownloader::Downloading;
    }
    if (status == statusPause)
    {
        return AbstractDownloader::Pause;
    }
    if (status == statusPaused)
    {
        return AbstractDownloader::Paused;
    }
    if (status == statusResumed)
    {
        return AbstractDownloader::Resumed;
    }
    if (status == statusStopped)
    {
        return AbstractDownloader::Stopped;
    }
    if (status == statusFinished)
    {
        return AbstractDownloader::Finished;
    }
    return AbstractDownloader::Error;
}

}  // namespace download
