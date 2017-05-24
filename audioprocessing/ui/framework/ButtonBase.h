#pragma once
#include "Control.h"

enum class ButtonReaction {
    ByMousePress,
    ByMouseRelease
};

class ButtonBase : public Control
{
public:
    explicit ButtonBase(ButtonReaction reaction = ButtonReaction::ByMouseRelease);

protected:
    void onPointerPressed(PointerEventArgs & e) override;
    void onPointerMoved(PointerEventArgs & e) override;
    void onPointerExited() override;
    void onPointerReleased(PointerEventArgs & e) override;
    void onPointerCaptureLost() override;

    virtual void onClick() = 0;

    bool pressed() const {
        return pressed_;
    }

    bool pointerHover() const {
        return pointerHover_;
    }

private:
    ButtonReaction reaction_;
    bool pressed_;
    bool pointerHover_;
};

