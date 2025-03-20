## SSD1306 Display STM32 I2C C++ Library 

**This is small C++ library for monochrome displays based on ssd1306 controller.
Library is written for the STM32 MCU, but may be ported to any other MCU by modifying low-level part of it.**


### MAIN FEATURES:
- Connect up to 3 displays simultaneously
- Update entire display or any part of it independenly - create layouts
- Draw primitives (pixels, lines, squares, circles)
- Draw text (Windows 1251 fonts included)
- Draw GUI primitives (items, progressbars, charts)
- Select menu items (draw arrow near selected item or inverse item color)
- Draw bitmap pictures


### SHORT DESCRIPTION:

This library focuses on:
* updating only the necessary parts of the display, rather than the entire display
* having basic GUI primitives (menu items, progressbars, charts) out of the box


### DETAILED DESCRIPTION:

Library supposes "Display - Layout - Segment" architecture. 
Each display can be assoshiated with up to 10 different LAYOUTS. Each LAYOUT can consist of with up to 10 SEGMENTS.

Segment is graphic part of display. It can be any width, but it must be aligned in height to the display pages.
All segments are located inside LAYOUTS. All layouts share same heap memory. 

By default, after creation, each display assosiated with one default layout that consists of one default segment.
All draw functions are called throught segment class functions.

All this allows independly use segments, update and redraw them.



### CONTENT:

- ssd1306_ll_interface.cpp (.hpp)   - low level part, implements I2C interface to ssd1306. Uses STM32 HAL library
- ssd1306_display.cpp (.hpp)        - main part, implements all draw features
- ssd1306_fonts.cpp (.hpp)          - contains embedded fonts
- ssd1306_bitmaps.cpp (.hpp)        - contains class definition for bitmap pictures
- ssd1306_charts.cpp (.hpp)         - contains graphics charts (only bar chart implemented at the moment)
- ssd1306_tests.cpp (.hpp)          - contains tests and use-cases


### HOW TO USE:

1. Each display requires heap memory allocation for graphics data so make sure that heap size in your project is sufficient.
    You can calculate the memory size as follows: MEM_SZ = DISP_WIDTH * DISP_HEIGTH / 8

2. Include `ssd1306.hpp` into your project and specify HAL Library for your controller in "ssd1306_ll_interface.hpp" file (`#include <stm32g0xx.h>` for example)

3. Create Display object and init it:
   
   ```
   #include "ssd1306_display.hpp"

   SSD1306_Display* display;

   display = SSD1306_Display::create(SSD1306_SCREEN_RESOLUTION::W128xH64, (void *)(&hi2c2), (0x3C << 1));
   display->init(SSD1306_MIRROR_VERT::SSD1306_MIRROR_VERT_ON, SSD1306_MIRROR_HORIZ::SSD1306_MIRROR_HORIZ_ON);
   ```

4. Use library 
   
   * You can use it without any layouts, for example:
    
   ```
   display->write_string(0, ROW1, "Hello World", font16);
   display->update();
   ```
   * Or you can create new layout, split it into segments and work with them independently: 
  
   ```
   test_layout = display->create_layout();
    
   test_title            = test_layout->create_segment(SSD1306_ADDR_MODE::HORIZONTAL, 0,  0, display->WIDTH_PX-1, 1);
   test_verctical_area   = test_layout->create_segment(SSD1306_ADDR_MODE::VERTICAL,   0,  2, 15,                  display->HEIGHT_PG-1);
   test_horizontal_area  = test_layout->create_segment(SSD1306_ADDR_MODE::HORIZONTAL, 16, 2, display->WIDTH_PX-1, display->HEIGHT_PG-1);

   test_title->write_string(0, ROW1, "Layout Test", font16); 
   test_title->update();
   ```

#### NOTE: 
**The library was developed to simplify the prototyping of electronic devices. It is not fully tested and may contain errors**
