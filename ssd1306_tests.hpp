#pragma once

#include "ssd1306.hpp"


void ssd1306_run_tests(SSD1306_Display* display);

void ssd1306_font_test(SSD1306_Display* display);
void ssd1306_primitives_test(SSD1306_Display* display);
void ssd1306_menu_test11(SSD1306_Display* display);
void ssd1306_menu_test12(SSD1306_Display* display);
void ssd1306_menu_test21(SSD1306_Display* display);
void ssd1306_menu_test22(SSD1306_Display* display);
void ssd1306_chart_test(SSD1306_Display* display);
void ssd1306_layout_test(SSD1306_Display* display);
void ssd1306_bitmap_test(SSD1306_Display* display);
void ssd1306_end_test(SSD1306_Display* display);

void my_test(SSD1306_Display* display);