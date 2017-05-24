#pragma once

#include <ui/framework/ScrollableView.h>
#include <functional>

class EffectDescriptor;

class LeftPanel : public ScrollableView, public DragSource
{
    typedef ScrollableView base;

public:
    LeftPanel();

    ~LeftPanel();

    void addElement(const char * effectName);

    std::function<void(const EffectDescriptor * descriptor)> eventAddEffect;

protected:
    DragData beginDrag(Control * draggingControl, PointerEventArgs & e) override;
    
    void endDrag(DropEffect dropEffect) override;

    void onResized() override;

    void drawBackground(Canvas & canvas, const sf::Time &);

private:
    void alignElements();

    class LibraryItem;
    std::vector<LibraryItem*> items_;
    float lastWidth_ = 0;
};

