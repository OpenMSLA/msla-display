#pragma once

#include "file_card.h"
#include "timer.h"
#include "uart/uart.h"
#include "ui/screens/screen.h"

#include <ui/widgets/button.h>
#include <ui/widgets/container.h>
#include <ui/widgets/image.h>
#include <ui/widgets/label.h>

class Usb : public Screen {
private:
    uint16_t page = 0;
    uint16_t take = 4;
    uint16_t requesting_folder_id = 0;

    uint16_t current_parent_id = 0;

    bool requesting = true;
    bool first_request = true;
    Timer requesting_timer;

    Container top_container =
        Container(tft, CARD_MARGIN * 3 + 50 * 2, CARD_MARGIN,
                  TFT_HEIGHT - (CARD_MARGIN * 4 + 50 * 2), 50, 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    int main_card_width = TFT_HEIGHT - 10;

    Container main_container =
        Container(tft, CARD_MARGIN, CARD_MARGIN * 2 + 50, main_card_width,
                  TFT_WIDTH - (CARD_MARGIN * 3 + 50), 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    Image back_image = Image(tft, (50 - 24) / 2, (50 - 24) / 2, 24, 24);

    Button back_button = Button(tft, "", CARD_MARGIN, CARD_MARGIN, 50, 50, 5, TFT_CARD_COLOR,
                                TFT_TRANSPARENT, TFT_BG_COLOR, 1);

    Image home_image = Image(tft, (50 - 24) / 2, (50 - 24) / 2, 24, 24);

    Button home_button = Button(tft, "", CARD_MARGIN * 2 + 50, CARD_MARGIN, 50, 50, 5,
                                TFT_CARD_COLOR, TFT_TRANSPARENT, TFT_BG_COLOR, 1);

    Button file_button_1 =
        Button(tft, "", CARD_PADDING, CARD_PADDING, main_card_width - (CARD_PADDING * 2), 50, 5,
               TFT_CARD_COLOR, TFT_TRANSPARENT, TFT_CARD_COLOR, 1);

    FileCard file_card_1 = FileCard(tft, 0, 0, main_card_width - (CARD_PADDING * 2), 50, 5,
                                    TFT_EL_COLOR, TFT_CARD_COLOR, TFT_WHITE);

    Button file_button_2 = Button(tft, "", CARD_PADDING, CARD_PADDING + 50 + CARD_GAP,
                                  main_card_width - (CARD_PADDING * 2), 50, 5, TFT_CARD_COLOR,
                                  TFT_TRANSPARENT, TFT_CARD_COLOR, 1);

    FileCard file_card_2 = FileCard(tft, 0, 0, main_card_width - (CARD_PADDING * 2), 50, 5,
                                    TFT_EL_COLOR, TFT_CARD_COLOR, TFT_WHITE);

    Button file_button_3 = Button(tft, "", CARD_PADDING, CARD_PADDING + 50 * 2 + CARD_GAP * 2,
                                  main_card_width - (CARD_PADDING * 2), 50, 5, TFT_CARD_COLOR,
                                  TFT_TRANSPARENT, TFT_CARD_COLOR, 1);

    FileCard file_card_3 = FileCard(tft, 0, 0, main_card_width - (CARD_PADDING * 2), 50, 5,
                                    TFT_EL_COLOR, TFT_CARD_COLOR, TFT_WHITE);

    Button file_button_4 = Button(tft, "", CARD_PADDING, CARD_PADDING + 50 * 3 + CARD_GAP * 3,
                                  main_card_width - (CARD_PADDING * 2), 50, 5, TFT_CARD_COLOR,
                                  TFT_TRANSPARENT, TFT_CARD_COLOR, 1);

    FileCard file_card_4 = FileCard(tft, 0, 0, main_card_width - (CARD_PADDING * 2), 50, 5,
                                    TFT_EL_COLOR, TFT_CARD_COLOR, TFT_WHITE);

public:
    Usb(TFT_eSPI& tft, Application& app);

    void update(TouchPointType& t) override;
    void onUART(PacketReader& pr) override;
    void invalidate() override;

    void request(uint16_t page, uint16_t id);

    /// @brief Send request to a core
    void sendRequest();
};