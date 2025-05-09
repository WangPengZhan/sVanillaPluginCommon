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
constexpr char ffmpegCommand[] = R"(-i "%1" -i "%2" -acodec copy -vcodec copy -f mp4 "%3")";

FFmpegHelper::FFmpegHelper() = default;

FFmpegHelper::~FFmpegHelper()
{
    closeFFmpeg();
}

void FFmpegHelper::mergeVideo(const MergeInfo& mergeInfo)
{
    FFmpegHelper::mergeVideo(
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

void FFmpegHelper::mergeVideo(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    FFmpegHelper::globalInstance().startFFpmegAsync(mergeInfo, std::move(errorFunc), std::move(finishedFunc));
}

FFmpegHelper& FFmpegHelper::globalInstance()
{
    static FFmpegHelper ffmpegHelper;
    return ffmpegHelper;
}

void FFmpegHelper::startFFpmegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc)
{
    {
        std::lock_guard lk(m_mutex);
        m_futures.remove_if([](const std::future<bool>& future) {
            return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        });
    }

    std::future<bool> result = std::async(std::launch::async, [mergeInfo, errorFunc = std::move(errorFunc), finishedFunc = std::move(finishedFunc)]() -> bool {
        QString executablePath = QApplication::applicationDirPath() + "/ffmpeg";
        QString ffmpegExecutable = QStandardPaths::findExecutable("ffmpeg", QStringList() << executablePath);
        QStringList ffmpegArg;
        ffmpegArg << "-i" << mergeInfo.audio.c_str() << "-i" << mergeInfo.video.c_str() << "-acodec" << "copy" << "-vcodec" << "copy" << "-f" << "mp4"
                  << mergeInfo.targetVideo.c_str() << "-y";
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
    });

    {
        std::lock_guard lk(m_mutex);
        m_futures.push_back(std::move(result));
    }
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
