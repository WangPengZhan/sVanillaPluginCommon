#include <chrono>
#include <future>
#include <utility>
#include <filesystem>
#include <list>

#include "process.hpp"

#include "Util/LocaleHelper.h"
#include "Util/TimerUtil.h"

#include "FFmpegHelper.h"
#include "FFmpegLog.h"

#ifdef _WIN32
#    include <windows.h>

inline std::wstring utf8ToLocaleW(const std::string& str)
{
    if (str.empty())
        return {};

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);

    std::wstring wstr(size_needed, 0);

    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &wstr[0], size_needed);

    return wstr;
}

inline std::string localeWToUtf8(const std::wstring& wstr)
{
    if (wstr.empty())
        return {};

    int size_needed = WideCharToMultiByte(CP_UTF8,  // 转成 UTF-8
                                          0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);

    std::string str(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);

    return str;
}

#endif

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
    std::vector<std::string> ffmpegArgVec;
    std::vector<std::string> mapArgs;
    int index = 0;
    int videoIndex = 0;

    if (!mergeInfo.video.empty())
    {
        ffmpegArgVec.push_back("-i");
        ffmpegArgVec.push_back(mergeInfo.video);
        videoIndex = index;
        mapArgs.push_back("-map");
        mapArgs.push_back(std::to_string(index++) + ":v?");
    }

    if (!mergeInfo.audio.empty())
    {
        ffmpegArgVec.push_back("-i");
        ffmpegArgVec.push_back(mergeInfo.audio);
        mapArgs.push_back("-map");
        mapArgs.push_back(std::to_string(index++) + ":a?");
    }
    else
    {
        mapArgs.push_back("-map");
        mapArgs.push_back(std::to_string(videoIndex) + ":a?");
    }

    if (!mergeInfo.subtitle.empty())
    {
        ffmpegArgVec.push_back("-i");
        ffmpegArgVec.push_back(mergeInfo.subtitle);
        mapArgs.push_back("-map");
        mapArgs.push_back(std::to_string(index++) + ":s?");
    }
    else
    {
        mapArgs.push_back("-map");
        mapArgs.push_back(std::to_string(videoIndex) + ":s?");
    }

    ffmpegArgVec.insert(ffmpegArgVec.end(), mapArgs.begin(), mapArgs.end());

    ffmpegArgVec.push_back("-c:v");
    ffmpegArgVec.push_back("copy");
    ffmpegArgVec.push_back("-c:a");
    ffmpegArgVec.push_back("copy");
    ffmpegArgVec.push_back("-c:s");
    ffmpegArgVec.push_back("mov_text");
    ffmpegArgVec.push_back("-y");
    ffmpegArgVec.push_back(mergeInfo.targetVideo);

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
    std::vector<std::string> ffmpegArgVec = {"-f", "concat", "-safe", "0", "-i", mergeInfo.concatFile.c_str(), "-c", "copy", mergeInfo.targetVideo.c_str(),
                                             "-y"};
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
    std::string path = getModulePath();
    path = path + "/ffmpeg/ffmpeg";
#ifdef _WIN32
    path += ".exe";

    std::wstring command = L"\"" + utf8ToLocaleW(path) + L"\"";
    for (const auto& arg : ffmpegArgVec)
    {
        command += L" ";
        std::wstring argw = utf8ToLocaleW(arg);
        if (argw.find(L' ') != std::wstring::npos || argw.find(L'\"') != std::wstring::npos)
        {
            command += (L"\"" + argw + L"\"");
        }
        else
        {
            command += argw;
        }
    }

    std::wstring workDir = utf8ToLocaleW(ffmpegWorkDir);
#else

    std::string command = "\"" + path + "\"";
    for (const auto& arg : ffmpegArgVec)
    {
        command += " ";
        if (arg.find(' ') != std::string::npos || arg.find('\"') != std::string::npos)
        {
            command += ("\"" + arg + "\"");
        }
        else
        {
            command += arg;
        }
    }

    std::string workDir = ffmpegWorkDir;
#endif

    if (!workDir.empty() && !std::filesystem::exists(workDir))
    {
        FFMPEG_LOG_ERROR("ffmpeg working directory does not exist: {}", ffmpegWorkDir);
        if (errorFunc)
        {
            errorFunc();
        }
        return false;
    }

#ifdef _WIN32
    FFMPEG_LOG_INFO("starting ffmpeg process, param: {}", localeWToUtf8(command));
#else
    FFMPEG_LOG_INFO("starting ffmpeg process, param: {}", command);
#endif

    std::string stdOutStr;
    std::string stdErrStr;

    TinyProcessLib::Process ffmpegProcess(
        command, workDir,
        [&](const char* bytes, size_t n) {
            stdOutStr.append(bytes, n);
        },
        [&](const char* bytes, size_t n) {
            stdErrStr.append(bytes, n);
        });

    int exitCode = ffmpegProcess.get_exit_status();

    if (!stdOutStr.empty())
    {
        FFMPEG_LOG_INFO("ffmpeg process: {}", stdOutStr);
    }

    if (!stdErrStr.empty())
    {
        FFMPEG_LOG_ERROR("ffmpeg process: {}", stdErrStr);
    }

    if (exitCode == 0)
    {
        if (finishedFunc)
            finishedFunc();
        FFMPEG_LOG_INFO("starting ffmpeg process end");
        return true;
    }
    else
    {
        FFMPEG_LOG_ERROR("starting ffmpeg process error, errorCode: {}", exitCode);
        if (errorFunc)
            errorFunc();
        return false;
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
