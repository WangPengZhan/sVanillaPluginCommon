#include <chrono>
#include <future>
#include <utility>
#include <filesystem>
#include <list>

#include <QStandardPaths>
#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QString>

#include "Util/LocaleHelper.h"

#include "FFmpegHelper.h"
#include "FFmpegLog.h"

namespace ffmpeg
{

void removeMergeBeforeFile(const MergeInfo& mergeInfo)
{
    auto audio = util::utf8ToLocale(mergeInfo.audio);
    auto path = std::filesystem::path(audio);
    if (std::filesystem::exists(path))
    {
        std::filesystem::remove(path);
    }

    auto video = util::utf8ToLocale(mergeInfo.video);
    path = std::filesystem::path(video);
    if (std::filesystem::exists(path))
    {
        std::filesystem::remove(path);
    }
    auto subtitle = util::utf8ToLocale(mergeInfo.subtitle);
    path = std::filesystem::path(subtitle);
    if (std::filesystem::exists(path))
    {
        std::filesystem::remove(path);
    }
}

FFmpegHelper::FFmpegHelper() = default;

FFmpegHelper::~FFmpegHelper()
{
    closeFFmpeg();
}

bool FFmpegHelper::mergeVideo(const MergeInfo& mergeInfo)
{
    return FFmpegHelper::mergeVideo(
        mergeInfo, []() {},
        [mergeInfo]() {
            removeMergeBeforeFile(mergeInfo);
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
    return FFmpegHelper::globalInstance().startFFmpegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

void FFmpegHelper::mergeVideoDetach(const MergeInfo& mergeInfo)
{
    return FFmpegHelper::mergeVideoDetach(
        mergeInfo, [] {},
        [mergeInfo] {
            removeMergeBeforeFile(mergeInfo);
        });
}

void FFmpegHelper::mergeVideoDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFmpegDetach(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

FFmpegHelper& FFmpegHelper::globalInstance()
{
    static FFmpegHelper ffmpegHelper;
    return ffmpegHelper;
}

std::future<bool> FFmpegHelper::startFFmpegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return std::async(std::launch::async, [this, mergeInfo, errorFunc = std::move(errorFunc), finishedFunc = std::move(finishedFunc)]() -> bool {
        return startFFmpeg(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
    });
}

bool FFmpegHelper::startFFmpeg(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    QStringList ffmpegArg;
    QStringList mapArgs;
    int index = 0;
    int videoIndex = 0;

    if (!mergeInfo.video.empty())
    {
        ffmpegArg << "-i" << mergeInfo.video.c_str();
        videoIndex = index;
        mapArgs << "-map" << QString::number(index++) + ":v?";
    }

    if (!mergeInfo.audio.empty())
    {
        ffmpegArg << "-i" << mergeInfo.audio.c_str();
        mapArgs << "-map" << QString::number(index++) + ":a?";
    }
    else
    {
        mapArgs << "-map" << QString::number(videoIndex) + ":a?";
    }

    if (!mergeInfo.subtitle.empty())
    {
        ffmpegArg << "-i" << mergeInfo.subtitle.c_str();
        mapArgs << "-map" << QString::number(index++) + ":s?";
    }
    else
    {
        mapArgs << "-map" << QString::number(videoIndex) + ":s?";
    }

    ffmpegArg << mapArgs << "-c:v" << "copy" << "-c:a" << "copy" << "-c:s" << "mov_text" << "-y";
    ffmpegArg << mergeInfo.targetVideo.c_str();

    std::vector<std::string> ffmpegArgVec;
    for (const auto& arg : ffmpegArg)
    {
        ffmpegArgVec.push_back(arg.toStdString());
    }
    return startFFmpeg(ffmpegArgVec, std::move(errorFunc), std::move(finishedFunc));
}

void FFmpegHelper::startFFmpegDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    {
        std::lock_guard lk(m_mutex);
        m_futures.remove_if([](const std::future<bool>& future) {
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });
    }

    std::future<bool> result = startFFmpegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));

    {
        std::lock_guard lk(m_mutex);
        m_futures.push_back(std::move(result));
    }
}

bool FFmpegHelper::mergeVideo(const MergeTsInfo& mergeInfo)
{
    return FFmpegHelper::mergeVideo(mergeInfo, []() {}, []() {});
}

bool FFmpegHelper::mergeVideo(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFmpeg(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

std::future<bool> FFmpegHelper::mergeVideoAsync(const MergeTsInfo& mergeInfo)
{
    return std::async(std::launch::async, static_cast<bool (*)(const MergeTsInfo&)>(&FFmpegHelper::mergeVideo), mergeInfo);
}

std::future<bool> FFmpegHelper::mergeVideoAsync(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFmpegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

void FFmpegHelper::mergeVideoDetach(const MergeTsInfo& mergeInfo)
{
    return FFmpegHelper::mergeVideoDetach(mergeInfo, []() {}, []() {});
}

void FFmpegHelper::mergeVideoDetach(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return FFmpegHelper::globalInstance().startFFmpegDetach(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

std::future<bool> FFmpegHelper::startFFmpegAsync(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    return std::async(std::launch::async, [this, mergeInfo, errorFunc = std::move(errorFunc), finishedFunc = std::move(finishedFunc)]() -> bool {
        return startFFmpeg(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
    });
}

bool FFmpegHelper::startFFmpeg(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    QStringList ffmpegArg;
    ffmpegArg << "-f" << "concat" << "-safe" << "0" << "-i" << mergeInfo.concatFile.c_str() << "-c" << "copy" << mergeInfo.targetVideo.c_str() << "-y";
    std::vector<std::string> ffmpegArgVec;
    for (const auto& arg : ffmpegArg)
    {
        ffmpegArgVec.push_back(arg.toStdString());
    }
    return startFFmpeg(ffmpegArgVec, std::move(errorFunc), std::move(finishedFunc));
}

void FFmpegHelper::startFFmpegDetach(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    {
        std::lock_guard lk(m_mutex);
        m_futures.remove_if([](const std::future<bool>& future) {
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });
    }

    std::future<bool> result = startFFmpegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));

    {
        std::lock_guard lk(m_mutex);
        m_futures.push_back(std::move(result));
    }
}

bool FFmpegHelper::startFFmpeg(const std::vector<std::string>& ffmpegArgVec, std::function<void()> errorFunc, std::function<void()> finishedFunc,
                               const std::string& ffmpegWorkDir)
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
    if (!ffmpegWorkDir.empty())
    {
        QString workDir = QString::fromStdString(ffmpegWorkDir);
        if (std::filesystem::exists(workDir.toStdString()))
        {
            ffmpegProcess.setWorkingDirectory(workDir);
        }
        else
        {
            FFMPEG_LOG_ERROR("ffmpeg working directory does not exist: {}", workDir.toStdString());
            if (errorFunc)
            {
                errorFunc();
            }
            return false;
        }
    }
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
