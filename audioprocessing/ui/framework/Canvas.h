#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
//#include <SFML/Graphics/Drawable.hpp>
//#include <SFML/Graphics/Vertex.hpp>

#include <util/NonCopyable.h>
#include <ui/framework/Coordinates.h>
#include <ui/framework/Color.h>
#include <ui/framework/Icon.h>
#include <SFML/Graphics/PrimitiveType.hpp>

class Canvas
{
public:
    Canvas(sf::RenderTarget * renderTarget,
           const sf::RenderStates & states = sf::RenderStates());

    ~Canvas();

    void draw(const sf::Drawable & drawable);

    void draw(const Icon & icon, const Rect & r);

    void line(Point a, Point b, Color colorA, Color colorB);

    void line(Point a, Point b, Color color) {
        line(a, b, color, color);
    }

    template<size_t N>
    void lines(sf::Vertex(&points)[N]) {
        renderTarget_->draw(points, N, sf::LineStrip, states_);
    }

    template<size_t N>
    void lines(Point(&points)[N], Color color) {
        sf::Vertex vertices[N];

        for(unsigned i = 0; i < N; i++) {
            vertices[i].position = points[i];
            vertices[i].color = color;
        }

        lines(vertices);
    }

    void triangle(Point a, Point b, Point c, Color color) {
        sf::Vertex vertices[4] = {{a, color}, {b, color}, {c, color}, {a, color}};
        lines(vertices);
    }

    void fillTriangle(Point a, Point b, Point c, Color color) {
        sf::Vertex vertices[3] = { { a, color },{ b, color },{ c, color }};
        renderTarget_->draw(vertices, 3, sf::Triangles, states_);
    }

    // non-filled rectanlge
    void rect(Rect r, Color color);

    // non-filled rectanlge
    void rect(Rect r, Color topLeft, Color topRight,
              Color bottomRight, Color bottomLeft);

    // Fill rectangle, each corner has its own color
    void gradient(Rect area, Color topLeft, Color topRight,
                  Color bottomRight, Color bottomLeft);

    void fill(const Rect & area, Color color) {
        gradient(area, color, color, color, color);
    }

    void fill(const Rect & area, const Texture * texture);

    void fill(const Rect & area, const Texture * texture, const Rect & textureRect,
              Color color = Color::White);

    void fill(const Rect & area, const Texture * texture, Size scale);

    void fill(const Rect & area, const Texture * texture, float scale) {
        fill(area, texture, {scale, scale});
    }

    void filledRoundRect(const Rect & area, Size radiusXY, Color color);

    void filledRoundRect(const Rect & area, float radius, Color color) {
        filledRoundRect(area, {radius, radius}, color);
    }

    void fillEllipse(const Rect & area, Color color);

    void verticalGradient(Rect area, Color top, Color bottom) {
        gradient(area, top, top, bottom, bottom);
    }

    void horizontalGradient(Rect area, Color left, Color right) {
        gradient(area, left, right, right, left);
    }

    // middlePos = 0..1
    void horizontalGradient(Rect area, float middlePos, Color left, Color middle, Color right);

    // middlePos = 0..1
    void verticalGradient(Rect area, float middlePos, Color top, Color middle, Color bottom);

    void draw3dPanel(Rect bounds, Color keyColor);

    void setTexture(const Texture * texture) {
        states_.texture = texture;
    }

    sf::Transform & transform() {
        return states_.transform;
    }

private:
    sf::RenderTarget * renderTarget_;
    sf::RenderStates states_;
};

