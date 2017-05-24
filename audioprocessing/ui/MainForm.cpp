#include "pch.h"
#include "MainForm.h"

constexpr float LeftPanelInitialProporion = 0.33f;

MainForm::MainForm()
    : leftPanelProportion_(LeftPanelInitialProporion)
{
    addControls({ &leftPanel_, &splitter_, &rightPanel_ });

    leftPanel_.addElement("Screamer");
    leftPanel_.addElement("Flanger");
    leftPanel_.addElement("Amplifier");
    leftPanel_.addElement("Cabinet");
    leftPanel_.addElement("Oscilloscope");
	leftPanel_.addElement("Compressor");

    setSize({ 1200, 700 });

    rightPanel_.btnSettings_.eventClick = [this] {
        configWindow_.show();
    };
}

MainForm::~MainForm()
{
}

MainForm & MainForm::from(HostWindow * hw)
{
    return *static_cast<MainForm *>(hw->content());
}

void MainForm::addEffect(const EffectDescriptor * descriptor)
{
    rack().addEffect(descriptor);
}

void MainForm::onShowSettings()
{
    configWindow_.show();
}

void MainForm::onResized()
{
    float x = float(int(width() * leftPanelProportion_));
    float y = 0;
    float h = height() - y;

    leftPanel_.setPosition({ 0, y });
    leftPanel_.setSize({ x, h});

    x = leftPanel_.right();
    splitter_.setPosition({x, y});
    splitter_.setSize({6, h});

    x = splitter_.right();
    rightPanel_.setPosition({x, y});
    rightPanel_.setSize({right() - x, h});
}

FxRack & MainForm::rack()
{
    return rightPanel_.fxRack_;
}
