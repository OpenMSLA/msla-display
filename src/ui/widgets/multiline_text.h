#pragma once

#include "ui/util.h"
#include "widget.h"

class MultilineText : public Widget {
private:
    int w;
    int h;
    uint32_t bg_color;
    uint32_t text_color;

    DirtyArea dirty_area;

public:
    String text;
    uint8_t font_size = 1;

    MultilineText(TFT_eSPI& tft, String text, int x, int y, int w, int h, uint32_t text_color,
                  uint32_t bg_color, uint8_t font_size)
        : text(text), bg_color(bg_color), text_color(text_color), font_size(font_size), w(w), h(h),
          Widget(tft, x, y) {}

    void draw() override {
        tft.setTextColor(this->text_color, this->bg_color);
        tft.setTextSize(this->font_size);

        tft.fillRect(dirty_area.x, dirty_area.y, dirty_area.w, dirty_area.h, this->bg_color);

        int line_index = 0;
        String line;
        String word;

        for (int i = 0; i < text.length(); i++) {
            char c = text[i];

            if (c != ' ') {
                word += c;
                continue;
            }

            if (!line.isEmpty() && tft.textWidth(line + word) > w) {
                tft.drawString(line, getX(), getY() + (tft.fontHeight() + 3) * line_index++);
                line = word;
            } else {
                line += word;
            }

            line += ' ';
            word.clear();
        }

        line += word;

        if (!line.isEmpty()) {
            tft.drawString(line, getX(), getY() + (tft.fontHeight() + 3) * line_index);
        }
    };

    void update(TouchPointType& touch) override {
        redraw();
        this->dirty = false;
    };

    /// @brief Set label text
    void setText(String str) {
        if (str == text)
            return;

        this->dirty_area = DirtyArea(getX(), getY(), w, h);

        this->text = str;
        this->invalidate();
    };
};