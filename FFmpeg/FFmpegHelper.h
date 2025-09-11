#pragma once
#include <string>
#include <future>
#include <functional>
#include <vector>
#include <list>
#include <mutex>

#include "FFmpegConfig.h"

namespace ffmpeg
{

struct FFMPEG_PORT MergeInfo
{
    std::string audio;
    std::string video;
    std::string subtitle;
    std::string targetVideo;
};

struct FFMPEG_PORT MergeTsInfo
{
    std::string concatFile;
    std::string targetVideo;
};

class FFMPEG_PORT FFmpegHelper
{
public:
    FFmpegHelper();
    ~FFmpegHelper();

    static bool mergeVideo(const MergeInfo& mergeInfo);
    static bool mergeVideo(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    static std::future<bool> mergeVideoAsync(const MergeInfo& mergeInfo);
    static std::future<bool> mergeVideoAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    static void mergeVideoDetach(const MergeInfo& mergeInfo);
    static void mergeVideoDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    static FFmpegHelper& globalInstance();

    std::future<bool> startFFmpegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    bool startFFmpeg(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    void startFFmpegDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    static bool mergeVideo(const MergeTsInfo& mergeInfo);
    static bool mergeVideo(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    static std::future<bool> mergeVideoAsync(const MergeTsInfo& mergeInfo);
    static std::future<bool> mergeVideoAsync(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    static void mergeVideoDetach(const MergeTsInfo& mergeInfo);
    static void mergeVideoDetach(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    std::future<bool> startFFmpegAsync(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    bool startFFmpeg(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    void startFFmpegDetach(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    bool startFFmpeg(const std::vector<std::string>& ffmpegArg, std::function<void()> errorFunc, std::function<void()> finishedFunc,
                     const std::string& ffmpegWorkDir = {});

    void closeFFmpeg();

private:
    std::list<std::future<bool>> m_futures;
    std::recursive_mutex m_mutex;
};

}  // namespace ffmpeg
