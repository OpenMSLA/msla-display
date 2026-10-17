#include "model_preview.h"

#include "ui/app.h"

ModelPreview::ModelPreview(TFT_eSPI& tft, Application& app)
    : Screen(tft, app, "model_preview"), requesting_timer(2000, true) {
    logo.loadImage("/logo.rgb565");
    stack_image.loadImage("/stack.rgb565");
    total_time_image.loadImage("/clock.rgb565");
    size_image.loadImage("/file_card_color.rgb565");

    back_image.loadImage("/back.rgb565");
    back_button.add_children(&back_image);
    back_button.setPressedCallback([this]() { this->app.setScreen("usb"); });

    print_button.setPressedCallback([this]() {
        PacketWriter pw;
        pw.write((uint8_t)76);
        pw.write(this->file_id);

        UARTSend(pw);
    });

    preview.setRequestCallback([this](uint16_t h, uint16_t request_bytes, uint16_t offset_bytes) {
        PacketWriter pw;
        pw.write((uint8_t)74);
        pw.write(this->file_id);
        pw.write(h);
        pw.write(request_bytes);
        pw.write(offset_bytes);

        UARTSend(pw);
    });

    cont.add_children(&model_info_header);
    cont.add_children(&hr1);

    image_cont.add_children(&logo);
    image_cont.add_children(&preview);

    name_cont.add_children(&name);

    printing_status_container.add_children(&stack_image);
    printing_status_container.add_children(&layers);

    printing_status_container.add_children(&total_time_image);
    printing_status_container.add_children(&total_time);

    printing_status_container.add_children(&size_image);
    printing_status_container.add_children(&size);
}

void ModelPreview::update(TouchPointType& t) {

    if (requesting && (requesting_timer.timeout() || first_request)) {
        first_request = false;

        preview.startLoading();

        PacketWriter pw;
        pw.write((uint8_t)72);
        pw.write(this->file_id);

        UARTSend(pw);
    }

    cont.update(t);
    image_cont.update(t);
    back_button.update(t);
    print_button.update(t);
    name_cont.update(t);
    printing_status_container.update(t);
}

void ModelPreview::onUART(PacketReader& pr) {
    uint8_t id;
    if (!pr.readUInt8(id))
        return;

    switch (id) {
        /// Model info response
        case 73: {
            uint8_t status;
            if (!pr.readUInt8(status))
                return;

            if (status == 0) {
                uint16_t total_layers;
                pr.readUInt16(total_layers);

                layers.setText("Layers: " + String(total_layers));

                uint32_t print_time;
                pr.readUInt32(print_time);

                total_time.setText(String("Time: ") + String(format_duration(print_time).c_str()));

                float layer_height;
                pr.readFloat(layer_height);

                uint32_t file_size;
                pr.readUInt32(file_size);

                size.setText(String("Size: ") + String(format_size(file_size).c_str()));

                String name_str = "<No name>";
                uint8_t name_available;
                if (pr.readUInt8(name_available) && name_available == 1 &&
                    pr.readString(&name_str)) {
                }

                name.setText(name_str);
                print_button.setEnabled(true);
            } else if (status == 1) {
                String error;
                if (pr.readString(&error)) {
                    name.setText("Error: " + error);
                }

                print_button.setEnabled(false);
            }

            if (print_button.isEnabled()) {
                print_button.setFgColor(TFT_ACT_COLOR);
            } else {
                print_button.setFgColor(darken(TFT_ACT_COLOR, 80));
            }

            requesting = false;
            break;
        }
        /// Preview loading callback
        case 75: {
            preview.onUART(pr);
            break;
        }
        /// Start print callback
        case 77: {
            uint8_t status;
            if (!pr.readUInt8(status))
                return;

            if (status == 0) {
                this->app.setScreen("main");
                return;
            } else if (status == 1) {
                String error;
                if (pr.readString(&error)) {
                    name.setText("Error: " + error);
                }
            }

            break;
        }
    }
}

void ModelPreview::invalidate() {
    tft.fillScreen(TFT_BG_COLOR);

    cont.invalidate();
    preview.reset();
    back_button.invalidate();
    print_button.invalidate();
    image_cont.invalidate();
    name_cont.invalidate();
    printing_status_container.invalidate();

    name.setText("");
    layers.setText("Layers: -");
    total_time.setText("Time: -s");
    size.setText("Size: -B");
    // preview.invalidate();

    requesting = true;
    first_request = true;
    requesting_timer.reset();
}