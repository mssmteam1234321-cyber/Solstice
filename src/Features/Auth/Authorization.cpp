//
// Created by alteik on 30/10/2024.
//

#include "Authorization.hpp"
#include "CustomCryptor.hpp"
#include <src/Utils/OAuthUtils.hpp>

void Auth::init()
{
    CustomCryptor cryptor(xorstr_("PleaseDoNotReverseMyAss"));

    mHWID = HWUtils::getCpuInfo().toString();
    if(mHWID.empty())
    {
        exit();
    }
    else
    {
        std::string lastHwidFile = FileUtils::getSolsticeDir() + xorstr_("hwid.txt");

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
    if(mDiscordUserID.empty()) exit();

    if(!InternetGetConnectedState(nullptr, 0)) exit();

    mHash = cryptor.encrypt(mDiscordUserID + mHWID);
}

void Auth::exit()
{
    __fastfail(0);
}

bool Auth::isPrivateUser()
{
    HttpRequest request(HttpMethod::GET, url + mHash, "", "", [](HttpResponseEvent event) {}, nullptr);
    HttpResponseEvent event = request.send();

    if(event.mStatusCode == 200)
    {
        nlohmann::json json = nlohmann::json::parse(event.mResponse);
        return json[xorstr_("isPrivateUser")].get<bool>();
    }

    return false;
}