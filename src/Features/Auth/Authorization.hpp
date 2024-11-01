//
// Created by alteik on 30/10/2024.
//

class Auth
{
public:
    std::string mHWID = "";
    std::string mDiscordUserID = "";
    std::string mHash = "";
    static inline std::string url = xorstr_("https://dllserver.solstice.works/isPrivateUser?id=");

    void init();
    void exit();
    bool isPrivateUser();
};