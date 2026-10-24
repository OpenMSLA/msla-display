#pragma once

#include "ui/util.h"
#include "widget.h"

class Label : public Widget {
private:
    uint32_t bg_color;
    uint32_t text_color;

    DirtyArea dirty_area;

    bool center_x = false;
    bool center_y = false;

public:
    String text;
    uint8_t font_size = 1;

    Label(TFT_eSPI& tft, String text, int x, int y, uint32_t text_color, uint32_t bg_color,
          uint8_t font_size)
        : text(text), bg_color(bg_color), text_color(text_color), font_size(font_size),
          Widget(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color, this->bg_color);
        tft.setTextSize(this->font_size);

        uint16_t w = tft.textWidth(text);
        uint16_t h = tft.fontHeight();

        uint16_t x = center_x ? getX() - w / 2 : getX();
        uint16_t y = center_y ? getY() - h / 2 : getY();

        tft.fillRect(dirty_area.x, dirty_area.y, dirty_area.w, dirty_area.h, this->bg_color);
        tft.drawString(this->text, x, y);
    };

    void update(TouchPointType& touch) override {
        redraw();
        this->dirty = false;
    };

    /// @brief Set label text
    void setText(String str) {
        if (str == text)
            return;

        uint8_t s = tft.textsize;
        tft.setTextSize(this->font_size);
        int text_w = tft.textWidth(text);
        int font_h = tft.fontHeight();
        tft.setTextSize(s);

        uint16_t x = center_x ? getX() - text_w / 2 : getX();
        uint16_t y = center_y ? getY() - font_h / 2 : getY();

        this->dirty_area = DirtyArea(x, y, text_w, font_h);

        this->text = str;
        this->invalidate();
    };

    void setCentering(bool x, bool y) {
        center_x = x;
        center_y = y;
    }

    void setBgColor(uint16_t c) {
        if (bg_color == c) {
            return;
        }

        bg_color = c;
        invalidate();
    }
};