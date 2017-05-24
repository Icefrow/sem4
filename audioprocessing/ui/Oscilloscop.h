#pragma once

class Oscilloscop
{
public:
    enum { WIDTH = 220, HEIGHT = 80, MAX_SAMPLES = 220*2 };
    enum { CMD_INVALIDATE = 1001 };

    Oscilloscop(HWND mainW, HWND controlW, float scale = 1.0f);

    void paint(HDC dc, RECT clip);

    void consume(float * data, long samples) {
        while (samples--) {
            if (!nextData(*data++))
                return;
        }
    }

    bool nextData(float s) {
        switch(_state) {
        case iState::WaitPaint:
            return false;

        case iState::Filling:
            if(sampleCounter_ == MAX_SAMPLES) {
                _state = iState::WaitPaint;
                PostMessage(mainWindowHWnd, WM_COMMAND, CMD_INVALIDATE, LPARAM(controlHWnd));
                return false;
            }

            samples_[sampleCounter_++] = s;
            return true;

        case iState::WaitTrigger:
            if(_lastValue < 0 && s > 0 || waitForFront_== MAX_SAMPLES) {
                _state = iState::Filling;
                waitForFront_ = 0;
                return nextData(s);
            }

            _lastValue = s;
            waitForFront_++;
            return true;

        default:
            return false;
        }
    }

private:
    float scale_;
    enum class iState { WaitTrigger, WaitPaint, Filling };

    float samples_[MAX_SAMPLES];
    int sampleCounter_;
    float _lastValue;
    iState _state = iState::WaitTrigger;
    int waitForFront_ = 0;
    HWND mainWindowHWnd;
    HWND controlHWnd;
};

