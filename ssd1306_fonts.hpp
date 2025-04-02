#include "stdint.h"

#define DEFAULT_ASCII 32
#define DIGITS_ONLY 48


class Font
{
    const uint8_t shift;
    const void *data_en;
    const void *data_ru;

    public:
    const uint8_t width;
    const uint8_t height;
    const uint8_t interval;

	
	Font(uint8_t font_width, uint8_t font_heigth, uint8_t font_interval, const void* font_en_data_ptr, const void* font_ru_data_ptr, uint8_t font_shift = DEFAULT_ASCII);

	uint8_t get_symbol_width(char c);
	unsigned get_column(char c, uint8_t col);
};

extern Font font5;
extern Font font8;
extern Font font16;

extern Font font_dig32;
