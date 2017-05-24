#pragma once

#include <ui/framework/Button.h>

struct ScrollInfo {
    float contentSize;
    float pageSize;
    float position;
};

class ScrollBar : public Control
{
public:
    ScrollBar();

    std::function<void(float newPosition)> eventPositionChanged;

    void setScrollInfo(const ScrollInfo & scrollInfo);

    void setScrollPosition(float position);

protected:
    void draw(Canvas & canvas, const sf::Time & elapsedTime) override;

    const ScrollInfo & scrollInfo() const {
        return scrollInfo_;
    }

	void onPointerPressed(PointerEventArgs & e) override;

    void onResized() override;

private:
    void arrangeSlider();

    void shiftPosition(float dy);

    ScrollInfo scrollInfo_;

    class ThumbBase : public Button
    {
    public:
        explicit ThumbBase(ButtonReaction reaction);
        ScrollBar * parent() {
            return static_cast<ScrollBar *>(Control::parent());
        }
        Color foregroundColor() const;
        using Button::draw;
    };

    class UpThumb : public ThumbBase
    {
    public:
        explicit UpThumb();

    protected:
        void drawForeground(Canvas & canvas, const sf::Time &) override;
    } upThumb_;

    class DownThumb : public ThumbBase
    {
    public:
        explicit DownThumb();

    protected:
        void drawForeground(Canvas & canvas, const sf::Time &) override;
    } downThumb_;

    class Slider : public ThumbBase
    {
    public:
        Slider();
    protected:
        void onPointerPressed(PointerEventArgs & e) override;
        void onPointerMoved(PointerEventArgs & e) override;
        void drawForeground(Canvas & canvas, const sf::Time &) override;
    private:
        ScrollBar * parent() {
            return static_cast<ScrollBar *>(Control::parent());
        }
        float lastY_;
    } slider_;

    void drawChild(ThumbBase * child, Canvas & canvas, const sf::Time &);
};

