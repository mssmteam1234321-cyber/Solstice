//
// Created by vastrakai on 6/29/2024.
//

#include "Event.hpp"

bool CancelableEvent::isCancelled() const
{
    return mCancelled;
}

void CancelableEvent::setCancelled(bool cancelled)
{
    mCancelled = cancelled;
}

void CancelableEvent::cancel()
{
    setCancelled(true);
}
