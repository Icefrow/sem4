#include "pch.h"
#include "ScrollableView.h"

ScrollableView::ScrollableView()
{
    addChildInternal(&content_);
    addChildInternal(&vScroll_);
    setSize({100, 100});

    vScroll_.eventPositionChanged = [this](float newPosition) {
        content_.setPosition({0, round(-newPosition)});
    };
}

void ScrollableView::setVScrollVisible(bool visible)
{
    if(visible != vScrollVisible()) {
        if(visible)
            addChildInternal(&vScroll_);
        else
            removeChildInternal(&vScroll_);

        onResized();
    }
}

void ScrollableView::setDropTarget(DropTarget* dropTarget)
{
    Control::setDropTarget(dropTarget);
    content_.setDropTarget(dropTarget);
}

void ScrollableView::onResized()
{
    vScroll_.setPosition({right() - vScroll_.width() - 1, 1 });
    vScroll_.setSize({vScroll_.width(), height() - 3});

    float contentWidth = vScroll_.left() - 1;
    float contentHeight = content_.height();

    if(contentHeight < height())
        contentHeight = height();

    content_.setSize({contentWidth, contentHeight});
}

void ScrollableView::onContentResized()
{
    if(content_.height() <= height())
        content_.setPosition({0, 0});

    ScrollInfo scrollInfo = { content_.height(), height(), -content_.top()};
    vScroll_.setScrollInfo(scrollInfo);
}

void ScrollableView::MyPanel::onResized()
{
    static_cast<ScrollableView *>(parent())->onContentResized();
}

void ScrollableView::MyPanel::drawBackground(Canvas & canvas, const sf::Time & elapsedTime)
{
    static_cast<ScrollableView *>(parent())->drawBackground(canvas, elapsedTime);
}

void ScrollableView::draw(Canvas & canvas, const sf::Time & elapsedTime)
{
    {
        Canvas c = canvas;
        c.transform().translate(content_.position());
        content_.draw(c, elapsedTime);
    }

    if(vScrollVisible()) {
        Canvas c = canvas;
        c.transform().translate(vScroll_.position());
        vScroll_.draw(c, elapsedTime);
    }
}

void ScrollableView::drawBackground(Canvas & canvas, const sf::Time &)
{}
