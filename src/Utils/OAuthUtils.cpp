//
// Created by vastrakai on 9/3/2024.
//

#include "OAuthUtils.hpp"

bool OAuthUtils::hasValidToken()
{
    HttpRequest request(HttpMethod::GET, sEndpoint + "isAuthenticated?id=" + getToken(), "", "", [](HttpResponseEvent event) {}, nullptr);

    HttpResponseEvent event = request.send();

    spdlog::info(xorstr_("Status code: {}, response {}"), event.mStatusCode, event.mResponse);

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
