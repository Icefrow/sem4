#pragma once

#include <util/NonCopyable.h>

#include <ui/framework/Coordinates.h>
#include <ui/framework/VirtualKey.h>

#include <SFML/Window/Event.hpp>

/* -------------------------------------------------------------- */
// Forward declarations
class Control;

/* -------------------------------------------------------------- */
class ControlEventArgs //: NonCopyable
{
public:
    bool handled() const {
        return handled_;
    }

    void notifyHandled() {
        handled_ = true;
    }

private:
    bool handled_ = false;
};

/* -------------------------------------------------------------- */
class PointerEventArgs : public ControlEventArgs
{
public:
    PointerEventArgs(Point position, VirtualKeyModifiers keyModifiers)
        : position_(position), keyModifiers_(keyModifiers) {}

    Point position() const {
        return position_;
    }

    VirtualKeyModifiers keyModifiers() const {
        return keyModifiers_;
    }

private:
    Point position_;
    VirtualKeyModifiers keyModifiers_;

    friend class HostWindow;
};

/* -------------------------------------------------------------- */
class KeyEventArgs : public ControlEventArgs
{
public:
    explicit KeyEventArgs(VirtualKey key)
        : key_(key) {}

    VirtualKey key() const {
        return key_;
    }

private:
    VirtualKey key_;
    //CorePhysicalKeyStatus keyStatus_;
};
