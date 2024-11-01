//
// Created by alteik on 30/10/2024.
//

class Auth
{
public:
    std::string mHWID = "";
    std::string mDiscordUserID = "";
    std::string mHash = "";
    //std::string lastHwidFile = FileUtils::getSolsticeDir() + xorstr_("hwid.txt");
    //std::string injectLogsFile = FileUtils::getSolsticeDir() + xorstr_("logs.txt");
    static inline std::string url = xorstr_("https://dllserver.solstice.works/isPrivateUser?id=");

    void init();
    void exit();
    bool isPrivateUser();
};