#include "pch.h"
#include "FxPanel.h"

#include <ui/framework/ResourceManager.h>

using namespace std;

/* ------------------------------------------------------ */
// EffectInfo members
static const EffectDescriptor * s_first = nullptr;

EffectDescriptor::EffectDescriptor(const char * effectName, const char * bkTexture)
    : effectName_(effectName)
    , bkTexture_(bkTexture)
    , next_(s_first)
{
    s_first = this;
}

const EffectDescriptor * EffectDescriptor::find(const char * effectName)
{
    for(const EffectDescriptor * walker = s_first; walker != nullptr; walker = walker->next())
        if(strcmp(walker->effectName(), effectName) == 0)
            return walker;

    return nullptr;
}

const EffectDescriptor * EffectDescriptor::first()
{
    return s_first;
}

/* ------------------------------------------------------ */
// FxPanel members
FxPanel::FxPanel(const EffectDescriptor * descriptor)
{
    background_ = ResourceManager::getTexture(descriptor->bkTextureName());
    initialSize_ = background_->getSize();
}

void FxPanel::onResized()
{
    scale_ = width() / initialSize_.width();

    for(IScalableControl * ctl : scalableControls_)
        ctl->setScale(scale());
}

void FxPanel::drawBackground(Canvas & canvas, const sf::Time &)
{
    Canvas c1 = canvas;
    c1.fill(internalBounds(), background_, scale_);
}
