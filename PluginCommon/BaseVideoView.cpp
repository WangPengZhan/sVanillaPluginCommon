#include "BaseVideoView.h"

#include <iomanip>
#include <sstream>
#include <ctime>

#include "Util/UrlProccess.h"
#include <iomanip>

std::vector<int> findAllSubstrings(std::string_view str, const std::string& target)
{
    std::vector<int> positions;
    size_t pos = str.find(target);
    while (pos != std::string::npos)
    {
        positions.push_back(static_cast<int>(pos));
        pos = str.find(target, pos + target.length());
    }
    return positions;
}

std::map<int, std::string> parserRules(std::string_view text)
{
    std::map<int, std::string> results;

    for (const auto& name : VideoInfoFull::showRuleList)
    {
        auto positions = findAllSubstrings(text, name);
        for (const auto& pos : positions)
        {
            results[pos] = name;
        }
    }

    return results;
}

std::string showTag(const std::string& text)
{
    std::string result = text;
    result = "$" + result + "$";
    return result;
}

void DateTimeResolver::generator()
{
    std::time_t now = std::time(nullptr);
    std::tm local_tm;

#ifdef _WIN32
    localtime_s(&local_tm, &now);  // Windows
#else
    localtime_r(&now, &local_tm);  // Linux / POSIX
#endif

    std::stringstream os;
    os << std::put_time(&local_tm, "%Y-%m-%d_%H-%M-%S");
    dateTime = os.str();
    auto pos = dateTime.find("_");
    date = dateTime.substr(0, pos);
    time = dateTime.substr(pos + 1);
}

std::string VideoInfoFull::getGuid() const
{
    auto guid = videoView->Identifier + videoView->IdType + videoView->ParentId + downloadConfig->downloadDir +
                std::to_string(static_cast<int>(downloadConfig->videoQuality)) + fileName();
    guid = util::removeSpecialChars(guid);
    return guid;
}

std::string VideoInfoFull::parseNameRules(const std::string& ruleName) const
{
    if (!downloadConfig || !videoView)
    {
        return ruleName;
    }

    if (dateTimeResolver.date.empty())
    {
        dateTimeResolver.generator();
    }

    auto temp = ruleName;
    auto resultIndex = parserRules(temp);
    int offset = 0;
    for (const auto& [pos, name] : resultIndex)
    {
        if (name == showTag(adapter::id))
        {
            temp.replace(pos + offset, name.size(), videoView->Identifier);
            offset += videoView->Identifier.size() - name.size();
        }
        else if (name == showTag(adapter::title))
        {
            temp.replace(pos + offset, name.size(), videoView->Title);
            offset += videoView->Title.size() - name.size();
        }
        else if (name == showTag(adapter::publisher))
        {
            temp.replace(pos + offset, name.size(), videoView->Publisher);
            offset += videoView->Publisher.size() - name.size();
        }
        else if (name == showTag(adapter::publishdate))
        {
            temp.replace(pos + offset, name.size(), videoView->PublishDate);
            offset += videoView->PublishDate.size() - name.size();
        }
        else if (name == showTag(adapter::date))
        {
            temp.replace(pos + offset, name.size(), dateTimeResolver.date);
            offset += dateTimeResolver.date.size() - name.size();
        }
        else if (name == showTag(adapter::time))
        {
            temp.replace(pos + offset, name.size(), dateTimeResolver.time);
            offset += dateTimeResolver.time.size() - name.size();
        }
        else if (name == showTag(adapter::datetime))
        {
            temp.replace(pos + offset, name.size(), dateTimeResolver.dateTime);
            offset += dateTimeResolver.dateTime.size() - name.size();
        }
    }

    temp = util::removeSpecialChars(temp);
    temp += videoView->fileExtension;

    return temp;
}

std::string VideoInfoFull::fileName(bool update) const
{
    if (update)
    {
        dateTimeResolver.generator();
    }

    return parseNameRules(downloadConfig->nameRule);
}

std::string VideoInfoFull::coverPath() const
{
    return getGuid();
}

const std::vector<std::string> VideoInfoFull::ruleList = {adapter::id,   adapter::title, adapter::publisher, adapter::publishdate,
                                                          adapter::date, adapter::time,  adapter::datetime};
const std::vector<std::string> VideoInfoFull::showRuleList = [](std::vector<std::string> rules) {
    for (auto& rule : rules)
    {
        rule = showTag(rule);
    }
    return rules;
}(VideoInfoFull::ruleList);
const std::map<std::string, std::string> VideoInfoFull::ruleMap = []() {
    std::map<std::string, std::string> ruleMap;
    for (size_t i = 0; i < VideoInfoFull::ruleList.size(); ++i)
    {
        ruleMap[VideoInfoFull::ruleList[i]] = VideoInfoFull::showRuleList[i];
    }
    return ruleMap;
}();
