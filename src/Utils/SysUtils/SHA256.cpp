//
// Created by vastrakai on 7/21/2024.
//

#include "SHA256.hpp"

SHA256::SHA256() {}

void SHA256::update(const unsigned char* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        buffer[buffer_length++] = data[i];
        if (buffer_length == buffer.size()) {
            transform(buffer.data());
            bit_length += 512;
            buffer_length = 0;
        }
    }
}

void SHA256::update(const std::string& data) {
    update(reinterpret_cast<const unsigned char*>(data.c_str()), data.size());
}

std::string SHA256::final() {
    bit_length += buffer_length * 8;
    buffer[buffer_length++] = 0x80;
    if (buffer_length > 56) {
        while (buffer_length < 64) {
            buffer[buffer_length++] = 0x00;
        }
        transform(buffer.data());
        buffer_length = 0;
    }

    while (buffer_length < 56) {
        buffer[buffer_length++] = 0x00;
    }

    for (int i = 7; i >= 0; --i) {
        buffer[buffer_length++] = (bit_length >> (i * 8)) & 0xff;
    }

    transform(buffer.data());

    std::stringstream ss;
    for (auto val : h) {
        ss << std::hex << std::setw(8) << std::setfill('0') << val;
    }
    return ss.str();
}

std::string SHA256::hash(const std::string& data) {
    SHA256 sha256;
    sha256.update(data);
    return sha256.final();
}

void SHA256::transform(const unsigned char* data) {
    std::array<uint32_t, 64> w;
    for (size_t i = 0; i < 16; ++i) {
        w[i] = (data[i * 4] << 24) | (data[i * 4 + 1] << 16) | (data[i * 4 + 2] << 8) | (data[i * 4 + 3]);
    }
    for (size_t i = 16; i < 64; ++i) {
        w[i] = sig1(w[i - 2]) + w[i - 7] + sig0(w[i - 15]) + w[i - 16];
    }

    uint32_t a = h[0];
    uint32_t b = h[1];
    uint32_t c = h[2];
    uint32_t d = h[3];
    uint32_t e = h[4];
    uint32_t f = h[5];
    uint32_t g = h[6];
    uint32_t h0 = h[7];

    for (size_t i = 0; i < 64; ++i) {
        uint32_t t1 = h0 + sig1(e) + choose(e, f, g) + k[i] + w[i];
        uint32_t t2 = sig0(a) + majority(a, b, c);
        h0 = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    h[0] += a;
    h[1] += b;
    h[2] += c;
    h[3] += d;
    h[4] += e;
    h[5] += f;
    h[6] += g;
    h[7] += h0;
}

uint32_t SHA256::rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t SHA256::choose(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

uint32_t SHA256::majority(uint32_t a, uint32_t b, uint32_t c) {
    return (a & b) ^ (a & c) ^ (b & c);
}

uint32_t SHA256::sig0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t SHA256::sig1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}