#include "pch.h"

#include <SFML/Graphics/Text.hpp>

#include <ui/framework/Theme.h>
#include <ui/framework/ResourceManager.h>

#include "LeftPanel.h"
#include "FxPanel.h"
#include "MainForm.h"

using namespace std;

namespace {
const float DefaultWidth = 1920;
const float TextSize = 13;
const float TextWidth = 90;
const Rect s_imageRect = { 0, 0, 1780, 242 };
const Rect s_iconRect = s_imageRect - Size {16, 16};
}

class LeftPanel::LibraryItem : public Control
{
public:
    explicit LibraryItem(const EffectDescriptor * descriptor)
        : descriptor_(descriptor)
        , icon_(ResourceManager::getTexture(descriptor->bkTextureName()), s_iconRect) {
        text_.setString(descriptor->effectName());
        text_.setFont(Theme::current().font());
        text_.setCharacterSize(13);
        text_.setOutlineThickness(0.2f);
        text_.setFillColor(mix(Color::White, Theme::current().accentColor()));
    }

    LeftPanel * parent() {
        return static_cast<LeftPanel *>(Control::parent());
    }

    void onDoubleTapped(PointerEventArgs & e) override {
        cout << "Double click: " << descriptor_->effectName() << endl;

        MainForm & mainForm = MainForm::from(hostWindow());
        mainForm.addEffect(descriptor_);
    }

    void draw(Canvas & canvas, const sf::Time &) override {
        Theme & theme = Theme::current();
        Color keyColor = theme.keyColor() * 0.85f;
        Color accentColor = theme.accentColor();

        bool hover = isPointerHover();

        Color bkColor = hover
                        ? mix(keyColor * 0.4, accentColor * 0.75f, 0.91f)
                        : keyColor * 0.4f;

        Rect r = internalBounds();

        canvas.fill(r - Size{1, 1}, bkColor);

        if(hover) {
            Color boundsColor = mix(keyColor * 0.25, accentColor, 0.66f);
            canvas.rect(r, boundsColor);
        }

        float padding = 6;

        float scale = (width() - padding * 2 - TextWidth) / s_imageRect.width();
        r = { padding, padding, round(s_imageRect.width()*scale), round(s_imageRect.height()*scale) };

        text_.setPosition(r.right() + padding * 2, 10);
        canvas.draw(text_);
        canvas.draw(icon_, r);
    }

    void onPointerEntered() override {
        redraw();
    }

    void onPointerExited() override {
        redraw();
    }

    const EffectDescriptor * descriptor_;
    Icon icon_;
    sf::Text text_;
};

LeftPanel::LeftPanel()
{
}

LeftPanel::~LeftPanel()
{
    for(LibraryItem * item : items_)
        delete item;
}

void LeftPanel::addElement(const char * effectName)
{
    if(const EffectDescriptor * descriptor = EffectDescriptor::find(effectName)) {
        LibraryItem * item = new LibraryItem(descriptor);
        items_.push_back(item);
        content().addControl(item);
        item->setDragSource(this);
    }
    else
        cerr << "Can not find effect: " << effectName << endl;
}

void LeftPanel::alignElements()
{
    float contentWidth = content().width();
    float scale = contentWidth / DefaultWidth;

    float horizGap = 1;
    float vertGap = 1;
    float vertPadding = 3;

    float itemWidth = contentWidth - horizGap * 2;
    float itemHeight = round(vertPadding + vertGap + s_iconRect.height() * scale * 1.2f - 10);

    float x = horizGap;
    float y = vertGap;

    for(unsigned i = 0; i < items_.size(); i++) {
        LibraryItem * item = items_[i];
        item->setPosition({ x, y });
        item->setSize({itemWidth, itemHeight});
        y += itemHeight + vertGap;
    }

    if(y < height())
        y = height();

    content().setHeight(y);
    redraw();
}

DragData LeftPanel::beginDrag(Control * draggingControl, PointerEventArgs & e)
{
    LibraryItem * libraryItem = static_cast<LibraryItem *>(draggingControl);
    cout << "Begin drag: " << libraryItem->descriptor_->effectName() << endl;
    DragData result = { DropEffect::Copy, "AddFx", (void *)libraryItem->descriptor_ };
    return result;
}

void LeftPanel::endDrag(DropEffect dropEffect)
{
    cout << "End drag, drop effect: " << dropEffect << endl;
}

void LeftPanel::onResized()
{
    base::onResized();

    if(width() != lastWidth_) {
        lastWidth_ = width();
        alignElements();
    }
}

void LeftPanel::drawBackground(Canvas & canvas, const sf::Time &)
{
}
