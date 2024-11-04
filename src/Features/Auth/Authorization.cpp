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

        authData["password"] = "";
        authData["username"] = "";

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

        mPassword = authData.value("password", "");
        mUsername = authData.value("username", "");
    }

    mHWID = HWUtils::getCpuInfo().toString();
    if(mHWID.empty()) exit();

    if (!InternetCheckConnectionA("https://dllserver.solstice.works", FLAG_ICC_FORCE_CONNECTION, 0)) {
        exit();
    }

    mHash = cryptor.encrypt(mUsername + ":" + mPassword + ":" + mHWID);
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