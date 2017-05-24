#pragma once

#include <ui/framework/Control.h>

class Splitter : public Control
{
public:
    Splitter();

protected:
    void draw(Canvas & canvas, const sf::Time & elapsedTime) override;


};

