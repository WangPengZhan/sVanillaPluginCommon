#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>

namespace adapter
{
enum FileType
{
    Unknow = -1,
    Video,
    Image,
    Audio,
    PDF,
    Markdown,
    Text,
    File,
};

struct BaseVideoView
{
    std::string Identifier;
    std::string IdType;
    std::string ParentId;
    std::string Title;
    std::string Publisher;
    std::string Cover;
    std::string Duration;
    std::string Description;
    std::string PublishDate;
    std::string PlayListTitle;
    std::string fileExtension{".mp4"};
    std::string Option1;
    std::string Option2;
    std::string Option3;
    FileType fileType{FileType::Video};
    int pluginType = {-1};
};

using VideoView = std::vector<BaseVideoView>;

using Views = std::vector<std::shared_ptr<BaseVideoView>>;

constexpr char id[] = "id";
constexpr char title[] = "title";
constexpr char publisher[] = "publisher";
constexpr char publishdate[] = "publishdate";
constexpr char date[] = "date";
constexpr char time[] = "time";
constexpr char datetime[] = "datetime";
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

struct DateTimeResolver
{
    std::string date;
    std::string time;
    std::string dateTime;

    void generator();
};

struct VideoInfoFull
{
    std::shared_ptr<DownloadConfig> downloadConfig;
    std::shared_ptr<adapter::BaseVideoView> videoView;

    std::string getGuid() const;
    std::string fileName(bool update = false) const;
    std::string coverPath() const;
    std::string parseNameRules(const std::string& ruleName) const;

    static const std::vector<std::string> ruleList;
    static const std::vector<std::string> showRuleList;
    static const std::map<std::string, std::string> ruleMap;

    mutable DateTimeResolver dateTimeResolver;
};

struct PluginMessage
{
    int pluginId{};
    std::string name;
    std::string version;
    std::string description;
    std::string domain;
};
