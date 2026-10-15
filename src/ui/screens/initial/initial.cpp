#include "initial.h"

#include "ui/app.h"

Initial::Initial(TFT_eSPI& tft, Application& app)
    : Screen(tft, app, "initial"), update_timer(5000) {
    logo.loadImage("/logo.rgb565");

    uint8_t s = tft.textsize;
    tft.setTextSize(loading.font_size);
    loading.x = (TFT_HEIGHT - tft.textWidth(loading.text)) / 2;
    tft.setTextSize(s);

    cont.add_children(&logo);
    cont.add_children(&loading);
}

void Initial::update(TouchPointType& t) {
    if (update_timer.timeout() || first_update) {
        UARTSend(10); // STATUS REQUEST
        first_update = false;
    }

    cont.update(t);
}

void Initial::onUART(PacketReader& pr) {
    uint8_t id;
    // SUCCESS STATUS RESPONSE
    if (pr.readUInt8(id) && id == 11) {
        app.setScreen("main");
        return;
    }
}

void Initial::invalidate() {
    tft.fillScreen(TFT_BG_COLOR);

    cont.invalidate();
}