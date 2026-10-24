#pragma once

#include "config.h"
#include "uart/packet.h"
#include "ui/widgets/container.h"
#include "ui/widgets/widget.h"

#include <FT6336U.h>
#include <TFT_eSPI.h>
#include <vector>

class Application;

class Screen {
public:
    Application& app;

    using SetScreenCallback = std::function<void(String)>;

    String screen_name;
    Screen(TFT_eSPI& tft, Application& app, String screen_name)
        : tft(tft), screen_name(screen_name), app(app) {}

    virtual void update(TouchPointType& t) = 0;

    /// @brief Calls on every uart packet
    virtual void onUART(PacketReader& pr) = 0;

    /// @brief Invalidate screen contents
    virtual void invalidate() = 0;

protected:
    TFT_eSPI& tft;
};

class OverlayScreen : public Screen {
public:
    Container base_container;

    OverlayScreen(TFT_eSPI& tft, Application& app, String screen_name, uint16_t x, uint16_t y,
                  uint16_t w, uint16_t h, uint16_t r, uint16_t bg_color, uint16_t bgbg_color)
        : base_container(tft, x, y, w, h, r, bg_color, bgbg_color), Screen(tft, app, screen_name) {}

    void add_children(Widget* child) {
        base_container.add_children(child);
    }
};