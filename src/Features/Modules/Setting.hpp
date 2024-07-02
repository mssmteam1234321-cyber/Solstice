#pragma once
#include <functional>
#include <imgui.h>
#include <string>
#include <nlohmann/json.hpp>
//
// Created by vastrakai on 6/30/2024.
//

enum class SettingType
{
    Bool,
    Number,
    Enum,
    Color,
    String,
};

class Setting
{
public:
    virtual ~Setting() = default;

    std::string mName;
    std::string mDescription;
    SettingType mType;
    bool mDisplay = false;
    bool* mHideOnClickGUI = nullptr;
    std::function<bool()> mIsVisible = []() { return true; };

    Setting(std::string name, std::string description, SettingType type)
        : mName(std::move(name)), mDescription(std::move(description)), mType(type)
    {
    }

    //had to do dis sorry :/
    float sliderEase = 0;
    float boolScale = 0;
    bool isDragging = false;

    bool enumExtended = false;
    float enumSlide = 0;

    virtual nlohmann::json serialize()
    {
        nlohmann::json j;
        j["name"] = mName;
        j["description"] = mDescription;
        j["type"] = static_cast<int>(mType);
        return j;
    }
};

// Define visible condition
#define VISIBILITY_CONDITION(setting, condition) setting.mIsVisible = std::function<bool()>([&]() { return condition; });

class BoolSetting : public Setting
{
public:
    bool mValue = false;

    BoolSetting(std::string name, std::string description, bool value)
        : Setting(std::move(name), std::move(description), SettingType::Bool), mValue(value)
    {

    }

    void setValue(bool value)
    {
        mValue = value;
    }

    nlohmann::json serialize() override
    {
        nlohmann::json j = Setting::serialize();
        j["boolValue"] = mValue;
        return j;
    }
};

class NumberSetting : public Setting
{
public:
    float mValue = 0.0f;
    float mMin = 0.0f;
    float mMax = 0.0f;
    float mStep = 0.0f;

    NumberSetting(std::string name, std::string description, float value, float min, float max, float step)
        : Setting(std::move(name), std::move(description), SettingType::Number), mValue(value), mMin(min), mMax(max), mStep(step)
    {

    }

    void setValue(float value)
    {
        mValue = std::round(value / mStep) * mStep;
    }

    nlohmann::json serialize() override
    {
        nlohmann::json j = Setting::serialize();
        j["numberValue"] = mValue;
        return j;
    }
};

class EnumSetting : public Setting
{
public:
    int mValue = 0;
    std::vector<std::string> mValues;

    EnumSetting(std::string name, std::string description, int index, std::vector<std::string> values)
        : Setting(std::move(name), std::move(description), SettingType::Enum), mValue(index), mValues(std::move(values))
    {

    }

    template <typename IndexType, typename... Args>
    EnumSetting(std::string name, std::string description, IndexType index, Args... values)
        : Setting(std::move(name), std::move(description), SettingType::Enum), mValue(static_cast<int>(index))
    {
        mValues = { values... };
    }

    void setValue(int value)
    {
        mValue = value;
    }

    nlohmann::json serialize() override
    {
        nlohmann::json j = Setting::serialize();
        j["enumValue"] = mValue;
        return j;
    }
};

class ColorSetting : public Setting
{
public:
    // Use ImColor to convert this to a color
    float mValue[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool mIsExtended = false;
    float mSlide = 0;
    // vertical size of the color picker
    static inline float mColorPickerSize = 400;


    ColorSetting(std::string name, std::string description, float r, float g, float b, float a)
        : Setting(std::move(name), std::move(description), SettingType::Color)
    {
        mValue[0] = r;
        mValue[1] = g;
        mValue[2] = b;
        mValue[3] = a;
    }

    ColorSetting(std::string name, std::string description, uint64_t color)
        : Setting(std::move(name), std::move(description), SettingType::Color)
    {
        mValue[0] = ((color >> 16) & 0xFF) / 255.0f;
        mValue[1] = ((color >> 8) & 0xFF) / 255.0f;
        mValue[2] = (color & 0xFF) / 255.0f;
        mValue[3] = ((color >> 24) & 0xFF) / 255.0f;
    }



    void setValue(float r, float g, float b, float a)
    {
        mValue[0] = r;
        mValue[1] = g;
        mValue[2] = b;
        mValue[3] = a;
    }

    ImColor getAsImColor()
    {
        return { mValue[0], mValue[1], mValue[2], mValue[3] };
    }

    void setFromImColor(const ImColor& color)
    {
        mValue[0] = color.Value.x;
        mValue[1] = color.Value.y;
        mValue[2] = color.Value.z;
        mValue[3] = color.Value.w;
    }

    void setFromImVec4(const ImVec4& color)
    {
        mValue[0] = color.x;
        mValue[1] = color.y;
        mValue[2] = color.z;
        mValue[3] = color.w;
    }

    ImVec4 getAsImVec4()
    {
        return { mValue[0], mValue[1], mValue[2], mValue[3] };
    }

    void setColor(float r, float g, float b, float a)
    {
        mValue[0] = r;
        mValue[1] = g;
        mValue[2] = b;
        mValue[3] = a;
    }

    nlohmann::json serialize() override
    {
        nlohmann::json j = Setting::serialize();
        j["colorValue"] = {
            mValue[0],
            mValue[1],
            mValue[2],
            mValue[3]
        };
        return j;
    }
};