#include "pch.h"

#include "Splitter.h"
#include "Theme.h"

Splitter::Splitter()
{
    setWidth(10);
    setCursor(Cursor::SizeWestEast);
}

void Splitter::draw(Canvas & canvas, const sf::Time& elapsedTime)
{
    canvas.draw3dPanel(internalBounds(), Theme::current().keyColor()*0.35);
}

