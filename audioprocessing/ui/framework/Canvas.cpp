#include "pch.h"

#include <ui/framework/Canvas.h>
#include <ui/framework/ResourceManager.h>

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/RenderTarget.hpp>


Canvas::Canvas(sf::RenderTarget * renderTarget,
               const sf::RenderStates & states)
    : renderTarget_(renderTarget)
    , states_(states)
{
}

Canvas::~Canvas()
{
}

void Canvas::draw(const sf::Drawable & drawable)
{
    states_.transform.translate(-0.5f, -0.5f);
    renderTarget_->draw(drawable, states_);
    states_.transform.translate(0.5f, 0.5f);
}

void Canvas::draw(const Icon & icon, const Rect & r)
{
    fill(r, icon.texture(), icon.textureRect());
}

void Canvas::line(Point a, Point b, Color colorA, Color colorB)
{
    a = transform().transformPoint(a);
    b = transform().transformPoint(b);

    sf::Vertex line[] = {{a, colorA}, {b, colorB}};
    renderTarget_->draw(line, 2, sf::Lines, states_);
}

void Canvas::rect(Rect r, Color color)
{
    //r = r - Size{ 1, 1 };

    sf::Vertex vertices[] = {
        {r.topLeft(), color},
        {r.topRight(), color},
        {r.bottomRight(), color},
        {r.bottomLeft(), color},
        {r.topLeft(), color}
    };

    renderTarget_->draw(vertices, 5, sf::LineStrip, states_);
}

void Canvas::rect(Rect r, Color topLeftColor, Color topRightColor,
                  Color bottomRightColor, Color bottomLeftColor)
{
    //r = r - Size{ 1, 1 };

    sf::Vertex vertices[] = {
        { r.topLeft(), topLeftColor },
        { r.topRight(), topRightColor },
        { r.bottomRight(), bottomRightColor },
        { r.bottomLeft(), bottomLeftColor },
        { r.topLeft(), topLeftColor }
    };

    renderTarget_->draw(vertices, 5, sf::LineStrip, states_);
}

void Canvas::gradient(Rect area,
                      Color topLeftColor, Color topRightColor,
                      Color bottomRightColor, Color bottomLeftColor)
{
    sf::Vertex vertices[] = {
        {area.topLeft(), topLeftColor},
        {area.topRight(), topRightColor},
        {area.bottomLeft(), bottomLeftColor},
        {area.bottomRight(), bottomRightColor}
    };

    renderTarget_->draw(vertices, 4, sf::TriangleStrip, states_);
}

void Canvas::fill(const Rect & area, const Texture * texture)
{
    fill(area, texture, Rect { {0, 0}, area.size()});
}

void Canvas::fill(const Rect & area, const Texture * texture, const Rect & textureRect,
                  Color color)
{
    assert(texture);
    const Texture * savedTexture = states_.texture;
    states_.texture = texture;

    sf::Vertex vertices[] = {
        { area.topLeft(), color, textureRect.topLeft()},
        { area.bottomLeft(), color, textureRect.bottomLeft()},
        { area.topRight(), color, textureRect.topRight()},
        { area.bottomRight(), color, textureRect.bottomRight() }
    };

    renderTarget_->draw(vertices, 4, sf::TriangleStrip, states_);
    states_.texture = savedTexture;
}


void Canvas::fill(const Rect & area, const Texture * texture, Size scale)
{
    float x1 = 1 / scale.width();
    float y1 = 1 / scale.height();

    float txtLeft = 0;
    float txtRight = area.width() * x1;
    float txtTop = 0;
    float txtBottom = area.height() * y1;

    float left = area.left() * x1;
    float right = area.right() * x1;
    float top = area.top() * y1;
    float bottom = area.bottom() * y1;

    sf::Vertex vertices[] = {
        { {left, top}, Color::White, { txtLeft, txtTop } },
        { {left, bottom}, Color::White, { txtLeft, txtBottom } },
        { {right, top}, Color::White, { txtRight, txtTop } },
        { {right, bottom}, Color::White, { txtRight, txtBottom } }
    };

    Canvas tmp(*this);
    tmp.states_.transform.scale(scale.width(), scale.height());
    tmp.states_.texture = texture;
    tmp.renderTarget_->draw(vertices, 4, sf::TriangleStrip, tmp.states_);
}

namespace {
Texture * s_circle200;

Texture * getCircle200px()
{
    if(Texture * t = s_circle200)
        return t;

    s_circle200 = ResourceManager::getTexture("~Circle200px.png");
    return s_circle200;
}
}   // namespace

void Canvas::filledRoundRect(const Rect & area, Size radiusXY, Color color)
{



}

void Canvas::fillEllipse(const Rect & area, Color color)
{
    fill(area, getCircle200px(), Rect(Point(0, 0), Size(200, 200)), color);
}

void Canvas::horizontalGradient(Rect area, float middlePos,
                                Color leftColor, Color middleColor, Color rightColor)
{
    float absMiddlePos = area.left() + area.width() * middlePos;

    sf::Vertex vertices[] = {
        {area.topLeft(), leftColor},
        {area.bottomLeft(), leftColor},
        {{absMiddlePos, area.top()}, middleColor},
        {{absMiddlePos, area.bottom()}, middleColor},
        {area.topRight(), rightColor},
        {area.bottomRight(), rightColor}
    };

    renderTarget_->draw(vertices, 6, sf::TriangleStrip, states_);
}

void Canvas::verticalGradient(Rect area, float middlePos,
                              Color topColor, Color middleColor, Color bottomColor)
{
    float absMiddlePos = area.top() + area.height() * middlePos;

    sf::Vertex vertices[] = {
        {area.topLeft(), topColor},
        {area.topRight(), topColor},
        {{area.left(), absMiddlePos}, middleColor},
        {{area.right(), absMiddlePos}, middleColor},
        {area.bottomLeft(), bottomColor},
        {area.bottomRight(), bottomColor}
    };

    renderTarget_->draw(vertices, 6, sf::TriangleStrip, states_);
}

void Canvas::draw3dPanel(Rect r, Color keyColor)
{
    gradient(r, keyColor * 1.15f, keyColor, keyColor * 0.75f, keyColor * 0.85f);
    rect(r, keyColor * 0.5f, keyColor * 0.5f, keyColor * 0.33f, keyColor * .33f);

    r = r - Size(2, 2);
    rect(r, keyColor * 1.75f, keyColor * 1.33f, keyColor * 1.33f, keyColor * 1.33f);

    //r = r - Size(2, 2);
    //rect(r, keyColor * 1.33f, keyColor * 1.0f, keyColor * 0.9f, keyColor * 1.0f);
    return;

    Rect internalRect = r - Size{1, 1};
    gradient(internalRect, keyColor * 1.15f, keyColor, keyColor * 0.75f, keyColor * 0.85f);

    line(r.topLeft(), { r.right() - 1, r.top() }, keyColor * 1.5f, keyColor * 1.33f);
    line({ r.left() + 1, r.top() + 1 }, { r.right() - 2, r.top() + 1 }, keyColor * 1.75f);

    line(r.topRight(), r.bottomRight(), keyColor * .75f, keyColor * .5f);
    line({ r.right() - 2, r.top() + 2 }, { r.right() - 2, r.bottom() - 1}, keyColor * 1.25f,
         keyColor * .9f);

    line(r.bottomRight(), r.bottomLeft(), keyColor * .65f, keyColor * 0.753f);
    line({ r.right() - 3, r.bottom() - 1 }, { r.left() + 1, r.bottom() - 1 }, keyColor * 1.1f,
         keyColor * 1.33f);

    line(r.bottomLeft(), r.topLeft(), keyColor * 1.33f);
    line({ r.left() + 1, r.bottom() - 1 }, { r.left() + 1, r.top() + 2 }, keyColor * 1.5f,
         keyColor * 1.65f);
}
