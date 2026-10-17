#pragma once

#include "timer.h"
#include "uart/uart.h"
#include "ui/screens/screen.h"
#include "ui/widgets/button.h"
#include "ui/widgets/container.h"
#include "ui/widgets/hr.h"
#include "ui/widgets/image.h"
#include "ui/widgets/label.h"
#include "ui/widgets/multiline_text.h"
#include "ui/widgets/remote_image.h"

class ModelPreview : public Screen {
private:
    uint16_t file_id = 0;
    uint16_t return_id;

    bool requesting = true;
    bool first_request = true;
    Timer requesting_timer;

    Container image_cont = Container(tft, CARD_MARGIN, CARD_MARGIN, TFT_WIDTH / 2, TFT_WIDTH / 2, 5,
                                     TFT_CARD_COLOR, TFT_BG_COLOR);

    Image logo = Image(tft, ((TFT_WIDTH / 2) - 120) / 2, ((TFT_WIDTH / 2) - 120) / 2, 120, 120);

    RemoteImage preview = RemoteImage(tft, 0, 0, TFT_WIDTH / 2, TFT_WIDTH / 2, 2);

    Container cont = Container(tft, TFT_WIDTH / 2 + CARD_MARGIN * 2, CARD_MARGIN,
                               TFT_HEIGHT - (TFT_WIDTH / 2 + CARD_MARGIN * 3), TFT_WIDTH / 2, 5,
                               TFT_CARD_COLOR, TFT_BG_COLOR);

    Label model_info_header =
        Label(tft, "Model info", CARD_PADDING, CARD_PADDING, TFT_WHITE, TFT_CARD_COLOR, 2);

    Hr hr1 = Hr(tft, CARD_MARGIN, CARD_GAP + 20,
                TFT_HEIGHT - (TFT_WIDTH / 2 + (CARD_GAP + CARD_MARGIN * 3)), 2, TFT_EL_COLOR,
                TFT_CARD_COLOR);

    Container printing_status_container =
        Container(tft, TFT_WIDTH / 2 + CARD_GAP, CARD_MARGIN + 32,
                  TFT_HEIGHT - (TFT_WIDTH / 2 + (CARD_GAP + CARD_MARGIN)), TFT_WIDTH / 2 - 37, 0,
                  TFT_CARD_COLOR, TFT_CARD_COLOR);

    /// layers
    Image stack_image = Image(tft, CARD_PADDING - 3, CARD_GAP, 24, 24);

    Label layers =
        Label(tft, "Layers: -", CARD_PADDING + 27, CARD_GAP + 5, TFT_WHITE, TFT_CARD_COLOR, 2);

    Image total_time_image = Image(tft, CARD_PADDING - 3, CARD_GAP + 25, 24, 24);

    Label total_time =
        Label(tft, "Time: -s", CARD_PADDING + 27, CARD_GAP + 30, TFT_WHITE, TFT_CARD_COLOR, 2);

    Image size_image = Image(tft, CARD_PADDING - 3, CARD_GAP + 55, 24, 24);

    Label size =
        Label(tft, "Size: -B", CARD_PADDING + 27, CARD_GAP + 60, TFT_WHITE, TFT_CARD_COLOR, 2);

    Image back_image = Image(tft, (50 - 24) / 2, (50 - 24) / 2, 24, 24);

    Button back_button = Button(tft, "", CARD_MARGIN, TFT_WIDTH - (50 + CARD_MARGIN), 50, 50, 5,
                                TFT_CARD_COLOR, TFT_BG_COLOR, TFT_BG_COLOR, 1);

    Button print_button =
        Button(tft, "Print model", CARD_MARGIN * 2 + 50, TFT_WIDTH - (50 + CARD_MARGIN),
               TFT_HEIGHT - 50 - CARD_MARGIN * 3, 50, 5, TFT_ACT_COLOR, TFT_WHITE, TFT_BG_COLOR, 2);

    Container name_cont =
        Container(tft, CARD_MARGIN, TFT_WIDTH / 2 + CARD_MARGIN * 2, TFT_HEIGHT - CARD_MARGIN * 2,
                  TFT_WIDTH / 2 - (CARD_MARGIN * 4 + 50), 5, TFT_CARD_COLOR, TFT_BG_COLOR);

    MultilineText name =
        MultilineText(tft, "", CARD_PADDING, CARD_PADDING, name_cont.w - CARD_PADDING * 2,
                      name_cont.h - CARD_PADDING * 2, TFT_WHITE, TFT_CARD_COLOR, 2);

public:
    ModelPreview(TFT_eSPI& tft, Application& app);

    void update(TouchPointType& t) override;
    void onUART(PacketReader& pr) override;
    void invalidate() override;

    /// @brief Set displaying model id
    void setFileId(uint16_t i, uint16_t ret) {
        file_id = i;
        return_id = ret;
    }
};