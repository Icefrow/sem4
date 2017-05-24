#pragma once

#include <ui/framework/Control.h>

/* -------------------------------------------------------------------- */
class Panel : public Control
{
    typedef Control base;

public:
    Panel();

    void addControl(Control * child);

    template<typename Container>
    void addControls(const Container & children) {
        for(Control * child : children)
            addControl(child);
    }

    void addControls(const std::initializer_list<Control*> & children) {
        typedef std::initializer_list<Control*> Container;
        addControls<Container>(children);
    }

protected:
    virtual void draw(Canvas & canvas, const sf::Time & elapsedTime);

    virtual void drawBackground(Canvas & canvas, const sf::Time & elapsedTime);
};

/* -------------------------------------------------------------------- */
// Does not draw background
class TransparentPanel : public Panel
{
protected:
    void drawBackground(Canvas & canvas, const sf::Time & elapsedTime) override;
};