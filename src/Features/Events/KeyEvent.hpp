//
// Created by vastrakai on 7/1/2024.
//

#pragma once

#include "Event.hpp"

class KeyEvent : public CancellableEvent {
public:
    int mKey;
    bool mPressed;

    explicit KeyEvent(int key, bool pressed) : mKey(key), mPressed(pressed) {}
};