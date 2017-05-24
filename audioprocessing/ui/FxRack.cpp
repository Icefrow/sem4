#include "pch.h"

#include "FxRack.h"
#include <ui/framework/Theme.h>
#include "AudioProcessor.h"

using namespace std;

/* ------------------------------------------------------------------ */
// default designed width of panel, this is base for scaling
constexpr float DefaultWidth = 1920;

/* ------------------------------------------------------------------ */
class FxRack::FxFrame : public Panel
{
    typedef Panel base;

public:
    FxPanel * const fxPanel_;

    explicit FxFrame(FxPanel * fxPanel)
        : fxPanel_(fxPanel) {
        assert(fxPanel);
        addControl(fxPanel);
    }

    void drawBackground(Canvas & canvas, const sf::Time &) override {
        const Color keyColor = Theme::current().keyColor() * 0.35f;
        canvas.draw3dPanel(internalBounds(), keyColor);
    }
};

/* ------------------------------------------------------------------ */
FxRack::FxRack()
{
    rackTexture_ = ResourceManager::getTexture("Rack.bmp");
    //rackTexture_->setSrgb(true);
    rackTexture_->setRepeated(true);
    rackTextureWidth_ = float(rackTexture_->getSize().x);

    addEffect("Output");
    addEffect("Input");
	setDropTarget(this);
    //addEffect("Screamer");
    //addEffect("Flanger");
    //addEffect("Amplifier");
    //addEffect("Cabinet");
    //addEffect("Oscilloscope");
}

void FxRack::addEffect(const char * effectName)
{
    if(const EffectDescriptor * descr = EffectDescriptor::find(effectName))
        addEffect(descr);
    else
        cerr << "Can not find effect: " << effectName << endl;
}

void FxRack::addEffect(const EffectDescriptor * descriptor)
{
    FxPanel * panel = descriptor->createFxPanel();
    FxFrame * frame = new FxFrame(panel);

    if(effects_.size())
        effects_.insert(effects_.begin() + (effects_.size() - 1), frame);
    else
        effects_.push_back(frame);

    if (panel->device())
        AudioProcessor::addDevice(panel->device(), 0);

    content().addControl(frame);
    alignEffects();
}

void FxRack::alignEffects()
{
    float y = 0;
    float x = 0;

    float rackWidth = content().width();

    float scale = rackWidth / DefaultWidth;
    float horizPadding = rackTextureWidth_ * scale;
    constexpr float DefaultVertPadding = 10;
    float vertPadding = DefaultVertPadding * scale;
    float fxPanelWidth = rackWidth - 2 * horizPadding;

    for(FxFrame * frame : effects_) {
        frame->setPosition({ x, y });

        FxPanel * panel = frame->fxPanel_;
        float fxPanelHeight = floor(panel->scaleHeight(fxPanelWidth));

        panel->setPosition({ horizPadding, vertPadding });
        panel->setSize({ fxPanelWidth, fxPanelHeight });
        frame->setSize({ fxPanelWidth + 2 * horizPadding, fxPanelHeight + 2 * vertPadding });

        y += frame->height();
    }

    if(y < height())
        y = height();

    content().setHeight(y);

    redraw();
}

void FxRack::onResized()
{
    base::onResized();

    if(width() != lastWidth_) {
        lastWidth_ = width();
        alignEffects();
    }
}

void FxRack::drawBackground(Canvas & canvas, const sf::Time &)
{
    Rect r = content().bounds();
    float textureScale = r.width() / DefaultWidth;
    float textureWidth = rackTextureWidth_ * textureScale;

    canvas.fill(Rect{0, 0, textureWidth, r.height()},
                rackTexture_, textureScale);

    canvas.fill(Rect{r.right() - textureWidth - 1, 0, textureWidth, r.height()},
                rackTexture_, textureScale);
}

DropEffect FxRack::drop(Control * dropToControl, const DragData & dd, PointerEventArgs & e)
{
    if(dd.format== "AddFx") {
        // data contains EffectDescriptor
        const EffectDescriptor * descr = reinterpret_cast<const EffectDescriptor *>(dd.data);
        addEffect(descr);
        return DropEffect::Copy;
    }

	return DropEffect();
}

DropEffect FxRack::hover(Control * dropToControl, const DragData & dd, PointerEventArgs & e)
{
	return DropEffect::Move;
}
