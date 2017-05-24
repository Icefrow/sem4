#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

/* -------------------------------------------------------------- */
// parameterized PointT
template<typename T>
using PointT = sf::Vector2<T>;

typedef PointT<float> Point;

/* -------------------------------------------------------------- */
// parameterized SizeT
template<typename T>
class SizeT
{
public:
    SizeT()
        : width_(0)
        , height_(0) {}

    SizeT(T w, T h)
        : width_(w)
        , height_(h) {}

    template<typename T1>
    SizeT(T1 w, T1 h)
        : width_(T(w))
        , height_(T(h)) {}

    template<typename T1>
    SizeT(const SizeT<T1> & size)
        : width_(T(size.width()))
        , height_(T(size.height())) {}

    template<typename T1>
    SizeT(const PointT<T1> & pt)
        : width_(T(pt.x))
        , height_(T(pt.y)) {}

    // convert to SFML Vector2
    operator sf::Vector2<T>() const {
        return {width_, height_};
    }

    // convert to Windows API POINT
    operator POINT() const {
        return {LONG(width_), LONG(height_)};
    }

    // convert to Windows API SIZE
    operator SIZE() const {
        return{ LONG(width_), LONG(height_) };
    }

    T width() const {
        return width_;
    }

    T height() const {
        return height_;
    }

private:
    T width_;
    T height_;
};

template<typename T>
bool operator==(const SizeT<T> & a, const SizeT<T> & b)
{
    return a.width() == b.width() && a.height() == b.height();
}

template<typename T>
bool operator!=(const SizeT<T> & a, const SizeT<T> & b)
{
    return !(a == b);
}

template<typename T>
SizeT<T> operator+(const SizeT<T> & a, const SizeT<T> & b)
{
    return{ a.width() + b.width(), a.height() + b.height() };
}

template<typename T>
SizeT<T> operator-(const SizeT<T> & a, const SizeT<T> & b)
{
    return{ a.width() - b.width(), a.height() - b.height() };
}

template<typename T>
SizeT<T> operator*(const SizeT<T> & a, T k)
{
    return{ a.width() * k, a.height() * k};
}

typedef SizeT<float> Size;

/* -------------------------------------------------------------- */
// parameterized RectT
template<typename T>
class RectT : public sf::Rect<T>
{
    typedef sf::Rect<T> base;
public:
    RectT() {}

    template<typename T1>
    RectT(const sf::Rect<T1> & other)
        : base(T(other.left), T(other.top), T(other.width), T(other.height)) {}

    RectT(T rectLeft, T rectTop, T rectWidth, T rectHeight)
        : base(rectLeft, rectTop, rectWidth, rectHeight) {}

    RectT(PointT<T> point, SizeT<T> size)
        : base(point.x, point.y, size.width(), size.height()) {}

    Point position() const {
        return {base::left, base::top};
    }

    Size size() const {
        return {base::width, base::height};
    }

    T left() const {
        return base::left;
    }

    T right() const {
        return base::left + base::width;
    }

    T top() const {
        return base::top;
    }

    T bottom() const {
        return base::top + base::height;
    }

    T width() const {
        return base::width;
    }

    T height() const {
        return base::height;
    }

    Point topLeft() const {
        return {base::left, base::top};
    }

    Point bottomLeft() const {
        return {base::left, base::top + base::height};
    }

    Point topRight() const {
        return {base::left + base::width, base::top};
    }

    Point bottomRight() const {
        return {base::left + base::width, base::top + base::height};
    }

    Point center() const {
        return{ base::left + base::width * 0.5f, base::top + base::height * 0.5f };
    }
};

template<typename T>
RectT<T> operator+(const RectT<T> & r, const SizeT<T> & s)
{
    T w = s.width() / 2;
    T h = s.height() / 2;
    return {r.left() - w, r.top() - h, r.width() + s.width(), r.height() + s.height()};
}

template<typename T>
RectT<T> operator-(const RectT<T> & r, const SizeT<T> & s)
{
    T w = s.width() / 2;
    T h = s.height() / 2;
    return{ r.left() + w, r.top() + h, r.width() - s.width(), r.height() - s.height() };
}

template<typename T>
RectT<T> operator*(const RectT<T> & r, const SizeT<T> & s)
{
    return{ r.left() * s.width(), r.top() * s.height(), r.width() * s.width(), r.height() * s.height() };
}

template<typename T>
RectT<T> operator*(const RectT<T> & r, T k)
{
    return r * SizeT<T>(k, k);
}

typedef RectT<float> Rect;

/* -------------------------------------------------------------- */
typedef PointT<int> IntPoint;
typedef SizeT<int> IntSize;
typedef RectT<int> IntRect;

typedef PointT<unsigned> UIntPoint;
typedef SizeT<unsigned> UIntSize;
typedef RectT<unsigned> UIntRect;

