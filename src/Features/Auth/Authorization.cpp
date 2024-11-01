//
// Created by alteik on 30/10/2024.
//

#include "Authorization.hpp"
#include "CustomCryptor.hpp"
#include <src/Utils/OAuthUtils.hpp>

void Auth::init()
{
    CustomCryptor cryptor(65537, 2753, 3233);

    if (FileUtils::fileExists(injectLogsFile))
    {
        FileUtils::deleteFile(injectLogsFile);
    }

    std::ofstream logs(injectLogsFile);

    mHWID = HWUtils::getCpuInfo().toString();
    if(mHWID.empty())
    {

        logs << xorstr_("Error: 0x0\n");
        logs.close();
        exit();
    }
    else
    {
        if (FileUtils::fileExists(lastHwidFile))
        {
            FileUtils::deleteFile(lastHwidFile);

            std::ofstream file(lastHwidFile);
            file << cryptor.encrypt(mHWID);
            file.close();
        }
        else
        {
            std::ofstream file(lastHwidFile);
            file << cryptor.encrypt(mHWID);
            file.close();
        }
    }

    mDiscordUserID = OAuthUtils::getToken();
    if(mDiscordUserID.empty())
    {
        logs << xorstr_("Error: 0x1\n");
        logs.close();
        exit();
    }

    if(!InternetGetConnectedState(nullptr, 0))
    {
        logs << xorstr_("Error: 0x2\n");
        logs.close();
        exit();
    }

    mHash = cryptor.encrypt(mDiscordUserID + mHWID);

    logs.close();
}

void Auth::exit()
{
    __fastfail(0);
}

bool Auth::isPrivateUser()
{
    if (FileUtils::fileExists(injectLogsFile))
    {
        FileUtils::deleteFile(injectLogsFile);
    }

    std::ofstream logs(injectLogsFile);

    HttpRequest request(HttpMethod::GET, url + mHash, "", "", [](HttpResponseEvent event) {}, nullptr);
    HttpResponseEvent event = request.send();

    logs << xorstr_("sent request\n");

    if(event.mStatusCode == 200)
    {
        logs << xorstr_("status: 0x1\n");
        nlohmann::json json = nlohmann::json::parse(event.mResponse);

        if(json[xorstr_("isPrivateUser")].get<bool>())
        {
            logs << xorstr_("auth: successful\n");
            logs.close();
            return true;
        }
        else
        {
            logs << xorstr_("auth: failed\n");
            logs.close();
            return false;
        }
    }
    else if(event.mStatusCode == 500)
    {
        logs << xorstr_("status: 0x2\n");
    }

    return false;
}