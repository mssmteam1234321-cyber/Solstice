//
// Created by vastrakai on 7/26/2024.
//

#include "Base64.hpp"

std::string Base64::encode(const std::string& input)
{
    // Base64 encoding

    // 1. Convert each character to its ASCII value
    // 2. Convert the ASCII value to binary
    // 3. Group the binary values into groups of 6 bits
    // 4. Convert each group of 6 bits to its decimal value
    // 5. Map the decimal value to the corresponding Base64 character

    // Base64 character set
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;

    // Convert each character to its ASCII value
    for (size_t i = 0; i < input.size(); i++)
    {
        char c = input[i];
        // Convert the character to its ASCII value
        int ascii_value = static_cast<int>(c);
        // Convert the ASCII value to binary
        std::string binary_value;
        while (ascii_value > 0)
        {
            binary_value = std::to_string(ascii_value % 2) + binary_value;
            ascii_value /= 2;
        }
        // Pad the binary value with zeros to make it 8 bits long
        while (binary_value.size() < 8)
        {
            binary_value = "0" + binary_value;
        }
        // Group the binary values into groups of 6 bits
        for (size_t j = 0; j < binary_value.size(); j += 6)
        {
            std::string group = binary_value.substr(j, 6);
            // Convert each group of 6 bits to its decimal value
            int decimal_value = 0;
            for (size_t k = 0; k < group.size(); k++)
            {
                decimal_value = decimal_value * 2 + (group[k] - '0');
            }
            // Map the decimal value to the corresponding Base64 character
            output += base64_chars[decimal_value];
        }
    }

    // Pad the output with '=' characters if the length is not a multiple of 4
    while (output.size() % 4 != 0)
    {
        output += '=';
    }

    return output;
}

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(const std::string &encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}


std::string Base64::decode(const std::string& input)
{
   return base64_decode(input);
}
