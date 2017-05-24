#pragma once
#include <ui/framework/ResourceManager.h>
#include <ui/framework/Panel.h>
#include <functional>

#include "devices/Device.h"

#include "AudioProcessor.h"
#include "CtlRoller.h"

// Forward declarations
class FxPanel;

/* ------------------------------------------------------ */
// Description of the effect
class EffectDescriptor : NonCopyable
{
public:
    EffectDescriptor(const char * effectName, const char * bkTexture);

    const char * effectName() const {
        return effectName_;
    }

    const char * bkTextureName() const {
        return bkTexture_;
    }

    virtual FxPanel * createFxPanel() const = 0;

    static const EffectDescriptor * find(const char * effectName);

    static const EffectDescriptor * first();

    const EffectDescriptor * next() const {
        return next_;
    }

private:
    const char * effectName_;
    const char * bkTexture_;

    const EffectDescriptor * next_;
};

/* ------------------------------------------------------ */
// Base GUI for effect control
class FxPanel : public Panel
{
    typedef Panel base;

public:
    explicit FxPanel(const EffectDescriptor * effectInfo);

    Device * device() {
        return device_;
    }

    float scaleHeight(float byWidth) const {
        float scale = byWidth / initialSize_.width();
        return initialSize_.height() * scale;
    }

protected:
    float scale() const {
        return scale_;
    }

    const Size & initialSize() const {
        return initialSize_;
    }

    void setDevice(Device * device) {
        device_ = device;
    }

    void bind(std::function<void(float value)> & callback, int ctlId) {
        callback = [this, ctlId](float value) {
            AudioProcessor::deviceCtl(device(), ctlId, value);
        };
    }

    void bind(CtlRoller & roller, int ctlId) {
        addControl(&roller);
        scalableControls_.push_back(&roller);
        device()->devCtl(ctlId, roller.value());
        bind(roller.eventValueChanged, ctlId);
    }

    void onResized() override;

    void drawBackground(Canvas & canvas, const sf::Time & elapsedTime) override;

private:
    sf::Texture * background_;
    float scale_ = 0;
    Size initialSize_;
    Device * device_ = nullptr;
    std::vector<IScalableControl*> scalableControls_;
};

/* ------------------------------------------------------ */
// Auto-register of effect
template<typename EffectPanel>
class FxPanelImpl : public FxPanel
{
protected:
    FxPanelImpl() : FxPanel(&descriptor) {}

    class Descriptor : public EffectDescriptor
    {
        typedef EffectDescriptor base;
    public:
        Descriptor(const char * effectName, const char * bkTexture)
            : base(effectName, bkTexture) {}

        FxPanel * createFxPanel() const override {
            return new EffectPanel();
        }
    };

    static Descriptor descriptor;
};

#define REGISTER_EFFECT(className, description, textureName)    \
    template<> className::Descriptor FxPanelImpl<className>::descriptor(description, textureName)
