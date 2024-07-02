//
// Created by vastrakai on 7/1/2024.
//

#include "Interface.hpp"

void Interface::onEnable()
{

}

void Interface::onDisable()
{

}

void Interface::onModuleStateChange(ModuleStateChangeEvent& event)
{
    if (event.mModule == this)
    {
        event.setCancelled(true);
    }
}
