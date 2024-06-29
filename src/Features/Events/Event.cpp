//
// Created by vastrakai on 6/29/2024.
//

#include "Event.hpp"

bool CancellableEvent::isCancelled() const
{
    return mCancelled;
}

void CancellableEvent::setCancelled(bool cancelled)
{
    mCancelled = cancelled;
}

void CancellableEvent::cancel()
{
    setCancelled(true);
}
