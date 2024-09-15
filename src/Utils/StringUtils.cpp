//
// Created by vastrakai on 6/28/2024.
//

#include "StringUtils.hpp"

#include <algorithm>
#include <hex.h>
#include <windows.h>
#include <regex>
#include "spdlog/spdlog.h"
#include <cryptopp/files.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <random>

std::vector<std::string> motherboardModels = {
        "ASUS ROG Strix Z590-E",
        "MSI MPG Z490 Gaming Edge WiFi",
        "Gigabyte Z590 AORUS Master",
        "ASRock B450M PRO4",
        "ASUS TUF Gaming X570-PLUS",
        "MSI B450 TOMAHAWK MAX",
        "Gigabyte B550 AORUS Elite",
        "ASRock Z490 Phantom Gaming 4",
        "ASUS Prime B560M-A",
        "MSI MAG B550 TOMAHAWK",
        "Gigabyte B450M DS3H",
        "ASRock H570M Pro4",
        "ASUS ROG Crosshair VIII Hero",
        "MSI MPG Z390 Gaming PRO Carbon",
        "Gigabyte Z390 AORUS ULTRA",
        "ASRock B365M PRO4",
        "ASUS Prime Z490-P",
        "MSI MAG Z490 TOMAHAWK",
        "Gigabyte Z490 VISION G",
        "ASRock B460M Steel Legend",
        "ASUS ROG Strix B450-F Gaming",
        "MSI MPG B550 Gaming Plus",
        "Gigabyte Z390 GAMING X",
        "ASRock H410M-HDV",
        "ASUS TUF Z390-PLUS GAMING",
        "MSI B450 GAMING PLUS MAX",
        "Gigabyte B460M DS3H",
        "ASRock X570 Phantom Gaming 4",
        "ASUS Prime B365M-A",
        "MSI Z370 GAMING PLUS",
        "Gigabyte X570 AORUS ELITE",
        "ASRock B550M Steel Legend",
        "ASUS ROG Strix X570-E Gaming",
        "MSI MEG Z490 GODLIKE",
        "Gigabyte A520M DS3H",
        "ASRock Z390 Phantom Gaming 9",
        "ASUS TUF B450-PLUS GAMING",
        "MSI B365M PRO-VDH",
        "Gigabyte Z370 HD3P",
        "ASRock H310M-HDV",
        "ASUS Prime A320M-K",
        "MSI H310M PRO-VDH PLUS",
        "Gigabyte B450 I AORUS PRO WIFI",
        "ASRock B450M Steel Legend",
        "ASUS ROG Maximus XII Hero",
        "MSI MPG Z590 Gaming Carbon WiFi",
        "Gigabyte Z590 AORUS PRO AX",
        "ASRock B365 Phantom Gaming 4",
        "ASUS TUF Gaming B460-PRO",
        "MSI Z490-A PRO",
        "Gigabyte Z390 AORUS PRO WIFI",
        "ASRock H410M-HDV/M.2",
        "ASUS Prime Z390-P",
        "MSI MEG X570 UNIFY",
        "Gigabyte B365M DS3H",
        "ASRock X470 Taichi",
        "ASUS ROG Strix Z490-E Gaming",
        "MSI MAG B460M MORTAR WIFI",
        "Gigabyte Z490M GAMING X",
        "ASRock B460 Steel Legend",
        "ASUS Prime H310M-E R2.0",
        "MSI B450I GAMING PLUS AC",
        "Gigabyte H370 HD3",
        "ASRock B360M Pro4",
        "ASUS TUF Gaming B550M-PLUS",
        "MSI MPG Z490 GAMING EDGE WIFI",
        "Gigabyte B360 AORUS GAMING 3 WIFI",
        "ASRock Z370 Killer SLI",
        "ASUS Prime H310M-D R2.0",
        "MSI Z390-A PRO",
        "Gigabyte B365M D3H",
        "ASRock B365M Phantom Gaming 4",
        "ASUS TUF B360-PRO Gaming",
        "MSI H310M PRO-M2 PLUS",
        "Gigabyte Z370 AORUS ULTRA GAMING WIFI",
        "ASRock H370M-ITX/ac",
        "ASUS Prime B250M-A",
        "MSI B360 GAMING PLUS",
        "Gigabyte H310M S2H",
        "ASRock B250M Pro4",
        "ASUS TUF Z370-PLUS Gaming",
        "MSI B450M GAMING PLUS",
        "Gigabyte B460M GAMING HD",
        "ASRock Z370 Extreme4",
        "ASUS Prime Z270-A",
        "MSI H270 GAMING M3",
        "Gigabyte H270-HD3",
        "ASRock B250M-HDV",
        "ASUS Prime H270-PLUS",
        "MSI B250M Mortar",
        "Gigabyte GA-Z270X-Gaming K5",
        "ASRock H110M-DGS",
        "ASUS Prime A320M-E",
        "MSI H110M PRO-D",
        "Gigabyte GA-H110M-A",
        "ASRock Z170 Extreme6",
        "ASUS Z170-A",
        "MSI Z170A GAMING M5",
        "Gigabyte GA-Z170X-Gaming 7",
        "ASRock H97M Pro4",
        "ASUS H97-PLUS",
        "MSI H97 GAMING 3",
        "Gigabyte GA-H97-D3H",
        "ASUS Prime Z270-A",
        "MSI B350 TOMAHAWK",
        "Gigabyte GA-AX370-Gaming K7",
        "ASRock AB350M Pro4",
        "ASUS ROG Crosshair VI Hero",
        "MSI X470 GAMING PRO CARBON",
        "Gigabyte GA-AB350-Gaming 3",
        "ASRock X399 Taichi",
        "ASUS ROG Zenith Extreme",
        "MSI X399 GAMING PRO CARBON AC",
        "Gigabyte X299 AORUS Gaming 7",
        "ASRock X299 Taichi",
        "ASUS Prime X299-DELUXE",
        "MSI X299 SLI PLUS",
        "Gigabyte Z270X-Ultra Gaming",
        "ASRock Z270 Taichi",
        "ASUS ROG Maximus IX Hero",
        "MSI Z270 GAMING PRO CARBON",
        "Gigabyte GA-H270-Gaming 3",
        "ASRock H270M Pro4",
        "ASUS Prime H270M-PLUS",
        "MSI H270 TOMAHAWK",
        "Gigabyte GA-B250M-DS3H",
        "ASRock B250M Pro4",
        "ASUS Prime B250M-PLUS",
        "MSI B250M BAZOOKA",
        "Gigabyte GA-Z170X-Gaming GT",
        "ASRock Z170 Pro4",
        "ASUS Z170-DELUXE",
        "MSI Z170A KRAIT GAMING 3X",
        "Gigabyte GA-Z170X-UD3",
        "ASRock H110M-ITX/ac",
        "ASUS H110M-E/M.2",
        "MSI H110M GAMING",
        "Gigabyte GA-H110M-S2H",
        "ASRock H81M-HDS",
        "ASUS H81M-A",
        "MSI H81M-P33",
        "Gigabyte GA-H81M-DS2V",
        "ASRock Z97 Extreme4",
        "ASUS Z97-PRO",
        "MSI Z97 GAMING 5",
        "Gigabyte GA-Z97X-UD5H",
        "ASRock H97 Anniversary",
        "ASUS H97M-E",
        "MSI H97 PC MATE",
        "Gigabyte GA-H97N-WIFI",
        "ASRock B85M Pro4",
        "ASUS B85M-G",
        "MSI B85-G43 GAMING",
        "Gigabyte GA-B85M-DS3H",
        "ASRock Z87 Extreme4",
        "ASUS Z87-A",
        "MSI Z87-G45 GAMING",
        "Gigabyte GA-Z87X-UD3H",
        "ASRock H87 Pro4",
        "ASUS H87M-E",
        "MSI H87-G43",
        "Gigabyte GA-H87-D3H",
        "ASRock B75 Pro3",
        "ASUS P8B75-M",
        "MSI B75MA-P45",
        "Gigabyte GA-B75M-D3H",
        "ASRock Z77 Extreme4",
        "ASUS P8Z77-V",
        "MSI Z77A-G45",
        "Gigabyte GA-Z77X-UD3H",
        "ASRock H61M-HVS",
        "ASUS P8H61-M LX3",
        "MSI H61M-P31/W8",
        "Gigabyte GA-H61M-DS2",
        "ASRock A320M-HDV",
        "ASUS PRIME A320M-C R2.0",
        "MSI A320M PRO-VD/S",
        "Gigabyte GA-A320M-S2H",
        "ASRock FM2A68M-HD+",
        "ASUS A68HM-K",
        "MSI A68HM GRENADE",
        "Gigabyte GA-F2A68HM-S1",
        "ASRock 970M Pro3",
        "ASUS M5A97 LE R2.0",
        "MSI 970 GAMING",
        "Gigabyte GA-970A-DS3P",
        "ASRock 990FX Extreme6",
        "ASUS SABERTOOTH 990FX R2.0",
        "MSI 990FXA GAMING",
        "Gigabyte GA-990FXA-UD3",
        "ASRock A88M-G/3.1",
        "ASUS A88XM-A",
        "MSI A88XM GAMING",
        "Gigabyte GA-F2A88XM-D3H",
        "ASRock H61M-VG3",
        "ASUS H61M-K",
        "MSI H61M-E33/W8",
        "Gigabyte GA-H61M-USB3V",
        "ASRock Q1900-ITX",
        "ASUS J1800I-C",
        "MSI C847MS-E33",
        "Gigabyte GA-J1800N-D2P"
};

std::string StringUtils::generateMboard() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> modelDistr(0, motherboardModels.size() - 1);

    int modelIndex = modelDistr(gen);
    std::string mboard = motherboardModels[modelIndex];

    return mboard;
}

std::string StringUtils::generateUUID()
{
    // Generate a UUID v4
    std::string uuid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

    for (size_t i = 0; i < uuid.size(); i++)
    {
        char c = uuid[i];
        if (c == 'x')
        {
            uuid[i] = "0123456789abcdef"[std::rand() % 16];
        }
        else if (c == 'y')
        {
            uuid[i] = "89ab"[std::rand() % 4];
        }
    }

    return uuid;
}

bool StringUtils::startsWith(std::string_view str, std::string_view prefix)
{
    return str.substr(0, prefix.size()) == prefix;
}

bool StringUtils::endsWith(std::string_view str, std::string_view suffix)
{
    return str.substr(str.size() - suffix.size(), suffix.size()) == suffix;
}

std::string StringUtils::RemoveColorCodes(std::string str) {
    return std::regex_replace(std::regex_replace(str, std::regex("§."), ""), std::regex("&."), "");
}

std::string_view StringUtils::trim(std::string_view str)
{
    size_t start = 0;
    size_t end = str.size();

    while (start < end && std::isspace(str[start])) {
        start++;
    }

    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }

    return str.substr(start, end - start);
}

std::vector<std::string> StringUtils::split(std::string_view str, char delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
    }

    result.emplace_back(str.substr(start));

    return result;
}

// toLower and toUpper
std::string StringUtils::toLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}

std::string StringUtils::toUpper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
    return str;
}

bool StringUtils::equalsIgnoreCase(const std::string& str1, const std::string& str2)
{
    return toLower(str1) == toLower(str2);
}

bool StringUtils::containsIgnoreCase(const std::string& str, const std::string& subStr)
{
    return toLower(str).find(toLower(subStr)) != std::string::npos;
}

bool StringUtils::containsAnyIgnoreCase(const std::string& str, const std::vector<std::string>& strVector)
{
    if (std::ranges::any_of(strVector, [&str](const std::string& subStr) { return containsIgnoreCase(str, subStr); }))
    {
        return true;
    }

    return false;
}

std::string StringUtils::getClipboardText()
{
    // Try opening the clipboard
    if (! OpenClipboard(nullptr))
    {
        spdlog::error("Failed to open clipboard: {}", GetLastError());
        return "";
    }

    // Get handle of clipboard object for ANSI text
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr)
    {
        spdlog::error("Failed to get clipboard text: {}", GetLastError());
        return "";
    }

    // Lock the handle to get the actual text pointer
    char * pszText = static_cast<char*>( GlobalLock(hData) );
    if (pszText == nullptr)
    {
        spdlog::error("Failed to get clipboard text: {}", GetLastError());
        return "";
    }

    // Save text in a string class instance
    std::string text( pszText );

    // Release the lock
    GlobalUnlock( hData );

    // Release the clipboard
    CloseClipboard();

    return text;
}

std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter)
{
    std::string result;
    for (size_t i = 0; i < strings.size(); i++)
    {
        result += strings[i];
        if (i != strings.size() - 1)
        {
            result += delimiter;
        }
    }

    return result;
}

std::string StringUtils::replaceAll(std::string& string, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = string.find(from, start_pos)) != std::string::npos)
    {
        string.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return string;
}

std::string StringUtils::sha256(const std::string& str)
{
    return SHA256::hash(str);
}

std::string StringUtils::fromBase64(const std::string& str)
{
    return Base64::decode(str);
}

std::string StringUtils::toBase64(const std::string& str)
{
    return Base64::encode(str);
}

std::string StringUtils::getRelativeTime(std::chrono::system_clock::time_point time)
{
    auto now = std::chrono::system_clock::now();

    auto diff = now - time;

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(diff);
    if (seconds.count() < 60) return std::to_string(seconds.count()) + " seconds ago";

    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(diff);
    if (minutes.count() < 60) return std::to_string(minutes.count()) + " minutes ago";

    auto hours = std::chrono::duration_cast<std::chrono::hours>(diff);
    if (hours.count() < 24) return std::to_string(hours.count()) + " hours ago";

    auto days = std::chrono::duration_cast<std::chrono::days>(diff);
    if (days.count() < 30) return std::to_string(days.count()) + " days ago";

    auto months = std::chrono::duration_cast<std::chrono::months>(diff);
    if (months.count() < 12) return std::to_string(months.count()) + " months ago";

    auto years = std::chrono::duration_cast<std::chrono::years>(diff);
    return std::to_string(years.count()) + " years ago";
}

/*
public class StringCipher
{
    public static string Encode(string input)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(input);

        for (int i = 0; i < bytes.Length - 1; i += 2) (bytes[i], bytes[i + 1]) = (bytes[i + 1], bytes[i]);
        for (int i = 0; i < bytes.Length; i++) bytes[i] = (byte)~bytes[i];

        return Convert.ToBase64String(bytes);
    }

    public static string Decode(string input)
    {
        byte[] bytes = Convert.FromBase64String(input);

        for (int i = 0; i < bytes.Length; i++) bytes[i] = (byte)~bytes[i];
        for (int i = 0; i < bytes.Length - 1; i += 2) (bytes[i], bytes[i + 1]) = (bytes[i + 1], bytes[i]);

        return Encoding.UTF8.GetString(bytes);
    }
}
*/




std::string StringUtils::decode(const std::string& str)
{
    std::vector<uint8_t> bytes = Base64::decodeBytes(str);

    for (auto& byte : bytes) {
        byte = ~byte;
    }

    for (size_t i = 0; i < bytes.size() - 1; i += 2) {
        std::swap(bytes[i], bytes[i + 1]);
    }

    return std::string(bytes.begin(), bytes.end());
}

std::string StringUtils::encode(const std::string& input)
{
    std::vector<uint8_t> bytes(input.begin(), input.end());

    for (size_t i = 0; i < bytes.size() - 1; i += 2) {
        std::swap(bytes[i], bytes[i + 1]);
    }

    for (auto& byte : bytes) {
        byte = ~byte;
    }

    return Base64::encodeBytes(bytes);
}
/*
public static string Encrypt(string plaintext, string key)
    {
        // Truncate key to 16 bytes (128-bit)
        byte[] truncatedKey = new byte[16];
        Array.Copy(Encoding.UTF8.GetBytes(key), truncatedKey, Math.Min(16, key.Length));

        // simple encryption
        byte[] plaintextBytes = Encoding.UTF8.GetBytes(plaintext);
        byte[] ciphertextBytes = new byte[plaintextBytes.Length];
        for (int i = 0; i < plaintextBytes.Length; i++)
        {
            ciphertextBytes[i] = (byte)(plaintextBytes[i] ^ truncatedKey[i % 16]);
        }

        return Convert.ToBase64String(ciphertextBytes);
    }

    public static string Decrypt(string ciphertext, string key)
    {
        // Truncate key to 16 bytes (128-bit)
        byte[] truncatedKey = new byte[16];
        Array.Copy(Encoding.UTF8.GetBytes(key), truncatedKey, Math.Min(16, key.Length));

        // simple decryption
        byte[] ciphertextBytes = Convert.FromBase64String(ciphertext);
        byte[] plaintextBytes = new byte[ciphertextBytes.Length];
        for (int i = 0; i < ciphertextBytes.Length; i++)
        {
            plaintextBytes[i] = (byte)(ciphertextBytes[i] ^ truncatedKey[i % 16]);
        }

        return Encoding.UTF8.GetString(plaintextBytes);
    }*/

using namespace CryptoPP;
class EncUtils
{
public:
    static std::string Encrypt(const std::string& plaintext, const std::string& key) {
        // Truncate key to 16 bytes
        std::string truncatedKey = key.substr(0, 16);
        // add 2 to every byte in the key
        for (auto& byte : truncatedKey) {
            byte += 2;
        }

        // Simple encryption
        std::vector<unsigned char> plaintextBytes(plaintext.begin(), plaintext.end());
        std::vector<unsigned char> ciphertextBytes(plaintextBytes.size());

        for (size_t i = 0; i < plaintextBytes.size(); i++) {
            ciphertextBytes[i] = plaintextBytes[i] ^ truncatedKey[i % 16];
        }

        return Base64::encode(std::string(ciphertextBytes.begin(), ciphertextBytes.end()));
    }

    static std::string Decrypt(const std::string& ciphertext, const std::string& key) {
        // Truncate key to 16 bytes
        std::string truncatedKey = key.substr(0, 16);
        // add 2 to every byte in the key
        for (auto& byte : truncatedKey) {
            byte += 2;
        }

        // Simple decryption
        std::vector<unsigned char> ciphertextBytes = Base64::decodeBytes(ciphertext);
        std::vector<unsigned char> plaintextBytes(ciphertextBytes.size());

        for (size_t i = 0; i < ciphertextBytes.size(); i++) {
            plaintextBytes[i] = ciphertextBytes[i] ^ truncatedKey[i % 16];
        }

        return std::string(plaintextBytes.begin(), plaintextBytes.end());
    }
};

std::string StringUtils::encrypt(const std::string& input, const std::string& key) {
    return EncUtils::Encrypt(input, key);
}

std::string StringUtils::decrypt(const std::string& input, const std::string& key) {
    return EncUtils::Decrypt(input, key);
}

std::string StringUtils::toHex(const std::vector<uint8_t>& data)
{
    return ""; // not implemented
}

bool StringUtils::contains(const std::string& str, const std::string& subStr)
{
    return str.find(subStr) != std::string::npos;
}

std::string StringUtils::replace(const std::string& str, const std::string& from, const std::string& to)
{
    // replace only the first occurrence
    std::string result = str;
    size_t start_pos = result.find(from);
    if (start_pos != std::string::npos)
    {
        result.replace(start_pos, from.length(), to);
    }
    return result;
}

std::string StringUtils::randomString(int length)
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    ranges::shuffle(str, generator);

    return str.substr(0, length);};
