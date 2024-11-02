//
// Created by alteik on 30/10/2024.
//

class Auth
{
public:
    std::string mHWID = "";
    std::string mUsername = "";
    std::string mPassword = "";
    std::string mHash = "";
    std::string authFile = FileUtils::getSolsticeDir() + xorstr_("auth.txt");
    static inline std::string url = xorstr_("https://dllserver.solstice.works/isPrivateUser?id=");

    void init();
    void exit();
    bool isPrivateUser();
};