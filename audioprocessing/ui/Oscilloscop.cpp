#include "pch.h"

#include "Oscilloscop.h"

#include <gdiplus.h>
#include <memory>

#pragma comment (lib,"Gdiplus.lib")

using namespace Gdiplus;
using namespace std;

namespace {

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

struct GdipInit {
    GdipInit() {
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }

    ~GdipInit() {
        GdiplusShutdown(gdiplusToken);
    }
} gdipInit_;

Pen inputColor_(Color(180, 0, 192, 127));
Pen outLColor_(Color(180, 0, 255, 0));
Pen outRColor_(Color(180, 255, 0, 0));
//SolidBrush background_(Color(15, 15, 15));

LinearGradientBrush background_(PointF{ .0f, 0.f }, PointF{ 100.0f, 200.f }, Color(66, 66,
                                66), Color(33, 33, 33));

auto_ptr<Bitmap> memBitmap_;// = new Bitmap(rect.Width(), rect.Height());
auto_ptr<Graphics> memGraphics_;

}   // namespace

Oscilloscop::Oscilloscop(HWND mainW, HWND controlW, float scale)
    : scale_(scale)
    , sampleCounter_(0)
    , _lastValue(0)
    , mainWindowHWnd(mainW)
    , controlHWnd(controlW)
{
    memset(samples_, 0, sizeof(samples_));
}

void Oscilloscop::paint(HDC dc, RECT clip)
{
    //RECT clip;

    PAINTSTRUCT ps = { 0 };
    bool needDisposeDC = false;

    if(dc == NULL) {
        dc = BeginPaint(controlHWnd, &ps);
        needDisposeDC = true;
        clip = ps.rcPaint;
    }
    else {
        //dc = (HDC)wParam;
        //clip = { 0, 0, WIDTH, HEIGHT };
    }

    Graphics graphics(dc);

    if(!memBitmap_.get()) {
        memBitmap_.reset(new Bitmap(WIDTH, HEIGHT, &graphics));
        memGraphics_.reset(Graphics::FromImage(memBitmap_.get()));
		memGraphics_->SetSmoothingMode(SmoothingModeAntiAlias);
    }

    RectF rf(float(clip.left),
             float(clip.top),
             float(clip.right - clip.left),
             float(clip.bottom - clip.top));

    //Graphics * g = memGraphics_.get();
    memGraphics_->SetClip(rf);

    Pen * pen = &inputColor_;

    LineJoin lj = pen->GetLineJoin();
    RectF rc(0, 0, float(WIDTH), float(HEIGHT));

    memGraphics_->FillRectangle(&background_, rc);

    if(_state != iState::WaitPaint)
        memGraphics_->DrawLine(pen, rc.X, rc.Height / 2, rc.GetRight(), rc.Height / 2);
    else {
        float Kx = rc.Width / MAX_SAMPLES;
        float Ky = -float(HEIGHT) * scale_;
        float Dy = HEIGHT / 2.0f;

        float(&samples)[MAX_SAMPLES] = samples_;

        for(int i = 1; i < MAX_SAMPLES; i++)
            memGraphics_->DrawLine(pen, (i - 1)*Kx, samples[i - 1] * Ky + Dy, i * Kx,
                                   samples[i] * Ky + Dy);

        sampleCounter_ = 0;
        _state = iState::WaitTrigger;
        _lastValue = 1;
    }

    graphics.DrawImage(memBitmap_.get(), 0, 0);

    if(needDisposeDC)
        EndPaint(controlHWnd, &ps);
}



