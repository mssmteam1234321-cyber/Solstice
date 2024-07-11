#pragma once
//
// Created by vastrakai on 6/29/2024.
//


class Event {
public:
    explicit Event() = default;
};

class CancelableEvent : public Event {
public:
    bool mCancelled = false;

    explicit CancelableEvent(bool cancelled = false) : mCancelled(cancelled) {}

    [[nodiscard]] bool isCancelled() const;
    void setCancelled(bool cancelled);
    void cancel();
};