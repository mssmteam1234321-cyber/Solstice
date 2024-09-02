#pragma once
#include <Features/Events/ActorRenderEvent.hpp>
#include <Features/Events/BaseTickEvent.hpp>
#include <Features/Events/ModuleStateChangeEvent.hpp>
#include <Features/Events/DrawImageEvent.hpp>
//
// Created by vastrakai on 7/1/2024.
//


class Interface : public ModuleBase<Interface> {
public:
    enum ColorTheme {
        Trans,
        Rainbow,
        Custom
    };

    enum class FontType {
        Mojangles,
        /*ProductSans,
        Comfortaa,*/
        SFProDisplay
    };

    EnumSettingT<NamingStyle> mNamingStyle = EnumSettingT<NamingStyle>("Naming", "The style of the module names.", NamingStyle::Normal, "lowercase", "lower spaced", "Normal", "Spaced");
    EnumSettingT<ColorTheme> mMode = EnumSettingT<ColorTheme>("Theme", "The mode of the interface.", Custom, "Trans", "Rainbow", "Custom");
    // make sure you actually have the fonts u put here lol
    EnumSettingT<FontType> mFont = EnumSettingT<FontType>("Font", "The font of the interface.", FontType::Mojangles, "Mojangles" /*"Product Sans", "Comfortaa"*/, "SF Pro Display");
    NumberSetting mColors = NumberSetting("Colors", "The amount of colors in the interface.", 3, 1, 6, 1);
    // Colors will be Fire-themed by default
    ColorSetting mColor1 = ColorSetting("Color 1", "The first color of the interface.", 0xFFFF0000);
    ColorSetting mColor2 = ColorSetting("Color 2", "The second color of the interface.", 0xFFFF7F00);
    ColorSetting mColor3 = ColorSetting("Color 3", "The third color of the interface.", 0xFFFFD600);
    ColorSetting mColor4 = ColorSetting("Color 4", "The fourth color of the interface.", 0xFF00FF00);
    ColorSetting mColor5 = ColorSetting("Color 5", "The fifth color of the interface.", 0xFF0000FF);
    ColorSetting mColor6 = ColorSetting("Color 6", "The sixth color of the interface.", 0xFF8B00FF);
    NumberSetting mColorSpeed = NumberSetting("Color Speed", "The speed of the color change.", 3.f, 0.01f, 20.f, 0.01);
    NumberSetting mSaturation = NumberSetting("Saturation", "The saturation of the interface.", 1.f, 0.f, 1.f, 0.01);
    BoolSetting mSlotEasing = BoolSetting("Slot Easing", "Eases the selection of slots", true);
    NumberSetting mSlotEasingSpeed = NumberSetting("Easing Speed", "The speed of the slot easing", 20.f, 0.1f, 20.f, 0.01f);
    //BoolSetting mShowRotations = BoolSetting("Show Rotations", "Shows normally invisible server-sided rotations", false);


    Interface() : ModuleBase("Interface", "Customize the visuals!", ModuleCategory::Visual, 0, true) {
        gFeatureManager->mDispatcher->listen<ModuleStateChangeEvent, &Interface::onModuleStateChange, nes::event_priority::FIRST>(this);
        gFeatureManager->mDispatcher->listen<RenderEvent, &Interface::onRenderEvent, nes::event_priority::NORMAL>(this);
        gFeatureManager->mDispatcher->listen<ActorRenderEvent, &Interface::onActorRenderEvent, nes::event_priority::NORMAL>(this);
        gFeatureManager->mDispatcher->listen<BaseTickEvent, &Interface::onBaseTickEvent>(this);
        gFeatureManager->mDispatcher->listen<PacketOutEvent, &Interface::onPacketOutEvent, nes::event_priority::ABSOLUTE_LAST>(this);
        gFeatureManager->mDispatcher->listen<DrawImageEvent, &Interface::onDrawImageEvent>(this);

        addSettings(&mNamingStyle, &mMode, &mFont, &mColors, &mColor1, &mColor2, &mColor3, &mColor4, &mColor5, &mColor6, &mColorSpeed, &mSaturation, &mSlotEasing, &mSlotEasingSpeed);

        VISIBILITY_CONDITION(mColors, mMode.mValue == Custom);
        VISIBILITY_CONDITION(mColor1, mMode.mValue == Custom && mColors.mValue >= 1);
        VISIBILITY_CONDITION(mColor2, mMode.mValue == Custom && mColors.mValue >= 2);
        VISIBILITY_CONDITION(mColor3, mMode.mValue == Custom && mColors.mValue >= 3);
        VISIBILITY_CONDITION(mColor4, mMode.mValue == Custom && mColors.mValue >= 4);
        VISIBILITY_CONDITION(mColor5, mMode.mValue == Custom && mColors.mValue >= 5);
        VISIBILITY_CONDITION(mColor6, mMode.mValue == Custom && mColors.mValue >= 6);

        VISIBILITY_CONDITION(mSlotEasingSpeed, mSlotEasing.mValue);

        mNames = {
            {Lowercase, "interface"},
            {LowercaseSpaced, "interface"},
            {Normal, "Interface"},
            {NormalSpaced, "Interface"}
        };
    }

    static inline std::unordered_map<int, std::vector<ImColor>> ColorThemes = {
        {Trans, {
            ImColor(91, 206, 250, 255),
            ImColor(245, 169, 184, 255),
            ImColor(255, 255, 255, 255),
            ImColor(245, 169, 184, 255),
        }},
        {Rainbow, {}},
        {Custom, {}}
    };

    std::vector<ImColor> getCustomColors() {
        auto result = std::vector<ImColor>();
        if (mColors.mValue >= 1) result.push_back(mColor1.getAsImColor());
        if (mColors.mValue >= 2) result.push_back(mColor2.getAsImColor());
        if (mColors.mValue >= 3) result.push_back(mColor3.getAsImColor());
        if (mColors.mValue >= 4) result.push_back(mColor4.getAsImColor());
        if (mColors.mValue >= 5) result.push_back(mColor5.getAsImColor());
        if (mColors.mValue >= 6) result.push_back(mColor6.getAsImColor());
        return result;
    }

    void onEnable() override;
    void onDisable() override;
    void onModuleStateChange(ModuleStateChangeEvent& event);
    void onRenderEvent(class RenderEvent& event);
    void onActorRenderEvent(class ActorRenderEvent& event);
    void onDrawImageEvent(class DrawImageEvent& event);
    void onBaseTickEvent(class BaseTickEvent& event);
    void onPacketOutEvent(class PacketOutEvent& event);
};

class BodyYaw
{
public:
    static inline float bodyYaw = 0.f;
    static inline glm::vec3 posOld = glm::vec3(0, 0, 0);
    static inline glm::vec3 pos = glm::vec3(0, 0, 0);

    static inline void updateRenderAngles(Actor* plr, float headYaw)
    {
        posOld = pos;
        pos = *plr->getPos();

        float diffX = pos.x - posOld.x;
        float diffZ = pos.z - posOld.z;
        float diff = diffX * diffX + diffZ * diffZ;

        float body = bodyYaw;
        if (diff > 0.0025000002F)
        {
            float anglePosDiff = atan2f(diffZ, diffX) * 180.f / 3.14159265358979323846f - 90.f;
            float degrees = abs(wrapAngleTo180_float(headYaw) - anglePosDiff);
            if (95.f < degrees && degrees < 265.f)
            {
                body = anglePosDiff - 180.f;
            }
            else
            {
                body = anglePosDiff;
            }
        }

        turnBody(body, headYaw);
    };

    static inline void turnBody(float bodyRot, float headYaw)
    {
        float amazingDegreeDiff = wrapAngleTo180_float(bodyRot - bodyYaw);
        bodyYaw += amazingDegreeDiff * 0.3f;
        float bodyDiff = wrapAngleTo180_float(headYaw - bodyYaw);
        if (bodyDiff < -75.f)
            bodyDiff = -75.f;

        if (bodyDiff >= 75.f)
            bodyDiff = 75.f;

        bodyYaw = headYaw - bodyDiff;
        if (bodyDiff * bodyDiff > 2500.f)
        {
            bodyYaw += bodyDiff * 0.2f;
        }
    };

    static inline float wrapAngleTo180_float(float value)
    {
        value = fmodf(value, 360.f);

        if (value >= 180.0F)
        {
            value -= 360.0F;
        }

        if (value < -180.0F)
        {
            value += 360.0F;
        }

        return value;
    };
};