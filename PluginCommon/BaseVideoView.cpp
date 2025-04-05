#include "BaseVideoView.h"
#include "Util/UrlProccess.h"

std::string VideoInfoFull::getGuid() const
{
    auto guid = videoView->Identifier + videoView->AlternateId + videoView->VideoId + downloadConfig->downloadDir +
                std::to_string(static_cast<int>(downloadConfig->videoQuality)) + fileName();
    guid = util::removeSpecialChars(guid);
    return guid;
}

std::unordered_map<std::string, std::string> VideoInfoFull::nameRules() const
{
    std::unordered_map<std::string, std::string> result;
    if (!videoView)
    {
        return result;
    }

    result.insert({ruleList[0], videoView->Identifier});
    result.insert({ruleList[1], videoView->Title});
    result.insert({ruleList[2], videoView->Publisher});
    result.insert({ruleList[3], videoView->PublishDate});
    return result;
}

std::string VideoInfoFull::fileName() const
{
    std::string result;
    if (!downloadConfig || !videoView)
    {
        return result;
    }

    std::string temp = downloadConfig->nameRule;
    std::string subRule = "$" + ruleList[0] + "$";
    if (auto pos = temp.find(subRule); pos != std::string::npos)
    {
        temp.replace(pos, subRule.size(), videoView->Identifier);
    }
    subRule = "$" + ruleList[1] + "$";
    if (auto pos = temp.find(subRule); pos != std::string::npos)
    {
        temp.replace(pos, subRule.size(), videoView->Title);
    }
    subRule = "$" + ruleList[2] + "$";
    if (auto pos = temp.find(subRule); pos != std::string::npos)
    {
        temp.replace(pos, subRule.size(), videoView->Publisher);
    }
    subRule = "$" + ruleList[3] + "$";
    if (auto pos = temp.find(subRule); pos != std::string::npos)
    {
        temp.replace(pos, subRule.size(), videoView->PublishDate);
    }

    if (temp.empty())
    {
        temp = videoView->Title;
    }

    temp = util::removeSpecialChars(temp);

    return temp;
}

std::string VideoInfoFull::coverPath() const
{
    return getGuid();
}

const std::vector<std::string> VideoInfoFull::ruleList = {"id", "title", "publisher", "data"};
