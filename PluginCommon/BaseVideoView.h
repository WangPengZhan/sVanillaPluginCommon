#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace adapter
{
enum FileType
{
    Unknow = -1,
    Video,
    Image,
    Audio,
    File,
};

struct BaseVideoView
{
    std::string Identifier;
    std::string AlternateId;
    std::string VideoId;
    std::string Title;
    std::string Publisher;
    std::string Cover;
    std::string Duration;
    std::string Description;
    std::string PublishDate;
    std::string PlayListTitle;
    FileType fileType{FileType::Video};
    int pluginType = {-1};
};

using VideoView = std::vector<BaseVideoView>;

using Views = std::vector<std::shared_ptr<BaseVideoView>>;
}  // namespace adapter

enum class VideQuality
{
    Best = -1,  // auto
    R240P,
    R360P,
    R480P,
    R720P,
    R720P60F,
    R1080P,
    R1080PL,
    R1080R60F,
    R2160P,
};

struct DownloadConfig
{
    std::string downloadDir;
    VideQuality videoQuality = VideQuality::Best;
    std::string nameRule = "$title$";
};

struct VideoInfoFull
{
    std::shared_ptr<DownloadConfig> downloadConfig;
    std::shared_ptr<adapter::BaseVideoView> videoView;

    std::string getGuid() const;
    std::unordered_map<std::string, std::string> nameRules() const;
    std::string fileName() const;
    std::string coverPath() const;

    static const std::vector<std::string> ruleList;
};

struct PluginMessage
{
    int pluginId{};
    std::string name;
    std::string version;
    std::string description;
    std::string domain;
};
