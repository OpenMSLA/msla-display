#include "bed_level.h"

#include "ui/app.h"

BedLevel::BedLevel(TFT_eSPI& tft, Application& app)
    : Screen(tft, app, "bed_level"), pos_timer(1500) {
    home_image.loadImage("/home.rgb565");

    back_image.loadImage("/back.rgb565");
    general_label.setCentering(true, false);

    cont.add_children(&general_label);
    cont.add_children(&hr1);
    cont.add_children(&first_label);
    cont.add_children(&second_label);
    cont.add_children(&third_label);
    cont.add_children(&home_btn);

    cont.add_children(&back_button);

    cont.add_children(&up_hundred);
    cont.add_children(&up_ten);
    cont.add_children(&up_one);

    cont.add_children(&down_hundred);
    cont.add_children(&down_ten);
    cont.add_children(&down_one);
    cont.add_children(&pos_label);

    cont.add_children(&save_btn);

    cont.add_children(&error_cont);
    error_cont.add_children(&error);

    up_hundred.setPressedCallback([this]() { this->send(bed_pos + 0.01); });
    up_ten.setPressedCallback([this]() { this->send(bed_pos + 0.1); });
    up_one.setPressedCallback([this]() { this->send(bed_pos + 1); });

    down_hundred.setPressedCallback([this]() { this->send(bed_pos - 0.01); });
    down_ten.setPressedCallback([this]() { this->send(bed_pos - 0.1); });
    down_one.setPressedCallback([this]() { this->send(bed_pos - 1); });

    save_btn.setPressedCallback([this]() { this->save(bed_pos); });

    pos_label.setCentering(false, true);

    first_label.setCentering(false, true);
    second_label.setCentering(false, true);
    third_label.setCentering(false, true);

    home_btn.setPressedCallback([this]() {
        UARTSend(56);
        this->manual = false;
        this->homed = true;
    });
    home_btn.add_children(&home_image);
    home_btn.add_children(&home_label);

    home_label.setCentering(false, true);

    back_button.add_children(&back_image);
    back_button.setPressedCallback([this]() { this->app.setScreen("main"); });
}

void BedLevel::update(TouchPointType& t) {
    if (pos_timer.timeout() && homed && !manual) {
        UARTSend(12);
    }

    cont.update(t);
}

void BedLevel::onUART(PacketReader& pr) {
    uint8_t id;
    if (!pr.readUInt8(id))
        return;

    switch (id) {
        case 13:
            pr.pos++; // Ignore first byte of the packet - uv state
            if (pr.readFloat(bed_pos)) {
                this->updatePosLabel();
            }
            break;

        case 61: {
            uint8_t status;
            if (pr.readUInt8(status) && status == 0) {
                if (pr.readFloat(bed_pos)) {
                    this->updatePosLabel();
                }
            }
        }

        case 63: {
            uint8_t status;
            if (!pr.readUInt8(status))
                return;

            if (status == 0) {
                app.setScreen("main");
            } else if (status == 1) {
                String e;
                pr.readString(&e);
                error.setText(e);
            }
        }

        default:
            break;
    }
}

void BedLevel::invalidate() {
    tft.fillScreen(TFT_BG_COLOR);

    homed = false;
    manual = false;

    bed_pos = 0.f;

    cont.invalidate();
}