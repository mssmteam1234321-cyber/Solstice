#pragma once
//
// Created by vastrakai on 7/21/2024.
//

#include <SDK/Minecraft/mce.hpp>


class HWUtils {
public:
    static mce::UUID getHWID();
};

/*
std::vector<std::string> hwid_parts;
    // Same as above, but completely wrapped in xorstr_
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), xorstr_("ProcessorNameString"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), xorstr_("Identifier"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), xorstr_("VendorIdentifier"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS"), xorstr_("BaseBoardProduct"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS"), xorstr_("BIOSVendor"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS"), xorstr_("SystemManufacturer"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS"), xorstr_("SystemProductName"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS"), xorstr_("SystemSKU"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS"), xorstr_("SystemFamily"), xorstr_("Unknown")));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum"), xorstr_("0"), xorstr_("Unknown"))); // Disk Serial Number
    auto installTime = RegUtils::readReg<uint64_t>(xorstr_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), xorstr_("InstallDate"), 0);
    hwid_parts.push_back(fmt::format(xorstr_("{:x}"), installTime));
    hwid_parts.push_back(RegUtils::readReg(xorstr_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), xorstr_("ProductId"), xorstr_("Unknown")));


    std::string hwid_str;
    for (const auto& part : hwid_parts) hwid_str += part;
    return mce::UUID::fromString(SHA256::hash(hwid_str));*/

// Gets the HWID of the system (inline for performance)
#define GET_HWID() \
    []() -> mce::UUID { \
        std::vector<std::string> hwid_parts; \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0")), std::string(xorstr_("ProcessorNameString")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0")), std::string(xorstr_("Identifier")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0")), std::string(xorstr_("VendorIdentifier")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS")), std::string(xorstr_("BaseBoardProduct")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS")), std::string(xorstr_("BIOSVendor")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS")), std::string(xorstr_("SystemManufacturer")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS")), std::string(xorstr_("SystemProductName")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS")), std::string(xorstr_("SystemSKU")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("HARDWARE\\DESCRIPTION\\System\\BIOS")), std::string(xorstr_("SystemFamily")), std::string(xorstr_("Unknown")))); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("SYSTEM\\CurrentControlSet\\Services\\Disk\\Enum")), std::string(xorstr_("0")), std::string(xorstr_("Unknown")))); \
        auto installTime = RegUtils::readReg<uint64_t>(std::string(xorstr_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion")), std::string(xorstr_("InstallDate")), 0); \
        hwid_parts.push_back(fmt::format(xorstr_("{:x}"), installTime)); \
        hwid_parts.push_back(RegUtils::readReg(std::string(xorstr_("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion")), std::string(xorstr_("ProductId")), std::string(xorstr_("Unknown")))); \
        std::string hwid_str; \
        for (const auto& part : hwid_parts) hwid_str += part; \
        return mce::UUID::fromString(SHA256::hash(hwid_str)); \
    }()