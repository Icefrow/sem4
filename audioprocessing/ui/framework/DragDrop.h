#pragma once

#include "util/NonCopyable.h"
#include "ControlEvents.h"

// Forward declarations
class Control;
class PointerEventArgs;

/* ----------------------------------------------- */
enum class DropEffect : unsigned {
    None = 0x0,
    Copy = 0x1,
    Move = 0x2,
    Link = 0x4,
    All = Copy | Move | Link
};

inline
DropEffect operator&(DropEffect l, DropEffect r)
{
    typedef std::underlying_type<DropEffect>::type ut;
    return DropEffect(ut(l) & ut(r));
}

inline
DropEffect operator|(DropEffect l, DropEffect r)
{
    typedef std::underlying_type<DropEffect>::type ut;
    return DropEffect(ut(l) | ut(r));
}

std::ostream & operator<<(std::ostream & os, DropEffect value);

/* ----------------------------------------------- */
struct DragData {
    DropEffect dropEffect;
    std::string format;
    void * data;
};

/* ----------------------------------------------- */
class DragSource : NonCopyable
{
public:
    virtual ~DragSource() = default;

    bool canDrag() const {
        return canDrag_;
    }

protected:
    virtual DragData beginDrag(Control * draggingControl, PointerEventArgs & e);

    virtual void endDrag(DropEffect dropEffect);

    void setCanDrag(bool value) {
        canDrag_ = value;
    }

private:
    bool canDrag_ = true;

    friend class DragDrop;
};

/* ----------------------------------------------- */
class DropTarget
{
public:
    virtual ~DropTarget() = default;

protected:
    virtual DropEffect drop(Control * dropToControl, const DragData & dd,
                            PointerEventArgs & e);

    virtual DropEffect hover(Control * dropToControl, const DragData & dd,
                             PointerEventArgs & e);

    friend class DragDrop;
};

/* ----------------------------------------------- */
class DragDrop
{
public:
    DragDrop();

    static DropEffect processPointerMove(Control * control,
                                         PointerEventArgs & pointerEventArgs);

    static bool isDragging() {
        return dragSource_ != nullptr;
    }

    static const Control * draggingControl() {
        return draggingControl_;
    }

    static void beginDrag(DragSource * dragSource, Control * draggingControl,
                          PointerEventArgs & eventArgs);

    static void drop(Control * dropToControl, PointerEventArgs & eventArgs);

private:
    static DragData dragData_;
    static DragSource * dragSource_;
    static Control * draggingControl_;
    static Point startDragPoint_;
};
