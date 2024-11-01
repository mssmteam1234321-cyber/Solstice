//
// Created by alteik on 31/10/2024.
//
class CustomCryptor { // im just testing stuff, 0 comments plz (anyway its better then previous killswitch :>)
public:
    CustomCryptor(const std::string& key) : key(key) {}

    std::string encrypt(const std::string& text) const {
        std::string encrypted = xorCrypt(text);
        return toAlphaNumeric(encrypted);
    }

    std::string decrypt(const std::string& text) const {
        std::string fromAlphaNum = fromAlphaNumeric(text);
        return xorCrypt(fromAlphaNum);
    }

private:
    std::string key;
    const std::string alphaNum = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::string xorCrypt(const std::string& text) const {
        std::string result = text;
        for (size_t i = 0; i < text.size(); ++i) {
            result[i] = text[i] ^ key[i % key.size()];
        }
        return result;
    }

    std::string toAlphaNumeric(const std::string& text) const {
        std::string result;
        for (unsigned char c : text) {
            result += alphaNum[(c >> 4) & 0x3F];
            result += alphaNum[c & 0x3F];
        }
        return result;
    }

    std::string fromAlphaNumeric(const std::string& text) const {
        std::string result;
        for (size_t i = 0; i < text.size(); i += 2) {
            unsigned char high = alphaNum.find(text[i]) << 4;
            unsigned char low = alphaNum.find(text[i + 1]);
            result += high | low;
        }
        return result;
    }
};