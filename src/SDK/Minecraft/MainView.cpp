//
// Created by vastrakai on 6/24/2024.
//

#include "MainView.hpp"

class BedrockPlatformUWP* MainView::getBedrockPlatform()
{
    return hat::member_at<BedrockPlatformUWP*>(this, 0x118);
}

MainView* MainView::getInstance()
{
    static auto mainView = reinterpret_cast<MainView**>(SigManager::MainView_instance);
    return *mainView;
}
