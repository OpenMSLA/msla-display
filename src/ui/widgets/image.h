#pragma once

#include "ui/util.h"
#include "widget.h"

#include <FS.h>
#include <LittleFS.h>
#include <vector>

static inline uint16_t blend565(uint16_t background, uint16_t foreground, uint8_t alpha) {
    if (alpha == 0)
        return background;

    if (alpha == 255)
        return foreground;

    uint16_t inv_alpha = 255 - alpha;

    uint8_t r =
        (((background >> 11) & 0x1F) * inv_alpha + ((foreground >> 11) & 0x1F) * alpha + 127) / 255;

    uint8_t g =
        (((background >> 5) & 0x3F) * inv_alpha + ((foreground >> 5) & 0x3F) * alpha + 127) / 255;

    uint8_t b = ((background & 0x1F) * inv_alpha + (foreground & 0x1F) * alpha + 127) / 255;

    return (r << 11) | (g << 5) | b;
}

static inline uint16_t swap565(uint16_t color) {
    return (color >> 8) | (color << 8);
}

class Image : public Widget {
public:
    fs::File file;
    bool loaded = false;
    Image(TFT_eSPI& tft, int x, int y) : Widget(tft, x, y) {}

    void loadImage(String path) {
        file = LittleFS.open(path, "r");
        loaded = (bool)file;
    }

    ~Image() {
        if (file)
            file.close();
    }

    void draw() override {
        if (!loaded)
            return;

        constexpr size_t HEADER_SIZE = 5;
        constexpr size_t PIXELS_PER_CHUNK = 256;
        constexpr uint8_t FLAG_ALPHA = 0x01;

        uint8_t header[HEADER_SIZE];

        if (!file.seek(0))
            return;

        if (file.read(header, HEADER_SIZE) != HEADER_SIZE)
            return;

        const uint16_t image_w = header[0] | (uint16_t(header[1]) << 8);
        const uint16_t image_h = header[2] | (uint16_t(header[3]) << 8);
        const bool has_alpha = header[4] & 0x01;
        const size_t bytes_per_pixel = has_alpha ? 3 : 2;

        const int32_t x = getX();
        const int32_t y = getY();

        if (x < 0 || y < 0 || x + image_w > tft.width() || y + image_h > tft.height())
            return;

        const uint64_t pixel_count = uint64_t(image_w) * image_h;
        const uint64_t expected_size = HEADER_SIZE + pixel_count * bytes_per_pixel;

        if (file.size() < expected_size)
            return;

        if (!file.seek(HEADER_SIZE))
            return;

        uint8_t source[PIXELS_PER_CHUNK * 3];
        uint16_t background[PIXELS_PER_CHUNK];
        uint16_t output[PIXELS_PER_CHUNK];

        tft.setSwapBytes(true);
        for (uint16_t row = 0; row < image_h; row++) {
            for (uint16_t col = 0; col < image_w;) {
                const size_t count = min(PIXELS_PER_CHUNK, size_t(image_w - col));
                const size_t bytes = count * bytes_per_pixel;

                if (file.read(source, bytes) != bytes) {
                    return;
                }

                bool needs_background = false;
                bool has_visible_pixels = !has_alpha;

                if (has_alpha) {
                    for (size_t i = 0; i < count; i++) {
                        const uint8_t alpha = source[i * 3 + 2];

                        if (alpha != 255)
                            needs_background = true;

                        if (alpha != 0)
                            has_visible_pixels = true;
                    }
                }

                if (!has_visible_pixels) {
                    col += count;
                    continue;
                }

                if (needs_background) {
                    tft.readRect(x + col, y + row, count, 1, background);
                }

                for (size_t i = 0; i < count; i++) {
                    const size_t offset = i * bytes_per_pixel;

                    const uint16_t foreground =
                        uint16_t(source[offset]) | (uint16_t(source[offset + 1]) << 8);

                    if (!has_alpha) {
                        output[i] = swap565(foreground);
                        continue;
                    }

                    const uint8_t alpha = source[offset + 2];
                    uint16_t result;

                    if (alpha == 255) {
                        result = foreground;
                    } else if (alpha == 0) {
                        result = swap565(background[i]);
                    } else {
                        uint16_t bg = swap565(background[i]);
                        result = blend565(bg, foreground, alpha);
                    }

                    output[i] = swap565(result);
                }

                tft.pushRect(x + col, y + row, count, 1, output);
                col += count;
            }
        }

        tft.setSwapBytes(false);
        dirty = false;
    }

    void update(TouchPointType& touch) override {
        redraw();
        this->dirty = false;
    };
};