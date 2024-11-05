//
// Created by alteik on 30/10/2024.
//

#include "Authorization.hpp"
#include "CustomCryptor.hpp"
#include <src/Utils/OAuthUtils.hpp>

void Auth::init()
{
    CustomCryptor cryptor(65537, 2753, 3233);

    if (!FileUtils::fileExists(authFile))
    {
        nlohmann::json authData;

        authData[xorstr_("password")] = "";
        authData[xorstr_("username")] = "";

        std::ofstream file(authFile);
        file << authData.dump(4);
        file.close();
        exit();
    }
    else
    {
        std::ifstream file(authFile);
        nlohmann::json authData;
        file >> authData;
        file.close();

        mPassword = authData.value(xorstr_("password"), "");
        mUsername = authData.value(xorstr_("username"), "");
    }

    mHWID = HWUtils::getCpuInfo().toString();
    if(mHWID.empty()) exit();

    if (!InternetCheckConnectionA(xorstr_("https://dllserver.solstice.works"), FLAG_ICC_FORCE_CONNECTION, 0)) {
        exit();
    }

    mHash = cryptor.encrypt(mUsername + ':' + mPassword + ':' + mHWID);
}

void Auth::exit()
{
#ifndef __DEBUG__
    __fastfail(0);
#endif
}

bool Auth::isPrivateUser()
{
    HttpRequest request(HttpMethod::GET, url + mHash, "", "", [](HttpResponseEvent event) {}, nullptr);
    HttpResponseEvent event = request.send();

    if(event.mStatusCode == 200)
    {
        try
        {
            nlohmann::json json = nlohmann::json::parse(event.mResponse);
            bool success = false;

            try
            {
                success = json[xorstr_("isPrivateUser")].get<bool>();
            }
            catch(...)
            {
                return false;
            }

            return success;
        } catch (nlohmann::json::exception& e) {
#ifdef __DEBUG__
            Solstice::console->error(e.what());
#endif
            return false;
        }
    }

    return false;
}
