#pragma once
#include <string>
#include <atomic>
#include <memory>

#include "LoginProxy.h"
#include "BaseVideoView.h"
#include "Download/FileDownloader.h"

namespace plugin
{

class IPlugin
{
public:
    virtual ~IPlugin() = default;

    virtual const PluginMessage& pluginMessage() const = 0;

    virtual const std::vector<uint8_t>& websiteIcon() = 0;

    virtual bool canParseUrl(const std::string& url) = 0;
    virtual adapter::VideoView getVideoView(const std::string& url) = 0;

    virtual std::shared_ptr<download::FileDownloader> getDownloader(const VideoInfoFull& videoInfo) = 0;

    virtual LoginProxy loginer() = 0;
};

}  // namespace plugin
