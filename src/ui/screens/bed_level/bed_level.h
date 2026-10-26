#pragma once

#include "timer.h"
#include "uart/uart.h"
#include "ui/screens/screen.h"

#include <ui/widgets/button.h>
#include <ui/widgets/container.h>
#include <ui/widgets/hr.h>
#include <ui/widgets/image.h>
#include <ui/widgets/label.h>
#include <ui/widgets/multiline_text.h>

#define BUTTONS_SIZE 45

class BedLevel : public Screen {
private:
    Timer pos_timer;
    bool homed = false;
    bool manual = false;

    float bed_pos = 0.f;

    Container cont =
        Container(tft, 5, 5, TFT_HEIGHT - 10, TFT_WIDTH - 10, 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    Label general_label = Label(tft, "Bed Leveling", cont.w / 2, 10, TFT_WHITE, TFT_CARD_COLOR, 3);

    Hr hr1 =
        Hr(tft, CARD_PADDING, 45, cont.w - (CARD_PADDING * 2), 2, TFT_EL_COLOR, TFT_CARD_COLOR);

    /// ----------------------------------------------------------

    Label first_label = Label(tft, "1.", CARD_PADDING, 45 + CARD_PADDING + (BUTTONS_SIZE / 2),
                              TFT_WHITE, TFT_CARD_COLOR, 2);

    Label home_label = Label(tft, "Home", (BUTTONS_SIZE - 24) / 2 + 35, BUTTONS_SIZE / 2, TFT_WHITE,
                             TFT_ACT_COLOR, 2);

    Image home_image = Image(tft, (BUTTONS_SIZE - 24) / 2, (BUTTONS_SIZE - 24) / 2, 24, 24);

    Button home_btn = Button(tft, "", 45, 45 + CARD_PADDING, BUTTONS_SIZE * 2.6, BUTTONS_SIZE, 5,
                             TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 2);

    /// ----------------------------------------------------------

    Label second_label =
        Label(tft, "2.", CARD_PADDING, 45 + CARD_PADDING * 2 + BUTTONS_SIZE + (BUTTONS_SIZE / 2),
              TFT_WHITE, TFT_CARD_COLOR, 2);

    Button up_hundred = Button(tft, "+0.01", 45, 45 + CARD_PADDING * 2 + BUTTONS_SIZE, BUTTONS_SIZE,
                               BUTTONS_SIZE, 5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 1);

    Button up_ten =
        Button(tft, "+0.1", 45 + BUTTONS_SIZE + CARD_PADDING, 45 + CARD_PADDING * 2 + BUTTONS_SIZE,
               BUTTONS_SIZE, BUTTONS_SIZE, 5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 1);

    Button up_one = Button(tft, "+1", 45 + BUTTONS_SIZE * 2 + CARD_PADDING * 2,
                           45 + CARD_PADDING * 2 + BUTTONS_SIZE, BUTTONS_SIZE, BUTTONS_SIZE, 5,
                           TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 1);

    Label pos_label =
        Label(tft, "Z: ?.??mm", 45, 45 + CARD_PADDING * 2 + BUTTONS_SIZE * 2 + (BUTTONS_SIZE / 2),
              TFT_WHITE, TFT_CARD_COLOR, 2);

    Button down_hundred =
        Button(tft, "-0.01", 45, 45 + CARD_PADDING * 2 + BUTTONS_SIZE * 3, BUTTONS_SIZE,
               BUTTONS_SIZE, 5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 1);

    Button down_ten = Button(tft, "-0.1", 45 + BUTTONS_SIZE + CARD_PADDING,
                             45 + CARD_PADDING * 2 + BUTTONS_SIZE * 3, BUTTONS_SIZE, BUTTONS_SIZE,
                             5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 1);

    Button down_one = Button(tft, "-1", 45 + BUTTONS_SIZE * 2 + CARD_PADDING * 2,
                             45 + CARD_PADDING * 2 + BUTTONS_SIZE * 3, BUTTONS_SIZE, BUTTONS_SIZE,
                             5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 1);

    /// ----------------------------------------------------------

    Label third_label = Label(tft, "3.", CARD_PADDING,
                              45 + CARD_PADDING * 3 + BUTTONS_SIZE * 4 + (BUTTONS_SIZE / 2),
                              TFT_WHITE, TFT_CARD_COLOR, 2);

    Button save_btn =
        Button(tft, "Save", 45, 45 + CARD_PADDING * 3 + BUTTONS_SIZE * 4, BUTTONS_SIZE * 2.6,
               BUTTONS_SIZE, 5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 2);

    /// ----------------------------------------------------------

    Container error_cont = Container(
        tft, cont.w / 2 + CARD_PADDING, 45 + CARD_PADDING, cont.w / 2 - (CARD_PADDING * 2),
        cont.h - (45 + CARD_PADDING * 2), 5, TFT_EL_COLOR, TFT_CARD_COLOR);

    MultilineText error =
        MultilineText(tft, "", CARD_PADDING, CARD_PADDING, error_cont.w - (CARD_PADDING * 2),
                      error_cont.h - (CARD_PADDING * 2), TFT_WHITE, TFT_EL_COLOR, 2);

public:
    BedLevel(TFT_eSPI& tft, Application& app);

    void update(TouchPointType& t) override;
    void onUART(PacketReader& pr) override;
    void invalidate() override;

    void send(double pos) {
        if (!homed)
            return;

        manual = true;
        PacketWriter pw;
        pw.write((uint8_t)60);
        pw.write((double)pos);

        UARTSend(pw);
    }

    void save(double pos) {
        if (!homed || !manual)
            return;

        PacketWriter pw;
        pw.write((uint8_t)62);
        pw.write((double)pos);

        UARTSend(pw);
    }

    void updatePosLabel() {
        pos_label.setText("Z: " + String(bed_pos, 2) + "mm");
    }
};