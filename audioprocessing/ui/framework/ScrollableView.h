#pragma once

#include <ui/framework/Panel.h>
#include <ui/framework/ScrollBar.h>

class ScrollableView : public Control
{
public:
    ScrollableView();

    Panel & content() {
        return content_;
    }

    bool vScrollVisible() const {
        return vScroll_.parent() != nullptr;
    }

    void setVScrollVisible(bool visible);

protected:
    void setDropTarget(DropTarget * dropTarget) override;

    void onResized() override;

    void onContentResized();

    void draw(Canvas & canvas, const sf::Time & elapsedTime) override;

    virtual void drawBackground(Canvas & canvas, const sf::Time &);

private:
    class MyPanel : public Panel
    {
    public:
        using Panel::draw;
    protected:
        void onResized() override;
        void drawBackground(Canvas & canvas, const sf::Time &);
    };

    class VScroll : public ScrollBar
    {
    public:
        using ScrollBar::draw;
    };

    MyPanel content_;
    VScroll vScroll_;
};

