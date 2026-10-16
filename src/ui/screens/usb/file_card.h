#pragma once

#include "ui/widgets/image.h"
#include "ui/widgets/widget.h"

class FileCard : public Widget {
private:
    uint32_t bg_color;
    uint32_t bgbg_color;
    uint32_t text_color;

    int32_t r;

    uint16_t id;
    uint16_t parent_id;
    uint8_t file_type;
    String name;

    bool available = false;

    Image icon = Image(tft, 13, 13, 24, 24);

public:
    int w;
    int h;

    FileCard(TFT_eSPI& tft, int x, int y, int w, int h, int32_t r, uint32_t bg_color,
             uint32_t bggb_color, uint32_t text_color)
        : w(w), h(h), bg_color(bg_color), bgbg_color(bgbg_color), r(r), text_color(text_color),
          Widget(tft, x, y) {}

    void draw() override {
        if (!available) {
            return;
        }

        tft.fillSmoothRoundRect(getX(), getY(), w, h, r, bg_color, bgbg_color);

        icon.setRelativeCords(getX(), getY());
        icon.draw();

        tft.setTextSize(2);
        tft.setTextColor(text_color);

        int text_h = tft.fontHeight();
        tft.drawString(name, getX() + 48, getY() + (h / 2 - text_h / 2));
    };

    void update(TouchPointType& touch) override {
        redraw();
        this->dirty = false;
    };

    /// @brief Sets file
    void set_file(String n, uint16_t i, uint16_t pi, uint8_t ft) {
        name = n;
        id = i;
        parent_id = pi;
        file_type = ft;

        if (file_type == 0) {
            icon.loadImage("/file.rgb565");
        } else {
            icon.loadImage("/folder.rgb565");
        }

        if (name.length() > 29)
            name = name.substring(0, 29) + "...";

        invalidate();
    }

    /// @brief Sets card available (draw or not)
    void set_available(bool av) {
        available = av;
    }
};
