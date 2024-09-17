//
// Created by Tozic on 9/17/2024.
//

class AutoSpellBook : public ModuleBase<AutoSpellBook>
{
public:
    NumberSetting mDelay = NumberSetting("Delay", "The delay between using spellbooks (in milliseconds)", 200, 0, 2000, 1);
    BoolSetting mShowNotification = BoolSetting("Show Notification", "Shows a notification when a spellbook is used", true);

    AutoSpellBook() : ModuleBase("AutoSpellBook", "Automatically opens/uses your spellbook", ModuleCategory::Player, 0, false) {
        addSetting(&mDelay);
        addSetting(&mShowNotification);

        mNames = {
            {Lowercase, "autospellbook"},
            {LowercaseSpaced, "auto spell book"},
            {Normal, "AutoSpellBook"},
            {NormalSpaced, "Auto Spell Book"},
        };
    };

    uint64_t mLastItemUsed;

    void onEnable() override;
    void onDisable() override;
    void onBaseTickEvent(class BaseTickEvent& event);
};
