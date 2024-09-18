//
// Created by Tozic on 9/17/2024.
//

class AutoSpellBook : public ModuleBase<AutoSpellBook>
{
public:
    BoolSetting mUseHealthSpell = BoolSetting("Use Health Spell", "uses health spell when ur hearts low then 6", true);
    BoolSetting mUseSpeedSpell = BoolSetting("Use Health Spell", "uses speed spell when u toggling speed module", true);
    BoolSetting mUseFireTrailSpell = BoolSetting("Use FireTrail Spell", "uses fire trail spell when aura has target", true);
    BoolSetting mShowNotification = BoolSetting("Show Notification", "Shows a notification when a spellbook is used", true);

    AutoSpellBook() : ModuleBase("AutoSpellBook", "Automatically uses your spells", ModuleCategory::Player, 0, false) {
        addSetting(&mUseHealthSpell);
        addSetting(&mUseSpeedSpell);
        addSetting(&mUseFireTrailSpell);
        addSetting(&mShowNotification);

        mNames = {
            {Lowercase, "autospellbook"},
            {LowercaseSpaced, "auto spell book"},
            {Normal, "AutoSpellBook"},
            {NormalSpaced, "Auto Spell Book"},
        };
    };

    int mHealthSpellSlot = -1;
    int mSpeedSpellSlot = -1;
    int mFireTrailSpellSlot = -1;

    int getHealthSpell();
    int getSpeedSpell();
    int getFireTrailSpell();
    void useSpell(int slot);
    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};
