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

class FFMPEG_PORT FFmpegHelper
{
public:
    FFmpegHelper();
    ~FFmpegHelper();

    static void mergeVideo(const MergeInfo& mergeInfo);
    static void mergeVideo(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    static FFmpegHelper& globalInstance();

    void startFFpmegAsync(const MergeInfo& mergeInfo, std::function<void()> errorFunc, std::function<void()> finishedFunc);
    void closeFFmpeg();

private:
    std::list<std::future<bool>> m_futures;
    std::recursive_mutex m_mutex;
};

}  // namespace ffmpeg
