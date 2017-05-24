#pragma once

#include <ui/framework/Panel.h>
#include <ui/framework/Splitter.h>

#include "LeftPanel.h"
#include "RightPanel.h"
#include "ConfigWindow.h"

class MainForm : public TransparentPanel
{
public:
    MainForm();
    ~MainForm();

    static MainForm & from(HostWindow * hw);
    
    void addEffect(const EffectDescriptor* descriptor);

protected:
    void onShowSettings();
    void onLoadEffect();
    void onSaveEffect();

    void onResized() override;

private:
    FxRack & rack();

    LeftPanel leftPanel_;
    Splitter splitter_;

    RightPanel rightPanel_;
    ConfigWindow configWindow_;

    float leftPanelProportion_;
};

