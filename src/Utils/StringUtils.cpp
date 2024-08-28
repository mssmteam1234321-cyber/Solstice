//
// Created by vastrakai on 6/28/2024.
//

#include "StringUtils.hpp"

#include <algorithm>
#include <hex.h>
#include <windows.h>
#include "spdlog/spdlog.h"
#include <cryptopp/files.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>

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