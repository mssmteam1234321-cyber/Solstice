//
// Created by alteik on 30/10/2024.
//

class Auth
{
public:
    std::string UniqueID = "";
    std::string uniqueIdFile = FileUtils::getSolsticeDir() + xorstr_("uniqueId.txt");
    static inline std::string url = xorstr_("https://dllserver.solstice.works/hasPrivateAccess?uniqueId=");

    void init();
    void exit();
    bool isPrivateUser();
};