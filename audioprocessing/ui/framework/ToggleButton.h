#pragma once

#include <functional>
#include <ui/framework/ButtonBase.h>

class ToggleButton : public ButtonBase
{
    typedef ButtonBase base;

public:
    explicit ToggleButton(ButtonReaction reaction = ButtonReaction::ByMousePress);

    bool state() const {
        return state_;
    }

    std::function<void(bool state)> eventStateChanged;

protected:
    void onClick() override;

private:
    bool state_ = false;
};

