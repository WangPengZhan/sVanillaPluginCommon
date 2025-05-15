#include <chrono>
#include <future>
#include <thread>
#include <utility>
#include <filesystem>

#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QString>

#include "FFmpegHelper.h"
#include "FFmpegLog.h"

namespace ffmpeg
{

FFmpegHelper::FFmpegHelper() = default;

FFmpegHelper::~FFmpegHelper()
{
    closeFFmpeg();
}

bool FFmpegHelper::mergeVideo(const MergeInfo& mergeInfo)
{
    return FFmpegHelper::mergeVideo(
        mergeInfo, [] {},
        [mergeInfo] {
            auto path = std::filesystem::u8path(mergeInfo.audio);
            if (std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }

            path = std::filesystem::u8path(mergeInfo.video);
            if (std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }
        });
}

bool FFmpegHelper::mergeVideo(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFmpeg(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

std::future<bool> FFmpegHelper::mergeVideoAsync(const MergeInfo& mergeInfo)
{
    return std::async(std::launch::async, static_cast<bool (*)(const MergeInfo&)>(&FFmpegHelper::mergeVideo), mergeInfo);
}

std::future<bool> FFmpegHelper::mergeVideoAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFpmegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

void FFmpegHelper::mergeVideoDetach(const MergeInfo& mergeInfo)
{
    return FFmpegHelper::mergeVideoDetach(
        mergeInfo, [] {},
        [mergeInfo] {
            auto path = std::filesystem::u8path(mergeInfo.audio);
            if (std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }

            path = std::filesystem::u8path(mergeInfo.video);
            if (std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }
        });
}

void FFmpegHelper::mergeVideoDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFpmegDetach(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

FFmpegHelper& FFmpegHelper::globalInstance()
{
    static FFmpegHelper ffmpegHelper;
    return ffmpegHelper;
}

std::future<bool> FFmpegHelper::startFFpmegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return std::async(std::launch::async, [this, mergeInfo, errorFunc = std::move(errorFunc), finishedFunc = std::move(finishedFunc)]() -> bool {
        return startFFmpeg(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
    });
}

bool FFmpegHelper::startFFmpeg(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    QStringList ffmpegArg;
    ffmpegArg << "-i" << mergeInfo.audio.c_str() << "-i" << mergeInfo.video.c_str() << "-acodec" << "copy" << "-vcodec" << "copy" << "-f" << "mp4"
              << mergeInfo.targetVideo.c_str() << "-y";
    std::vector<std::string> ffmpegArgVec;
    for (const auto& arg : ffmpegArg)
    {
        ffmpegArgVec.push_back(arg.toStdString());
    }
    return startFFmpeg(ffmpegArgVec, std::move(errorFunc), std::move(finishedFunc));
}

void FFmpegHelper::startFFpmegDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    {
        std::lock_guard lk(m_mutex);
        m_futures.remove_if([](const std::future<bool>& future) {
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });
    }

    std::future<bool> result = startFFpmegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));

    {
        std::lock_guard lk(m_mutex);
        m_futures.push_back(std::move(result));
    }
}

bool FFmpegHelper::startFFmpeg(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    QStringList tsFiles;
    for (const auto& file : mergeInfo.tsFiles)
    {
        tsFiles << file.c_str();
    }
    QStringList ffmpegArg;
    ffmpegArg << "-i" << QString("concat: %1").arg(tsFiles.join("|")) << "-c copy" << "-bsf:a aac_adtstoasc" << "-f" << "mp4" << mergeInfo.targetVideo.c_str()
              << "-y";
    std::vector<std::string> ffmpegArgVec;
    for (const auto& arg : ffmpegArg)
    {
        ffmpegArgVec.push_back(arg.toStdString());
    }
    return startFFmpeg(ffmpegArgVec, std::move(errorFunc), std::move(finishedFunc));
}

bool FFmpegHelper::startFFmpeg(const std::vector<std::string>& ffmpegArgVec, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    QStringList ffmpegArg;
    for (const auto& arg : ffmpegArgVec)
    {
        ffmpegArg << arg.c_str();
    }
    QString executablePath = QApplication::applicationDirPath() + "/ffmpeg";
    QString ffmpegExecutable = QStandardPaths::findExecutable("ffmpeg", QStringList() << executablePath);
    qDebug() << ffmpegArg;
    FFMPEG_LOG_INFO("starting ffmpeg process, param: {}", ffmpegArg.join(" ").toStdString());

    QProcess ffmpegProcess;
    ffmpegProcess.setProgram(ffmpegExecutable);
    ffmpegProcess.setArguments(ffmpegArg);
    ffmpegProcess.start();
    ffmpegProcess.waitForFinished(-1);

    auto data = ffmpegProcess.readAllStandardOutput();
    if (!data.isEmpty())
    {
        FFMPEG_LOG_INFO("ffmpeg process: {}", data.toStdString());
    }

    data = ffmpegProcess.readAllStandardError();
    if (!data.isEmpty())
    {
        FFMPEG_LOG_ERROR("ffmpeg process: {}", data.toStdString());
    }

    if (ffmpegProcess.exitStatus() == QProcess::NormalExit && ffmpegProcess.exitCode() == 0)
    {
        if (finishedFunc)
        {
            finishedFunc();
        }
        FFMPEG_LOG_INFO("starting ffmpeg process end");
        return true;
    }
    else
    {
        std::string errorString = ffmpegProcess.errorString().toStdString();
        int exitCode = ffmpegProcess.exitCode();
        FFMPEG_LOG_ERROR("starting ffmpeg process error, errorCode: {}, msg: {}", exitCode, errorString);
        if (errorFunc)
        {
            errorFunc();
        }
        return false;
    }

    return true;
}

void FFmpegHelper::closeFFmpeg()
{
    {
        std::lock_guard lk(m_mutex);
        for (auto& future : m_futures)
        {
            future.wait();
        }
        m_futures.clear();
    }
}

}  // namespace ffmpeg
