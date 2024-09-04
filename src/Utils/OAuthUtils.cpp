//
// Created by vastrakai on 9/3/2024.
//

#include "OAuthUtils.hpp"

bool OAuthUtils::hasValidToken()
{

    static uint64_t lastAttempt = 0;
    static bool lastResult = false;

    // If the last attempt was less than 5 seconds ago, return the last result
    if (lastAttempt + 5000 > NOW)
    {
        //spdlog::info(xorstr_("Returning last result"));
        return lastResult;
    }

    HttpRequest request(HttpMethod::GET, sEndpoint + xorstr_("isAuthenticated?id=") + getToken(), "", "", [](HttpResponseEvent event) {}, nullptr);
    HttpResponseEvent event = request.send();



    //spdlog::info(xorstr_("Status code: {}, response {}"), event.mStatusCode, event.mResponse);

    lastAttempt = NOW;
    lastResult = event.mStatusCode == 200;

    if (event.mStatusCode == 200)
    {
        return true;
    }
    else
    {
        return false;
    }

}

std::string OAuthUtils::getToken()
{
    std::string tokenPath = FileUtils::getSolsticeDir() + xorstr_("uniqueId.txt");
    if (FileUtils::fileExists(tokenPath))
    {
        std::ifstream file(tokenPath);
        std::string token;
        std::getline(file, token);
        return token;
    }
    return "";
}

std::string OAuthUtils::getLatestCommitHash()
{
    HttpRequest request(HttpMethod::GET, sEndpoint + "getLatestHash", "", "", [](HttpResponseEvent event) {}, nullptr);

    HttpResponseEvent event = request.send();

    //var obj = new { commitHash = LatestCommitHash };
    //return JsonConvert.SerializeObject(obj);

    nlohmann::json json = nlohmann::json::parse(event.mResponse);

    return json["commitHash"].get<std::string>();
}
