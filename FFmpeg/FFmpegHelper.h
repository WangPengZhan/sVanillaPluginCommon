#pragma once
#include <string>
#include <future>
#include <functional>

#include "FFmpegConfig.h"

namespace ffmpeg
{

struct FFMPEG_PORT MergeInfo
{
    std::string audio;
    std::string video;
    std::string targetVideo;
};

struct FFMPEG_PORT MergeTsInfo
{
    std::vector<std::string> tsFiles;
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

    std::future<bool> startFFpmegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    bool startFFmpeg(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    void startFFpmegDetach(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    std::future<bool> startFFpmegAsync(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    bool startFFmpeg(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    void startFFpmegDetach(const MergeTsInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    bool startFFmpeg(const QStringList& ffmpegArg, std::function<void()> errorFunc, std::function<void()> finishedFunc);

    void closeFFmpeg();

private:
    std::list<std::future<bool>> m_futures;
    std::recursive_mutex m_mutex;
};

}  // namespace ffmpeg
