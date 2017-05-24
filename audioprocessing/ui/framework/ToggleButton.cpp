#include "pch.h"
#include "ToggleButton.h"

ToggleButton::ToggleButton(ButtonReaction reaction)
    : base(reaction){}

void ToggleButton::onClick()
{
    state_ = !state_;

    if (eventStateChanged)
        eventStateChanged(state_);
}
