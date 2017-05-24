#pragma once

#include "FxPanel.h"
#include <ui/framework/ScrollableView.h>

class FxRack : public ScrollableView, public DropTarget
{
    typedef ScrollableView base;
public:
    FxRack();

    void addEffect(const char * effectName);
    void addEffect(const EffectDescriptor * effectInfo);

protected:
    virtual DropEffect drop(Control * dropToControl, const DragData & dd,
        PointerEventArgs & e);

    virtual DropEffect hover(Control * dropToControl, const DragData & dd,
        PointerEventArgs & e);

    void drawBackground(Canvas & canvas, const sf::Time &) override;
    void onResized() override;

private:
    void alignEffects();

    Texture * rackTexture_;
    float rackTextureWidth_;
    float lastWidth_ = 0;

    class FxFrame;
    std::vector<FxFrame *> effects_;
};

