#include "usb.h"

#include "ui/app.h"

Usb::Usb(TFT_eSPI& tft, Application& app) : Screen(tft, app, "usb"), requesting_timer(2000, true) {
    back_image.loadImage("/back.rgb565");
    back_button.add_children(&back_image);

    back_button.setPressedCallback([this]() { this->request(0, this->current_parent_id); });

    home_image.loadImage("/home-d.rgb565");
    home_button.add_children(&home_image);

    home_button.setPressedCallback([this]() { this->app.setScreen("main"); });

    page_prev_image.loadImage("/arrowl.rgb565");
    page_prev_button.add_children(&page_prev_image);
    page_prev_button.setPressedCallback([this]() {
        if (page == 0)
            return;

        page = page >= 1 ? page - 1 : 0;
        this->request(page, this->requesting_folder_id);
    });

    page_next_image.loadImage("/arrowr.rgb565");
    page_next_button.add_children(&page_next_image);
    page_next_button.setPressedCallback([this]() {
        uint16_t max_page = ceil(total_entries / (float)take) - 1;

        if (page == max_page)
            return;

        page = min(uint16_t(page + 1), max_page);
        this->request(page, this->requesting_folder_id);
    });

    main_container.add_children(&file_button_1);
    main_container.add_children(&file_button_2);
    main_container.add_children(&file_button_3);
    main_container.add_children(&file_button_4);

    file_button_1.add_children(&file_card_1);
    file_button_2.add_children(&file_card_2);
    file_button_3.add_children(&file_card_3);
    file_button_4.add_children(&file_card_4);

    top_container.add_children(&header);

    nothing_to_c.setCentering(true, true);
    nothing_to_c.disableSelfRedraw();
    main_container.add_children(&nothing_to_c);
}

void Usb::update(TouchPointType& t) {
    if (requesting && (requesting_timer.timeout() || first_request)) {
        /// Request folder
        sendRequest();
        first_request = false;
    }

    top_container.update(t);
    main_container.update(t);
    back_button.update(t);
    home_button.update(t);

    page_next_button.update(t);
    page_prev_button.update(t);

    if (total_entries == 0) {
        nothing_to_c.draw();
    }
}

void Usb::onUART(PacketReader& pr) {
    uint8_t id;

    FileCard* arr[4] = {&file_card_1, &file_card_2, &file_card_3, &file_card_4};
    Button* buttons_arr[4] = {&file_button_1, &file_button_2, &file_button_3, &file_button_4};

    if (pr.readUInt8(id) && id == 71) {
        for (int i = 0; i < 4; i++) {
            arr[i]->set_available(false);
            buttons_arr[i]->setEnabled(false);
        }

        pr.readUInt16(current_parent_id);

        uint16_t count;
        pr.readUInt16(count);

        for (int x = 0; x < count; x++) {
            uint16_t id;
            uint8_t ft;
            uint32_t size;
            String name;

            pr.readUInt16(id);
            pr.readUInt8(ft);
            pr.readUInt32(size);
            pr.readString(&name);

            if (x < sizeof(arr) / sizeof(FileCard*)) {
                arr[x]->set_file(name, id, current_parent_id, ft);
                arr[x]->set_available(true);
                buttons_arr[x]->setEnabled(true);

                if (ft == 0) {
                    buttons_arr[x]->setPressedCallback([this, id]() {
                        this->app.model_preview.setFileId(id, requesting_folder_id);
                        this->app.setScreen("model_preview");
                    });
                } else {
                    buttons_arr[x]->setPressedCallback([this, id]() { this->request(0, id); });
                }
            }
        }
        pr.readUInt16(total_entries);

        header.setCentering(true, true);
        header.setText("Page " + String(page + 1) + "/" +
                       String(max(1, (int)ceil(total_entries / (float)take))));

        main_container.invalidate();
        requesting = false;
    }
}

void Usb::invalidate() {
    tft.fillScreen(TFT_BG_COLOR);

    top_container.invalidate();
    main_container.invalidate();
    back_button.invalidate();
    home_button.invalidate();

    page_next_button.invalidate();
    page_prev_button.invalidate();

    /*
    file_card_1.set_available(false);
    file_card_2.set_available(false);
    file_card_3.set_available(false);
    file_card_4.set_available(false);
    */

    // page = 0;
    // requesting_folder_id = 0;

    // current_parent_id = 0;
    // total_entries = 0;

    // requesting = true;
    // first_request = true;
    // requesting_timer.reset();
}

void Usb::sendRequest() {
    PacketWriter pw;
    pw.write((uint8_t)70); // packet id
    pw.write((uint16_t)requesting_folder_id);
    pw.write((uint16_t)page);
    pw.write((uint16_t)take);

    UARTSend(pw);
}

void Usb::request(uint16_t page, uint16_t id) {
    this->page = page;
    this->requesting_folder_id = id;

    this->requesting = true;
    this->requesting_timer.reset();
    this->first_request = true;
}