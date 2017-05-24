#pragma once

#include <ui/framework/VirtualKey.h>
#include <ui/framework/Cursor.h>
#include <ui/framework/Coordinates.h>
#include <ui/framework/DragDrop.h>

#include <SFML/Graphics/RenderWindow.hpp>
//#include <memory>

class PointerEventArgs;
class Control;

class HostWindow : public sf::RenderWindow
{
    typedef RenderWindow base;

public:
    // creates top-level window
    HostWindow(const std::string & title, Control * content,
               const sf::ContextSettings & settings = sf::ContextSettings());

    ~HostWindow();

    Control * content() {
        return content_;
    }

    void run();

    // Starts the capture of pointer input data
    void setPointerCapture(Control * ctl);

    void releasePointerCapture(Control * ctl);

    Control * getPointerCapture() const {
        return pointerCaptureCtl_;
    }

    /// Gets the current status of a virtual keypress.
    VirtualKeyStates getCurrentKeyState(VirtualKey virtualKey);

    Cursor pointerCursor() const {
        return pointerCursor_;
    }

    void setPointerCursor(Cursor cursor);

protected:
    void onResize() override;

    sf::Vector2u getSize() const override;

private:
    void redraw();
    void storeCurrentSize();
    void processPointerMove(PointerEventArgs & e);

    Control * activeControl(Point & pt);

    void processPointerPressed(VirtualKeyModifiers vk, LPARAM lParam);
    void processPointerReleased(VirtualKeyModifiers vk, LPARAM lParam);

    void processPointerDragMove(Control* hoverControl, PointerEventArgs& e);

    void setHoverControl(Control* hoverControl);

    static LRESULT CALLBACK wndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
    WNDPROC baseWndProc_;

    UIntSize size_;
    UIntSize minSize_;
    bool needRedraw_ = true;
    Control * content_;
    Control * pointerHoverCtl_ = nullptr;
    Control * pointerCaptureCtl_ = nullptr;
    Control * lastLClickControl_ = nullptr;

    Control * dragCandidate_ = nullptr;
    Point dragClickCoords_;

    std::uint64_t lastClickTime_ = 0; // in ms
    HCURSOR cursor_ = nullptr;
    Cursor pointerCursor_ = Cursor(-1);
    bool mouseInside_ = false;
    sf::Clock clock_;

    friend class Control;
};
