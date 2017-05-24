#pragma once

#include "FxRack.h"

#include <ui/framework/Button.h>

class RightPanel : public TransparentPanel
{
public:
    Button btnOpen_;
    Button btnSave_;
    Button btnSettings_;
    FxRack fxRack_;

    RightPanel();

protected:
    void onResized() override;

    void drawBackground(Canvas & canvas, const sf::Time &) override;

private:
    void initControls();

    class Toolbox : public Panel
    {
    protected:
        void drawBackground(Canvas & canvas, const sf::Time & elapsedTime) override;
    };

    Toolbox toolbox_;

};

