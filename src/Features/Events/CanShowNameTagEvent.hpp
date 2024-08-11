//
// Created by vastrakai on 8/11/2024.
//

#pragma once

#include "Event.hpp"

class CanShowNameTagEvent : public Event {
public:
    explicit CanShowNameTagEvent(void* a1, class Actor* actor, bool result) : mA1(a1), mActor(actor), mResult(result) {}

    void* mA1;
    class Actor* mActor;
    bool mResult = true;

    [[nodiscard]] bool getResult() const {
        return mResult;
    }

    void setResult(bool result) {
        mResult = result;
    }
};