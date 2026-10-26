#pragma once

#include "screens/bed_level/bed_level.h"
#include "screens/initial/initial.h"
#include "screens/main/main.h"
#include "screens/screen.h"
#include "screens/usb/model_preview/model_preview.h"
#include "screens/usb/usb.h"

#include <FT6336U.h>
#include <TFT_eSPI.h>

class Application {
public:
    Initial initial_screen;
    Main main_screen;
    Usb usb_screen;
    ModelPreview model_preview;

    BedLevel bed_level;

    Application(TFT_eSPI& tft)
        : tft(tft), main_screen(tft, *this), initial_screen(tft, *this), usb_screen(tft, *this),
          model_preview(tft, *this), bed_level(tft, *this), standby_timer(STANDBY_TIMEOUT * 10000) {
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);

        ledcSetup(0, 20000, 8);
        ledcAttachPin(TFT_BCKL, 0);

        screens.push_back(&main_screen);
        screens.push_back(&initial_screen);
        screens.push_back(&usb_screen);
        screens.push_back(&model_preview);
        screens.push_back(&bed_level);

        setScreen("initial");
    }

    void update(FT6336U& touch) {
        TouchPointType t = getTouch(touch);

        if (t.status != TouchStatusEnum::release) {
            standby_timer.reset();
            if (standby) {
                restoreStandbyScreen();
                post_standby_touch_supress = true;
            }
        }

        if (post_standby_touch_supress && t.status == TouchStatusEnum::release)
            post_standby_touch_supress = false;

        if (post_standby_touch_supress)
            t.status = TouchStatusEnum::release;

        if (standby_timer.timeout() && !standby) {
            setBacklight(0);

            standby = true;
        }

        if (screen_dirty) {
            screens[current_screen]->invalidate();
            screen_dirty = false;
        }

        screens[current_screen]->update(t);
    }

    void onUART(PacketReader pr) {
        screens[current_screen]->onUART(pr);
    }

    void setScreen(String name) {
        int i = 0;
        for (auto sc : screens) {
            if (sc->screen_name == name) {
                current_screen = i;
                screen_dirty = true;
                standby = false;
                post_standby_touch_supress = true;

                break;
            }
            i++;
        }
    }

    void restoreStandbyScreen() {
        standby_timer.reset();
        setBacklight(255);
        standby = false;
    }

    /// @brief Set backlight power
    void setBacklight(uint8_t pwm) {
        ledcWrite(0, pwm);
    }

private:
    TFT_eSPI& tft;

    std::vector<Screen*> screens;
    size_t current_screen = 0;

    Timer standby_timer;
    bool standby = false;

    bool post_standby_touch_supress = false;
    bool screen_dirty = false;
};