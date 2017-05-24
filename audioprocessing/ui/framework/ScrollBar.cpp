#include "pch.h"

#include "ScrollBar.h"
#include "Theme.h"
#include "Environment.h"

ScrollBar::ScrollBar()
    : scrollInfo_{1, 1, 0}
{
    this->addChildInternal(&upThumb_);
    this->addChildInternal(&slider_);
    this->addChildInternal(&downThumb_);

    upThumb_.setPosition({ 0, 0 });
    setWidth(float(Environment::scrollBarWidth()));

    upThumb_.eventClick = [this] {
        shiftPosition(-1);
    };

    downThumb_.eventClick = [this] {
        shiftPosition(1);
    };
}

void ScrollBar::draw(Canvas & canvas, const sf::Time & elapsed)
{
    Color color = Theme::current().keyColor() * 0.35f;

    Rect r = internalBounds();
    canvas.rect(r, color);

    r = r - Size{ 2, 2 };
    canvas.fill(r, color * 0.5f);

    drawChild(&upThumb_, canvas, elapsed);
    drawChild(&slider_, canvas, elapsed);
    drawChild(&downThumb_, canvas, elapsed);
}

void ScrollBar::setScrollInfo(const ScrollInfo & si)
{
    assert(si.pageSize > 0);

    if(si.position <= si.contentSize - si.pageSize)
        assert(si.position <= si.contentSize - si.pageSize);

    scrollInfo_ = si;
    arrangeSlider();
    redraw();
}

void ScrollBar::setScrollPosition(float position)
{
    assert(position >= 0);
    assert(position <= scrollInfo_.contentSize - scrollInfo_.pageSize);

    scrollInfo_.position = position;
}

void ScrollBar::onPointerPressed(PointerEventArgs & e)
{
	if (e.position().y >= slider_.top())
		shiftPosition(scrollInfo_.pageSize);
	else 
		shiftPosition(-scrollInfo_.pageSize);
}

void ScrollBar::onResized()
{
    downThumb_.setPosition({ 0, height() - downThumb_.height() });
    arrangeSlider();
}

void ScrollBar::arrangeSlider()
{
    float space = height() - upThumb_.height() - downThumb_.height() - 2;
    float h = round(space * scrollInfo_.pageSize / scrollInfo_.contentSize);

    if(h < upThumb_.height())
        h = upThumb_.height();

    float y = round(space * scrollInfo_.position / scrollInfo_.contentSize);

    slider_.setPosition({ 0, upThumb_.height() + 1 + y});
    slider_.setSize({ width(), h});
}

void ScrollBar::shiftPosition(float dy)
{
    float newPosition = scrollInfo_.position + dy;

    if(newPosition < 0)
        newPosition = 0;

    if(newPosition > scrollInfo_.contentSize - scrollInfo_.pageSize)
        newPosition = scrollInfo_.contentSize - scrollInfo_.pageSize;

    if(newPosition != scrollInfo_.position) {
        scrollInfo_.position = newPosition;
        arrangeSlider();
        redraw();

        if(eventPositionChanged)
            eventPositionChanged(newPosition);
    }
}

void ScrollBar::drawChild(ThumbBase * child, Canvas & canvas, const sf::Time & elapsed)
{
    Canvas c = canvas;
    c.transform().translate(child->position());
    child->draw(c, elapsed);
}

ScrollBar::ThumbBase::ThumbBase(ButtonReaction reaction)
    : Button("", reaction)
{
    float x = float(Environment::scrollBarWidth());
    setSize({ x, x });
}

Color ScrollBar::ThumbBase::foregroundColor() const
{
    Color keyColor = Theme::current().keyColor();
    Color accentColor = Theme::current().accentColor();

    return isPointerHover()
           ? mix(keyColor * 0.25, accentColor, 0.27f)
           : mix(keyColor * 0.25, accentColor, 0.42f);
}

ScrollBar::UpThumb::UpThumb()
    : ThumbBase(ButtonReaction::ByMousePress) {}

void ScrollBar::UpThumb::drawForeground(Canvas & canvas, const sf::Time &)
{
    Rect r = internalBounds() - Size{ 9, 12 };

    canvas.fillTriangle(r.bottomLeft(),
                        r.bottomRight(),
                        (r.topLeft() + r.topRight()) * 0.5f,
                        foregroundColor());

    if(pressed())
        parent()->shiftPosition(-1);
}

ScrollBar::DownThumb::DownThumb()
    : ThumbBase(ButtonReaction::ByMousePress) {}

void ScrollBar::DownThumb::drawForeground(Canvas & canvas, const sf::Time &)
{
    Rect r = internalBounds() - Size{9, 12};

    canvas.fillTriangle(r.topLeft(),
                        r.topRight(),
                        (r.bottomLeft() + r.bottomRight()) * 0.5f,
                        foregroundColor());

    if(pressed()) {
        Sleep(3);
        parent()->shiftPosition(1);
    }
}

ScrollBar::Slider::Slider()
    : ThumbBase(ButtonReaction::ByMouseRelease)
{
    float x = float(Environment::scrollBarWidth());
    setWidth(x);
}

void ScrollBar::Slider::onPointerPressed(PointerEventArgs & e)
{
    ThumbBase::onPointerPressed(e);

    if(pressed())
        lastY_ = e.position().y;
}

void ScrollBar::Slider::onPointerMoved(PointerEventArgs & e)
{
    ThumbBase::onPointerMoved(e);

    if(pressed()) {
        float dy = e.position().y - lastY_;
        parent()->shiftPosition(dy);
    }
}

void ScrollBar::Slider::drawForeground(Canvas & canvas, const sf::Time &)
{

}

