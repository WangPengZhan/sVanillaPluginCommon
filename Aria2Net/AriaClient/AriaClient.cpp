#include <iomanip>
#include <random>
#include <sstream>

#include "AriaClient.h"

namespace aria2net
{

std::string GetRpcUri(int listenPort)
{
    return std::string("http://localhost:") + std::to_string(listenPort) + std::string("/jsonrpc");
}

std::string GetGuid()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::ostringstream uuid_stream;

    for (int i = 0; i < 8; ++i)
        uuid_stream << std::hex << std::setw(1) << std::setfill('0') << dis(gen);
    uuid_stream << "-";
    for (int i = 0; i < 4; ++i)
        uuid_stream << std::hex << std::setw(1) << std::setfill('0') << dis(gen);
    uuid_stream << "-";
    for (int i = 0; i < 4; ++i)
        uuid_stream << std::hex << std::setw(1) << std::setfill('0') << dis(gen);
    uuid_stream << "-";
    for (int i = 0; i < 4; ++i)
        uuid_stream << std::hex << std::setw(1) << std::setfill('0') << dis(gen);
    uuid_stream << "-";
    for (int i = 0; i < 12; ++i)
        uuid_stream << std::hex << std::setw(1) << std::setfill('0') << dis(gen);

    return uuid_stream.str();
}

AriaClient& AriaClient::globalClient()
{
    static AriaClient ariaClient;
    return ariaClient;
}

SystemListNotifications AriaClient::listNotificationsAsync()
{
    return Call<SystemListNotifications>("system.listNotifications", {});
}

SystemListMethods AriaClient::listMethodsAsync()
{
    return Call<SystemListNotifications>("system.listMethods", {});
}

std::list<SystemMulticall> AriaClient::multicallAsync(const std::list<SystemMulticallMethod>& systemMulticallMethods)
{
    auto jsonResult = Call<nlohmann::json>("system.listMethods", {systemMulticallMethods});
    std::list<SystemMulticall> result;
    for (const auto& singleJson : jsonResult)
    {
        result.emplace_back(singleJson);
    }
    return result;
}

AriaSaveSession AriaClient::saveSessionAsync()
{
    return Call<AriaSaveSession>("aria2.saveSession", {});
}

AriaShutdown AriaClient::ForceShutdownAsync()
{
    return Call<AriaShutdown>("aria2.forceShutdown", {});
}

AriaShutdown AriaClient::ShutdownAsync()
{
    return Call<AriaShutdown>("aria2.shutdown", {});
}

AriaGetSessionInfo AriaClient::GetSessionInfoAsync()
{
    return Call<AriaGetSessionInfo>("aria2.getSessionInfo", {});
}

AriaVersion AriaClient::GetAriaVersionAsync()
{
    return Call<AriaVersion>("aria2.getVersion", {});
}

AriaRemove AriaClient::RemoveDownloadResultAsync(const std::string& gid)
{
    return Call<AriaRemove>("aria2.removeDownloadResult", {gid});
}

AriaRemove AriaClient::PurgeDownloadResultAsync()
{
    return Call<AriaRemove>("aria2.purgeDownloadResult", {});
}

AriaGetGlobalStat AriaClient::GetGlobalStatAsync()
{
    return Call<AriaGetGlobalStat>("aria2.getGlobalStat", {});
}

AriaChangeOption AriaClient::ChangeGlobalOptionAsync(const ListString& option)
{
    return Call<AriaChangeOption>("aria2.getGlobalStat", {option});
}

AriaGetOption AriaClient::GetGlobalOptionAsync()
{
    return Call<AriaGetOption>("aria2.getGlobalStat", {});
}

AriaGetOption AriaClient::GetOptionAsync(const std::string& gid)
{
    return Call<AriaGetOption>("aria2.getOption", {gid});
}

AriaChangeUri AriaClient::ChangeUriAsync(const std::string& gid, int fileIndex, ListString delUris, ListString addUris, int position)
{
    return Call<AriaChangeUri>("aria2.changePosition", {gid, fileIndex, delUris, addUris, position});
}

AriaChangePosition AriaClient::ChangePositionAsync(const std::string& gid, int pos, HowChangePosition how)
{
    return Call<AriaChangePosition>("aria2.changePosition", {gid, pos, how});
}

AriaAddUri AriaClient::AddUriAsync(const ListString& uris, AriaSendOption option, int position)
{
    if (position < 0)
    {
        return Call<AriaAddUri>("aria2.addUri", {uris, option});
    }
    else
    {
        return Call<AriaAddUri>("aria2.addUri", {uris, option, position});
    }
}

AriaTellStatus AriaClient::TellStatus(const std::string& gid)
{
    return Call<AriaTellStatus>("aria2.tellStatus", {gid});
}

AriaRemove AriaClient::RemoveAsync(const std::string& gid)
{
    return Call<AriaRemove>("aria2.remove", {gid});
}

AriaPause AriaClient::PauseAsync(const std::string& gid)
{
    return Call<AriaPause>("aria2.pause", {gid});
}

AriaPause AriaClient::UnpauseAsync(const std::string& gid)
{
    return Call<AriaPause>("aria2.unpause", {gid});
}

AriaChangeOption AriaClient::ChangeOptionAsync(const std::string& gid, const ListString& option)
{
    return Call<AriaChangeOption>("aria2.getGlobalStat", {gid});
}

std::string AriaClient::Request(const std::string& url, const std::string& param)
{
    std::string strResponse;
    post(url, strResponse, param);
    return strResponse;
}

std::string AriaClient::ConstructURL() const
{
    return GetRpcUri(6800);
}

std::string AriaClient::GetToken() const
{
    return TOKEN;
}

std::string AriaClient::ConstructSendData(std::string methodName, nlohmann::json::array_t params)
{
    AriaSendData data;
    data.id = GetGuid();
    data.jsonrpc = "2.0";
    data.method = std::move(methodName);
    data.params.reserve(params.size() + 1);
    data.params = std::move(params);
    data.params.insert(data.params.begin(), "token:" + GetToken());
    if (data.params.size() > 2 && data.params[2].empty())
    {
        data.params[2] = nlohmann::json::object();
    }

    return Request(ConstructURL(), data.toString());
}

}  // namespace aria2net
