#pragma once
//
// Created by vastrakai on 7/6/2024.
//

class Container {
public:
    virtual ~Container() = 0;
    class ItemStack* getItem(int slot);
};

class SimpleContainer : public Container {};