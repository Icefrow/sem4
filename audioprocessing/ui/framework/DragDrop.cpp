#include "pch.h"

#include "DragDrop.h"
#include "Control.h"


std::ostream & operator<<(std::ostream & os, DropEffect value)
{
    if((int(value) & int(DropEffect::All)) != int(value)) {
        os << "#invalid value";
        return os;
    }

    switch(value) {
    case DropEffect::None:
        return os << "None";

    case DropEffect::Copy:
        return os << "Copy";

    case DropEffect::Move:
        return os << "Move";

    case DropEffect::Link:
        return os << "Link";
    }

    os << "[";
    bool firstItem = true;

    if(bool(int(value) & int(DropEffect::Copy))) {
        os << "Copy";
        firstItem = false;
    }

    if(bool(value & DropEffect::Move)) {
        if(!firstItem)
            os << " ,";

        os << "Move";
        firstItem = false;
    }

    if(bool(value & DropEffect::Link)) {
        if(!firstItem)
            os << " ,";

        os << "Link";
    }

    return os << "]";
}

DragData DragSource::beginDrag(Control *, PointerEventArgs &)
{
    DragData result = { DropEffect::None };
    return result;
}

void DragSource::endDrag(DropEffect dropEffect)
{}

DropEffect DropTarget::drop(Control *, const DragData &, PointerEventArgs &)
{
    return DropEffect::None;
}

DropEffect DropTarget::hover(Control *, const DragData &, PointerEventArgs &)
{
    return DropEffect::None;
}

DragDrop::DragDrop()
{
}

DropEffect DragDrop::processPointerMove(Control * control,
                                        PointerEventArgs & pointerEventArgs)
{
    assert(isDragging());

    if(DropTarget * dropTarget = control->dropTarget())
        return dropTarget->hover(control, dragData_, pointerEventArgs);

    return DropEffect::None;
}

DragData DragDrop::dragData_ = {DropEffect::None};
DragSource * DragDrop::dragSource_ = nullptr;
Control * DragDrop::draggingControl_ = nullptr;
Point DragDrop::startDragPoint_ = {0, 0};

void DragDrop::beginDrag(DragSource * dragSource, Control * draggingControl,
                         PointerEventArgs & eventArgs)
{
    assert(dragSource);
    assert(draggingControl);

    dragData_ = dragSource->beginDrag(draggingControl, eventArgs);

    if(dragData_.dropEffect != DropEffect::None) {
        dragSource_ = dragSource;
        draggingControl_ = draggingControl;
        startDragPoint_ = eventArgs.position();
    }
}

void DragDrop::drop(Control * dropToControl, PointerEventArgs & eventArgs)
{
    assert(dragSource_);

    DropEffect dropEffect = DropEffect::None;

    if(DropTarget * dropTarget = dropToControl->dropTarget())
        dropEffect = dropTarget->drop(dropToControl, dragData_, eventArgs);

    dragSource_->endDrag(dropEffect);
    dragSource_ = nullptr;
    draggingControl_ = nullptr;
    dragData_ = { DropEffect::None };
}
