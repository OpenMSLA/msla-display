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

        int text_size = font_size;
        int line_index = 0;
        int shrinkage_offset = 0;

        String line;
        String word;

        auto drawLine = [&]() {
            if (line.isEmpty())
                return;

            tft.drawString(line, getX(),
                           getY() + (tft.fontHeight() + 3) * line_index + shrinkage_offset);

            line.clear();
            line_index++;
        };

        auto addWord = [&](const String& word) {
            if (word.isEmpty())
                return;

            if (!line.isEmpty() && tft.textWidth(line + word) > w) {
                drawLine();
            }

            for (int j = 0; j < word.length(); j++) {
                char c = word[j];

                if (!line.isEmpty() && tft.textWidth(line + c) > w) {
                    drawLine();
                }

                line += c;
            }
        };

        for (int i = 0; i < text.length(); i++) {
            char c = text[i];

            if (c != ' ') {
                word += c;
                continue;
            }

            int offset_y = (tft.fontHeight() + 3) * line_index;

            if ((float)offset_y / (float)h > 0.5 && (float)i / (float)text.length() < 0.5 &&
                shrinkage_offset == 0) {

                text_size = 1;
                tft.setTextSize(text_size);

                shrinkage_offset = offset_y;
                line_index = 0;
            }

            addWord(word);
            word.clear();

            if (!line.isEmpty() && tft.textWidth(line + ' ') <= w) {
                line += ' ';
            }
        }

        addWord(word);

        if (!line.isEmpty()) {
            tft.drawString(line, getX(),
                           getY() + (tft.fontHeight() + 3) * line_index + shrinkage_offset);
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