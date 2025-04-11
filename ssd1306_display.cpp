/**
  ******************************************************************************
  * @brief   SSD1306  high-level driver
  * 
  *   The "Display" class wraps most of the methods of the "DispSegment" class
  *
  *   For simple work with display use "Display" class methods
  *   For complicated work - create layout "DispSegment" instance(s). Call methods of the corresponding entity
  * 
                     ##### How to use this driver #####
  ==============================================================================

  SSD1306_Display* display;

  display = SSD1306_Display::create(SSD1306_SCREEN_RESOLUTION::W128xH64, (void *)(&hi2c2), (0x3C << 1));
  display->init(SSD1306_MIRROR_VERT::SSD1306_MIRROR_VERT_ON, SSD1306_MIRROR_HORIZ::SSD1306_MIRROR_HORIZ_ON);

  display->write_string(0, ROW1, "Hello World", font16);
*/

#include <stdlib.h>
#include <stdio.h>

#include "ssd1306_display.hpp"


// static variable specifies quantity of layouts in the system
uint8_t DispLayout::layouts_qnt = 0;


/**
  * @brief Create new layout
  * 
  * @note Layout is container for several segments. Segments can be added to layout. 
  * @note You can create several layouts with different segment arrangement and switch from one to another. 
  * @note WARNING! All segments of all layouts share same display GRAM memory
  * 
  * @param display                     Display pointer to which this layout will be applied
  * @return                            Returns pointer to new layout object DispLayout* 
*/
DispLayout* DispLayout::create_layout(SSD1306_Display& display){
  return new DispLayout(display.id*100 + (layouts_qnt++)*10, display, display.GRAM_PTR, display.GMEM_SZ);
}


/**
 * @brief Create new display segment
 * 
 * @note All segments share the same memory space and are located in it sequentially
 * @note It is very important not to overlap segments and align them on display pages. Otherwise, unpredictable errors are possible
 * 
 * @param addr_mode                   address mode for this segment SSD1306_ADDR_MODE:: [PAGE, HORIZONTAL, VERTICAL]
 * @param col_start_px                x start of the segment in display coordinates in px
 * @param page_start_pg               y start of the segment in display coordinates in pages
 * @param col_end_px                  x end of the segment in display coordinates in px (end is included in segment)
 * @param page_end_pg                 y end of the segment in display coordinates in pages (end is included in segment)
 * @return                            Return pointer to new DispSegment object
 */
DispSegment* DispLayout::create_segment(SSD1306_ADDR_MODE addr_mode, uint8_t col_start_px, uint8_t page_start_pg, uint8_t col_end_px, uint8_t page_end_pg){
  unsigned segment_size;
  uint8_t* segment_gram_ptr;

  if(segments_qnt == 10)
    while(1);                                                                 // to many segments 

  if(col_start_px > col_end_px || col_end_px >= disp.WIDTH_PX)
    while(1);

  if(page_start_pg > page_end_pg || page_end_pg >= disp.HEIGHT_PG)
    while(1);

  segment_size = (page_end_pg - page_start_pg + 1) * (col_end_px - col_start_px + 1);

  if(segment_size > gram_available)
    while(1);

    segment_gram_ptr = curr_segment_gram_ptr;
    curr_segment_gram_ptr += segment_size;
    gram_available -= segment_size;

  return new DispSegment(id+segments_qnt++, addr_mode, col_start_px, page_start_pg, col_end_px, page_end_pg, disp, segment_gram_ptr, segment_size);
}




/**
* @brief Clear full segment
*
* @param[in] color_noinv              no color inverse (optional, default = true)
*/
void DispSegment::clear(bool color_noinv)
{
  uint8_t mask = color_noinv ? 0x00 : 0xFF;

  for(unsigned i = 0; i < segment_sz; i++)
    gram[i] = mask;
}




/**
* @brief Clear segment row. Row is aligned to ssd1306 hardware rows - pages.
*
* @param[in] y_pg                     row index in pages [0 .. 7]
* @param[in] color_noinv              no color inverse (optional, default = true)
*/
void DispSegment::clear_row(uint8_t y_pg, bool color_noinv)
{
  if(y_pg > SSD1306_PROW_INDEXES::PROW8)
    return;

  uint8_t mask = color_noinv ? 0x00 : 0xFF;

  if(addr_mode == SSD1306_ADDR_MODE::VERTICAL)
  {
    for(unsigned idx = 0; idx < sw; idx++)
      gram[idx*sh + y_pg] = mask;
  }
  else
  {
    for(unsigned idx = 0; idx < sw; idx++)
      gram[y_pg*sw + idx] = mask;
  }
  
  update_part(0, y_pg*SSD1306_PAGE_SIZE, sw-1, y_pg*SSD1306_PAGE_SIZE);
}





/**
* @brief Clears part of the specified display area 
* 
* @param[in] xs_px                    x start area coordinate in px
* @param[in] ys_px                    y start area coordinate in px
* @param[in] xe_px                    x end area coordinate in px
* @param[in] ye_px                    y end area coordinate in px
 */
void DispSegment::clear_part(uint8_t xs_px, uint8_t ys_px, uint8_t xe_px, uint8_t ye_px, bool color_noinv)
{
 
  if(addr_mode == SSD1306_ADDR_MODE::PAGE)
  {
    if((xs_px > xe_px || xs_px > sw || xe_px > sw) || (ys_px > SSD1306_PROW_INDEXES::PROW8))
      while(1);

    uint8_t  mask = color_noinv ? ~((0xFF << (ys_px%8)) & (0xFF >> (7-(ye_px%8)))) : ((0xFF << (ys_px%8)) & (0xFF >> (7-(ye_px%8))));

    for (unsigned i = xs_px; i <= xe_px; i++)
      gram[i] &= mask;
  }
  else
  {
    if((xs_px > xe_px || xs_px > sw || xe_px > sw) || (ys_px > ye_px || ys_px > shp || ye_px > shp))
      while(1);

    uint8_t start_page = ((ys_px+1)>>3) + (((ys_px+1)%8)!=0) - 1;
    uint8_t end_page = ((ye_px+1)>>3) + (((ye_px+1)%8)!=0) - 1;
    uint8_t mask;
    
    if(addr_mode == SSD1306_ADDR_MODE::HORIZONTAL)
    {

      if(start_page == end_page)
      {
        mask = color_noinv ? ~((0xFF << (ys_px%8)) & (0xFF >> (7-(ye_px%8)))) : ((0xFF << (ys_px%8)) & (0xFF >> (7-(ye_px%8))));

        for(uint8_t col = 0; col < xe_px - xs_px + 1; col++)
          gram[start_page * sw + xs_px + col] &= mask;
      }
      else 
      {
        uint8_t y_border;

        //first page
        y_border = 8-(ys_px % 8);
        mask = color_noinv ? 0xFF >> y_border : ~(0xFF >> y_border);
       

        for(uint8_t col = 0; col < xe_px - xs_px + 1; col++)
          gram[start_page * sw + xs_px + col] &= mask;


        //middle pages (if exist)
        mask = color_noinv ? 0x00 : 0xFF;
        for(uint8_t p = start_page+1; p < end_page; p++)
          for(uint8_t col = 0; col < xe_px - xs_px + 1; col++)
            gram[p * sw + xs_px + col] = mask;
        

        //last page
        y_border = ye_px % 8 + 1;
        mask = color_noinv ? 0xFF << y_border : ~(0xFF << y_border);

        for(uint8_t col = 0; col < xe_px - xs_px + 1; col++)
          gram[end_page * sw + xs_px + col] &= mask;
      }
    }
    else
    {
      while(1);
      // for(uint8_t col = range_xs; col <= range_xe; col++)
      //   disp.iface.WriteData(&gram[col*sh + range_ys], range_ye-range_ys+1);
    }
  }
}





/**
* @brief Draw one pixel in the screenbuffer
*
* @param[in] x_px                     x coordinate in px
* @param[in] y_px                     y coordinate in px
* @param[in] px_en                    true - pixel  is on, false - pixel is off
*/
void DispSegment::draw_pixel(uint8_t x_px, uint8_t y_px, bool px_en) 
{
  if(x_px > sw-1 || y_px > shp-1) 
    return;

  switch (addr_mode)
  {
  case SSD1306_ADDR_MODE::HORIZONTAL:
    px_en ? (gram[x_px + (y_px >> 3) * sw] |= (1 << (y_px % 8))) : (gram[x_px + (y_px >> 3) * sw] &= ~(1 << (y_px % 8)));
    break;
  
  case SSD1306_ADDR_MODE::VERTICAL:
    px_en ? (gram[x_px * sh + (y_px >> 3)] |= 1 << (y_px % 8)) : (gram[x_px * sh + (y_px >> 3)] &= ~(1 << (y_px % 8)));
    break;

  case SSD1306_ADDR_MODE::PAGE:
    px_en ? (gram[x_px + (y_px >> 3) * sw] |= 1 << (y_px % 8)) : (gram[x_px + (y_px >> 3) * sw] &= ~(1 << (y_px % 8)));
    break;
  }  
}




/**
 * @brief Write char to the current place in display segment
 * 
 * @param ch                          char to be writed
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 * @param no_interval                 (optional, def = false) if "true", then it does not add intercharacter spacing
 */
bool DispSegment::write_char(char ch, Font &font, bool color_noinv, bool no_interval)
{
  unsigned symbol;
  uint8_t width = font.get_symbol_width(ch);

  if(text_vertical_mode == false)
  {
    if(!check_font(width, font.height))                                       
    return false;                                                                      // Not enouth space to write char

    for(unsigned i = 0; i < width; i++) 
    {
      symbol = font.get_column(ch, i);
      for(unsigned j = 0; j < font.height; j++)
        ((symbol >> j) & 0x01) ? draw_pixel(x + i, (y + j), color_noinv) : draw_pixel(x + i, (y + j), !color_noinv);
    }
    
    x += width;

    if(no_interval)
      return true;

    for(unsigned i = 0; i < font.interval; i++) 
      for(unsigned j = 0; j < font.height; j++)
        draw_pixel(x + i, (y + j), !color_noinv);

        
      // The current space is now taken
      x += font.interval;
      return true;
  }
  else
  {

    if(!check_vfont(width, font.height))                                       
      return false;                                                                      // Not enouth space to write char

    for(unsigned i = 0; i < width; i++) 
    {
      symbol = font.get_column(ch, i);
      for(unsigned j = 0; j < font.height; j++)
        ((symbol >> j) & 0x01) ? draw_pixel(x + j, (y - i), color_noinv) : draw_pixel(x + j, (y - i), !color_noinv);
    }
    
    y -= width;

    if(no_interval)
      return true;

    for(unsigned i = 0; i < font.interval; i++) 
      for(unsigned j = 0; j < font.height; j++)
        draw_pixel(x + j, (y + i), !color_noinv);

    // The current space is now taken
    y -= font.interval;
    return true;
  }
}




/**
 * @brief Writes string to the specified place in display segment
 * 
 * @param x_px                        x coordinate in px
 * @param y_px                        y coordinate in px
 * @param str                         string to be displayed
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::write_string(uint8_t x_px, uint8_t y_px, const char* str, Font &font, bool color_noinv)
{
  if(!set_cursor(x_px, y_px))                      // Bad coordinates
    return;

  while (*str)                              // Write until null-byte
  {
      if (!write_char(*str, font, color_noinv)) 
          return;                           // Char could not be written
      
      str++;                                // Next char
  }
}




/**
 * @brief Writes string to the specified place in display segment. Shows it IMMEDIATLY
 * 
 * @param x_px                        x coordinate in px
 * @param y_px                        y coordinate in px
 * @param str                         string to be displayed
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::write_string_now(uint8_t x_px, uint8_t y_px, const char* str, Font &font, bool color_noinv)
{
  write_string(x_px, y_px, str, font, color_noinv);
  update_part(x_px, y_px, x_px + get_string_size_px(str, font) - 1, y_px + font.height - 1);
}




/**
 * @brief Write numerical to the specified place in display segment
 * 
 * @param x_px                        x coordinate in px
 * @param y_px                        y coordinate in px
 * @param num                         num to be displayed
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::write_num(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv)
{
  char sbuf[10];
  sprintf(sbuf, "%d", num);

  write_string(x_px, y_px, sbuf, font, color_noinv);
}





/**
 * @brief Write numerical with space or minus sign to the specified place in display segment
 * 
 * @param x_px                        x coordinate in px
 * @param y_px                        y coordinate in px
 * @param num                         num to be displayed
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::write_num_sign(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv)
{
  char sbuf[10];
  sprintf(sbuf, "% d", num);

  write_string(x_px, y_px, sbuf, font, color_noinv);
}




/**
 * @brief Write numerical to the specified place in display segment. Shows it IMMEDIATLY
 * 
 * @param x_px                        x coordinate in px
 * @param y_px                        y coordinate in px
 * @param num                         num to be displayed
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::write_num_now(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv)
{
  char sbuf[10];
  sprintf(sbuf, "%d", num);

  write_string_now(x_px, y_px, sbuf, font, color_noinv);
}




/**
 * @brief Clears the specified font by a certain number of pixels horizontally
 * 
 * @param x_px                        x coordinate in px
 * @param y_px                        y coordinate in px
 * @param sz_px                       amount of pixels to be cleared
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::clear_font_px(uint8_t x_px, uint8_t y_px, uint8_t sz_px, Font &font, bool color_noinv)
{  
  if(!set_cursor(x_px, y_px))                      // Bad coordinates
    return;
   
   for(unsigned i = x_px; i < x_px + sz_px; i++)
     for(unsigned j = y_px; j < y_px + font.height; j++)
       draw_pixel(i, j, !color_noinv);
}




/**
 * @brief Clears symbol of the specified font
 * 
 * @param x_px                        x coordinate of symbol in px
 * @param y_px                        y coordinate of symbol in px
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::clear_font_symb(uint8_t x_px, uint8_t y_px, uint8_t sz_symb, Font &font, bool color_noinv){
  clear_font_px(x_px, y_px, sz_symb*(font.width + font.interval)-font.interval, font, color_noinv);
}




/**
 * @brief Clears string row of the specified font
 * 
 * @param y_px                        row coordinates in px
 * @param font                        font that determines clear height
 * @param color_noinv                 (optional, def = true) determines color no inversion
 */
void DispSegment::clear_font_row(uint8_t y_px, Font &font, bool color_noinv){
  clear_font_px(0, y_px, sw, font, color_noinv);
}


            

/**
* @brief Returns size of the specified string, written in specified font, in pixels
* @param[in] str                       string
* @param[in] font                      font
*/
uint8_t DispSegment::get_string_size_px(const char* str, Font &font)
{
  uint8_t size_px = 0;
  
  while (*str) 
    size_px += font.get_symbol_width(*str++) + font.interval;
  
  return size_px - font.interval;
}





/**
* @brief Returns size of the specified number, written in specified font, in pixels
* @param[in] num                       number
* @param[in] font                      font
*/
uint8_t DispSegment::get_num_string_size_px(signed num, Font &font)
{
  char sbuf[10];
  char* sbuf_ptr = sbuf;
  sprintf(sbuf, "%d", num);

  uint8_t size_px = 0;
  
  while (*sbuf_ptr) 
    size_px += font.get_symbol_width(*sbuf_ptr++) + font.interval;
  
  return size_px - font.interval;
}





/**
* @brief Draws horizontal line. Coordinates are correspond to left end of line
* @param[in] x_px                      x coordinate of the left end of line in px
* @param[in] y_px                      y coordinate of the left end of line in px
* @param[in] w_px                      line width in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_hline(uint8_t x_px, uint8_t y_px, uint8_t w_px, bool color_noinv) 
{
  while(w_px--)
    draw_pixel(x_px++, y_px, color_noinv);
}




/**
* @brief Draws vertical line. Coordinates are correspond to upper end of line
* @param[in] x_px                      x coordinate of the upper end of line in px
* @param[in] y_px                      y coordinate of the upper end of line in px
* @param[in] h_px                      line height in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_vline(uint8_t x_px, uint8_t y_px, uint8_t h_px, bool color_noinv) 
{
  while(h_px--)
    draw_pixel(x_px, y_px++, color_noinv);
}




/**
* @brief Draws any line. Pixel positions calculated using Bresenham's algorithm 
* @param[in] x1_px                     x coordinate of the 1-st end of line in px
* @param[in] y1_px                     y coordinate of the 1-st end of line in px
* @param[in] x2_px                     x coordinate of the 2-nd end of line in px
* @param[in] y2_px                     y coordinate of the 2-nd end of line in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_line(uint8_t x1_px, uint8_t y1_px, uint8_t x2_px, uint8_t y2_px, bool color_noinv) {
  int32_t deltaX = abs(x2_px - x1_px);
  int32_t deltaY = abs(y2_px - y1_px);
  int32_t signX = ((x1_px < x2_px) ? 1 : -1);
  int32_t signY = ((y1_px < y2_px) ? 1 : -1);
  int32_t error = deltaX - deltaY;
  int32_t error2;
  
  draw_pixel(x2_px, y2_px, color_noinv);

  while((x1_px != x2_px) || (y1_px != y2_px)) {
      draw_pixel(x1_px, y1_px, color_noinv);
      error2 = error * 2;
      if(error2 > -deltaY) {
          error -= deltaY;
          x1_px += signX;
      }
      
      if(error2 < deltaX) {
          error += deltaX;
          y1_px += signY;
      }
  }

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
      update_part(x1_px, y1_px, x2_px, y2_px);
}




/**
* @brief Draws a rectangle. Coordinates are correspond to left upper corner
* @param[in] x_px                      x coordinate of the left upper corner in px
* @param[in] y_px                      y coordinate of the left upper corner in px
* @param[in] w_px                      width of the rectangle in px
* @param[in] h_px                      height of the rectangle in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_frame(uint8_t x_px, uint8_t y_px, uint8_t w_px, uint8_t h_px, bool color_noinv) 
{
    draw_hline(x_px, y_px, w_px, color_noinv);
    draw_hline(x_px, y_px + (h_px-1), w_px, color_noinv);
    draw_vline(x_px, y_px, h_px, color_noinv);
    draw_vline(x_px + (w_px-1), y_px, h_px, color_noinv);

    if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
      update_part(x_px, y_px, x_px + w_px - 1, y_px + h_px - 1);
}




/**
* @brief Draws a filled rectangle. Coordinates are correspond to left upper corner
* @param[in] x_px                      x coordinate of the left upper corner in px
* @param[in] y_px                      y coordinate of the left upper corner in px
* @param[in] w_px                      width of the rectangle in px
* @param[in] h_px                      height of the rectangle in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_box(uint8_t x_px, uint8_t y_px, uint8_t w_px, uint8_t h_px, bool color_noinv) 
{
  while(h_px--)
    draw_hline(x_px, y_px++, w_px, color_noinv);

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
      update_part(x_px, y_px, x_px + w_px - 1, y_px + h_px - 1);
}




/**
* @brief Draws circle. Coordinates are center of the circle. Pixel positions calculated using Bresenham's algorithm 
* @param[in] x_px                      x coordinate of the center in px
* @param[in] y_px                      y coordinate of the center in px
* @param[in] r_px                      radius in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_circle(uint8_t x_px, uint8_t y_px, uint8_t r_px, bool color_noinv) {
    int32_t x = -r_px;
    int32_t y = 0;
    int32_t err = 2 - 2 * r_px;
    int32_t e2;

    if (x_px >= sw || y_px >= shp) 
        return;

    do {
        draw_pixel(x_px - x, y_px + y, color_noinv);
        draw_pixel(x_px + x, y_px + y, color_noinv);
        draw_pixel(x_px + x, y_px - y, color_noinv);
        draw_pixel(x_px - x, y_px - y, color_noinv);
        e2 = err;

        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
      update_part(x_px - r_px, y_px - r_px, x_px + r_px, y_px + r_px);
}




/**
* @brief Draws filled circle. Coordinates are center of the circle. Pixel positions calculated using Bresenham's algorithm 
* @param[in] x_px                      x coordinate of the center in px
* @param[in] y_px                      y coordinate of the center in px
* @param[in] r_px                      radius in px
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_filled_circle(uint8_t x_px, uint8_t y_px, uint8_t r_px, bool color_noinv) {
    int32_t x = -r_px;
    int32_t y = 0;
    int32_t err = 2 - 2 * r_px;
    int32_t e2;

    if (x_px >= sw || y_px >= shp) 
        return;

    do {
        for (uint8_t _y = (y_px + y); _y >= (y_px - y); _y--) {
            for (uint8_t _x = (x_px - x); _x >= (x_px + x); _x--) {
                draw_pixel(_x, _y, color_noinv);
            }
        }

        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if (-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
      update_part(x_px - r_px, y_px - r_px, x_px + r_px, y_px + r_px);
}




/**
* @brief Draws numeric parameter, for example: "speed: 90". Start is always at the beginning of the segment
* @param[in] y_px                     y coordinate in px
* @param[in] str                      parameter name 
* @param[in] font                     font that determines checkbox height
* @param[in] p_val                    parameter numeric value 
* @param[in] selected                 (optional, def = false) determines if this row is selected (highlighted with a sign or color)
*/
void DispSegment::draw_param(uint8_t y_px, const char* str, Font &font, signed p_val, bool selected){
  char sbuf[10];
  sprintf(sbuf, "%d", p_val);

  if(select_method == SSD1306_ITEM_SELECT_METHOD::ARROW)
  {
    clear_font_px(0, y_px, sw, font, SSD1306_COLOR_NON_INV);
    if(selected)
      write_string(0, y_px, ">", font, SSD1306_COLOR_NON_INV);
    write_string(font.width+font.interval, y_px, str, font, SSD1306_COLOR_NON_INV);

    selected = SSD1306_COLOR_NON_INV;
  }
  else
  {
    clear_font_px(0, y_px, sw, font, selected);
    write_string(1, y_px, str, font, selected);
  }

  write_string(sw - get_string_size_px(sbuf, font) - 1, y_px, sbuf, font, selected);

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Draws string parameter, for example: "show speed: off". Start is always at the beginning of the segment
* @param[in] y_px                     y coordinate in px
* @param[in] str                      parameter name (show speed:)
* @param[in] font                     font that determines checkbox height
* @param[in] p_str                    parameter string value (off)
* @param[in] selected                 (optional, def = false) determines if this row is selected (highlighted with a sign or color)
*/
void DispSegment::draw_param(uint8_t y_px, const char* str, Font &font, const char* p_str, bool selected)
{  
  if(select_method == SSD1306_ITEM_SELECT_METHOD::ARROW)
  {
    clear_font_px(0, y_px, sw, font, SSD1306_COLOR_NON_INV);
    if(selected)
      write_string(0, y_px, ">", font, SSD1306_COLOR_NON_INV);
    write_string(font.width+font.interval, y_px, str, font, SSD1306_COLOR_NON_INV);
    write_string(sw - get_string_size_px(p_str, font), y_px, p_str, font, SSD1306_COLOR_NON_INV);
  }
  else
  {
    clear_font_px(0, y_px, sw, font, !selected);

    write_string(1, y_px, str, font, !selected);
    write_string(sw - get_string_size_px(p_str, font) - 1, y_px, p_str, font, !selected);
  }

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Draws checkbox. Start is always at the beginning of the segment
* @param[in] y_px                     y coordinate in px
* @param[in] str                      checkbox title
* @param[in] font                     font that determines checkbox height
* @param[in] checked                  (optional, def = false) state of checkbox
* @param[in] selected                 (optional, def = false) determines if this row is selected (highlighted with a sign or color)
*/
void DispSegment::draw_checkbox(uint8_t y_px, const  char* str, Font &font, bool checked, bool selected)
{
  uint8_t color_noinv = select_method == SSD1306_ITEM_SELECT_METHOD::ARROW ? true : !selected;

  if(select_method == SSD1306_ITEM_SELECT_METHOD::ARROW)
  {
    clear_font_px(0, y_px, sw, font, SSD1306_COLOR_NON_INV);
    if(selected)
      write_string(0, y_px, ">", font, SSD1306_COLOR_NON_INV);
    write_string(font.width+font.interval, y_px, str, font, SSD1306_COLOR_NON_INV);
  }
  else
  {
    clear_font_row(y_px, font, !selected);
    write_string(1, y_px, str, font, !selected);
  }
 
  if(font.height == font8.height)
  {
    if(color_noinv)
    {
      draw_frame(sw-8, y_px, 8, 8, color_noinv);
      checked ? draw_box(sw-6, y_px+2, 4, 4, color_noinv) : draw_box(sw-6, y_px+2, 4, 4, !color_noinv);
    }
    else
      checked ? draw_frame(sw-7, y_px+1, 6, 6, color_noinv) : draw_box(sw-7, y_px+1, 6, 6, color_noinv);
  }

  if(font.height == font16.height)
  {
    draw_frame(sw-11, y_px+3, 10, 10, color_noinv);
    checked ? draw_box(sw-9, y_px+5, 6, 6, color_noinv) : draw_frame(sw-9, y_px+5, 6, 6, !color_noinv);
  }

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Print row select symbol 
* @param[in] y_px                     y coordinate in px
* @param[in] font                     font that determines height
*/
void DispSegment::select_item(uint8_t y_px, Font &font)
{
  set_cursor(0, y_px);
  write_char('>', font, SSD1306_COLOR_NON_INV, true);

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Hide row select symbol 
* @param[in] y_px                         y coordinate in px
* @param[in] font                      font that determines height
*/
void DispSegment::deselect_item(uint8_t y_px, Font &font)
{
  clear_font_px(0, y, font.width, font, SSD1306_COLOR_NON_INV);

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Draws horizontal progress bar. Start of the progress bar is always at the beginning of the segment.
* @param[in] y_px                      y coordinate in px
* @param[in] percent                   percent of progress bar [0 .. 100]
* @param[in] font                      font that determines progress bar height
* @param[in] color_noinv               (optional, def = true) determines color no inversion
*/
void DispSegment::draw_hbar(uint8_t y_px, unsigned percent, Font &font, bool color_noinv)
{
  if(percent > 100)
    percent = 100;

  uint8_t scale_length_px = sw - (font.width+font.interval)*3 - 4;
  uint8_t percent_px = (percent * scale_length_px) / 100;
  

  clear_font_px(0, y_px, sw, font);

  draw_frame(0, y_px, scale_length_px + 4, font.height);

  draw_box(2, y_px+2, percent_px, font.height - 4, color_noinv);
  draw_box(2+percent_px, y_px+2, scale_length_px-percent_px, font.height - 4, !color_noinv);

  set_cursor(scale_length_px + 4 + font.interval, y);

  if(percent < 100)
  {
      write_char(percent/10 + 48, font, color_noinv);
      write_char(percent%10 + 48, font, color_noinv);
      write_char('%', font, color_noinv);
  }
  else
  {
      write_char('1', font, color_noinv);
      write_char('0', font, color_noinv);
      write_char('0', font, color_noinv);
  }

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Draws horizontal tick label 
* @param[in] x_px                     x start coordinate of tick label in px
* @param[in] y_px                     y coordinate of tick label in px
* @param[in] qnt                      quantity of tick names
* @param[in] x_step_px                x step between ticks in px
* @param[in] labels                   array of tick names 
* @param[in] font                     font that determines progress bar height
*/
void DispSegment::draw_x_tick_labels(uint8_t x_px, uint8_t y_px, uint8_t qnt, uint8_t x_step_px, const char *labels[], Font &font)
{
  for(uint8_t i=0; i<qnt; i++)
    write_string(x_px + (font.width + x_step_px)*i, y_px, *labels++, font);

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_row(y_px, font);
}




/**
* @brief Draws accum bitmap. Coordinates correspond to upper left corner of the bitmap.
* @param[in] x_px                     x coordinate in px
* @param[in] y_px                     y coordinate in px
* @param[in] charge                   index corresponding to accum charge level image [0 .. 4]
*/
void DispSegment::draw_accum(uint8_t x_px, uint8_t y_px, uint8_t charge)
{
  if(charge < 5)
    draw_bitmap(x_px, y_px, accum[charge]);
}


  

/**
* @brief Draws a bitmap. Coordinates correspond to upper left corner of the bitmap.
* @param[in] x_px                        x coordinate in px
* @param[in] y_px                        y coordinate in px
* @param[in] bitmap                      Bitmap object
*/
void DispSegment::draw_bitmap(uint8_t x_px, uint8_t y_px, Bitmap &bitmap)
{
  unsigned idx = 0;
 
  for(uint8_t dy = 0; dy < bitmap.h; dy++)
  {
    for(uint8_t dx = 0; dx < (bitmap.w >> 3); dx++)
    {
      for(uint8_t b=0; b<8; b++)
        draw_pixel(x_px + (dx << 3) + b, y_px + dy, bitmap.data[idx]>>(7-b) & 0x01);
      idx++;
    }

    if(bitmap.w % 8)
    {
      for(uint8_t b=0; b < (bitmap.w % 8); b++)
        draw_pixel(x_px + ((bitmap.w >> 3) << 3) + b, y_px + dy, (bitmap.data[idx]>>(7-b) & 0x01));
      idx++;
    }
  }

  if(upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY)
    update_part(x_px, y_px, x_px + (bitmap.w-1), y_px + (bitmap.h-1));
}
   
    


/**
* @brief Redraws full segment area
*/
void DispSegment::update(void)
{
  if(disp.curr_segment_id != id || disp.segment_part_updated)
  {
    disp.segment_part_updated = false;
    disp.set_addr_mode(addr_mode);

    if(addr_mode == SSD1306_ADDR_MODE::PAGE)
      disp.set_page_range(cs, ps);
    else
      disp.set_hv_range(cs, ce, ps, pe);

      disp.curr_segment_id = id;
  }

  disp.iface.WriteData(gram, segment_sz);
}




/**
* @brief Updates full row of the segment. The height of the updated row is determined by font parameter.
* @param[in] y_px                     y coordinate of the row in px
* @param[in] font                     font that determines row height
*/
void DispSegment::update_row(uint8_t y_px, Font &font){
  update_part(0, y_px, sw-1, y_px + font.height-1);
}








/**
* @brief Redraws the specified display area
* @param[in] xs_px                    x start area coordinate in px
* @param[in] ys_px                    y start area coordinate in px
* @param[in] xe_px                    x end area coordinate in px
* @param[in] ye_px                    y end area coordinate in px
*/
void DispSegment::update_part(uint8_t xs_px, uint8_t ys_px, uint8_t xe_px, uint8_t ye_px)
{  
  disp.segment_part_updated = true;
  
  if(disp.curr_segment_id != id)
  {
    disp.set_addr_mode(addr_mode);
    disp.curr_segment_id = id;
  }
  
  if(addr_mode == SSD1306_ADDR_MODE::PAGE)
  {
      disp.set_page_range(cs+xs_px, ps);
      disp.iface.WriteData(gram+xs_px, xe_px-xs_px+1);
  }
  else
  {
    uint8_t range_xs, range_xe, range_ys, range_ye;

    range_xs = cs+xs_px;
    range_xe = cs+xe_px;
    range_ys = ps + ((ys_px+1)>>3) + (((ys_px+1)%8)!=0) -1;
    range_ye = ps + ((ye_px+1)>>3) + (((ye_px+1)%8)!=0) -1;
      
    disp.set_hv_range(range_xs, range_xe, range_ys, range_ye);
    
    if(addr_mode == SSD1306_ADDR_MODE::HORIZONTAL)
    {
      for(uint8_t pg = range_ys; pg<=range_ye; pg++)
        disp.iface.WriteData(&gram[(pg-ps)*sw + xs_px-cs], xe_px-xs_px+1);
    }
    else
    {
      for(uint8_t col = range_xs; col <= range_xe; col++)
        disp.iface.WriteData(&gram[col*sh + range_ys], range_ye-range_ys+1);
    }
  }
}




// static class field that specifies the number of displays in the system
uint8_t SSD1306_Display::display_qnt = 0;

/**
 * @brief Create new Display object corresponds to physical display
 * 
 * @param resolution                  resolution of different displays - SSD1306_SCREEN_RESOLUTION:: [W128xH64, W128xH32, W64xH48, W64xH32]
 * @param interface                   hardware intetrface must be instance of SSD1306_LL_INTERFACE class
 * @param address                     display address. For I2C interface always equals (0x3C << 1)
 * @return                            pointer to new instance of SSD1306_Display
 * 
 * @note Display instance have its own default layout with one horizontal addressed segment. 
 * @note You can call all segment methods throught display object. This is made for simplicity
 */
SSD1306_Display* SSD1306_Display::create(SSD1306_SCREEN_RESOLUTION resolution, void *interface, uint8_t address)
{
    uint8_t* gram_ptr;
    uint8_t w;
    uint8_t h;

    switch(resolution)
    {
        case SSD1306_SCREEN_RESOLUTION::W128xH64: 
            w = 128;
            h = 64;
            break;

        case SSD1306_SCREEN_RESOLUTION::W128xH32:
            w = 128;
            h = 32;
            break;

        case SSD1306_SCREEN_RESOLUTION::W64xH48:
            w = 64;
            h = 48;
            break;

        case SSD1306_SCREEN_RESOLUTION::W64xH32:
            w = 64;
            h = 32;
            break;
    }

    gram_ptr = (uint8_t*)malloc((w * h) / 8);
    if(gram_ptr == 0)   while(1);

    return new SSD1306_Display(display_qnt++, 128, 64, (void*)interface, address, gram_ptr);
}




/**
 * @brief Display init function. Set mirrowing, contrast, mult-x ratio, COM config, DC-DC, clocks, default address mode
 * 
 * @param v_mirror                    SSD1306_MIRROR_VERT:: [SSD1306_MIRROR_VERT_OFF, SSD1306_MIRROR_VERT_ON]
 * @param h_mirror                    SSD1306_MIRROR_HORIZ::  [SSD1306_MIRROR_HORIZ_OFF, SSD1306_MIRROR_HORIZ_ON]
 */
void SSD1306_Display::init(SSD1306_MIRROR_VERT v_mirror, SSD1306_MIRROR_HORIZ h_mirror)
{
  iface.reset();  
  off();

  set_mirror_vert(v_mirror);
  set_mirror_horiz(h_mirror);

  set_contrast(0xFF);

  iface.WriteCommand(0xA8); //--set multiplex ratio(1 to 64) seems it works like "enabling" rows
  iface.WriteCommand(HEIGHT_PX - 1); 

  iface.WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
  iface.WriteCommand(0xF0); //--set divide ratio


  iface.WriteCommand(0xDA); //--set com pins hardware configuration - CHECK  default - 01
  if (HEIGHT_PX == 32)
    iface.WriteCommand(0x02); 
  else
    iface.WriteCommand(0x12);


  iface.WriteCommand(0xDB); //--set vcomh
  iface.WriteCommand(0x20); //0x20,0.77xVcc

  iface.WriteCommand(0x8D); //--set DC-DC enable
  iface.WriteCommand(0x14); //
  on();                     //--turn on SSD1306 panel
  
  //Set default addresssing mode
  iface.WriteCommand(0x20); 
  iface.WriteCommand((uint8_t)SSD1306_ADDR_MODE::HORIZONTAL);
}




/**
 * @brief Set addressing mode: Page, Horizontal or Vertical
 * @param[in] new_addr_mode           SSD1306_ADDR_MODE::   [PAGE, HORIZONTAL, VERTICAL]
*/
void SSD1306_Display::set_addr_mode(SSD1306_ADDR_MODE new_addr_mode)
{
  if(addr_mode == new_addr_mode)
    return;

  addr_mode = new_addr_mode;

  iface.WriteCommand(0x20); 
  iface.WriteCommand((uint8_t)addr_mode);
}




/**
 * @brief Sets column start and end address & page start and end address in Horizontal or Vertical addressing mode
 * @param[in] x_start_px              column start address  [0 .. 127]
 * @param[in] x_end_px                column end address  [0 .. 127]
 * @param[in] y_start_pg              page start Address  [0 .. 7]
 * @param[in] y_end_pg                page end Address  [0 .. 7]
*/
void SSD1306_Display::set_hv_range(uint8_t x_start_px, uint8_t x_end_px, uint8_t y_start_pg, uint8_t y_end_pg)
{
  if(x_start_px > 127 || x_end_px > 127 || y_start_pg > 7 || y_end_pg > 7)
    while(1);

  iface.WriteCommand(0x21); // set column address
  iface.WriteCommand(x_start_px);
  iface.WriteCommand(x_end_px);

  iface.WriteCommand(0x22); // set Page address
  iface.WriteCommand(y_start_pg);
  iface.WriteCommand(y_end_pg);
}




/**
 * @brief Sets column start & page start address in Page addressing mode
 * @param[in] x_start_px              column start address [0 .. 127]
 * @param[in] y_start_pg              page start Address [0 .. 7] 
*/
void SSD1306_Display::set_page_range(uint8_t x_start_px,  uint8_t y_start_pg)
{
  if(x_start_px > 127 || y_start_pg > 7)
    while(1);

  iface.WriteCommand(0xB0 + y_start_pg);

  iface.WriteCommand(0x21); // set column address
  iface.WriteCommand(0);
  iface.WriteCommand(127);
  
  iface.WriteCommand(x_start_px & 0x0F);
  iface.WriteCommand(((x_start_px >> 4) & 0x0F) | 0x10);
}




/**
 * @brief Clears display throught DMA without changing internal gram
 * @param[in] color_noinv             (optional, def = true) determines color no inversion
 */
void SSD1306_Display::clear_screen_save_gram(bool color_noinv)
{
  curr_segment_id = 5;
  addr_mode = SSD1306_ADDR_MODE::HORIZONTAL;

  set_addr_mode(addr_mode);
  set_hv_range(0, WIDTH_PX-1, 0 , HEIGHT_PG-1);

  color_noinv ? iface.FillMemory(0x00, GMEM_SZ) : iface.FillMemory(0xFF, GMEM_SZ);
}




/**
 * @brief Sets Display Start Line
 * @param[in] start_line_px           starting address value in px [0 .. 63] 
*/
void SSD1306_Display::set_display_start_line(uint8_t start_line_px){
  if(start_line_px < HEIGHT_PX)
    iface.WriteCommand(0x40 + start_line_px);
}




/**
 * @brief Sets Display Offset
 * @param[in] offset                  vertical shift by COM  [0 .. 63]
*/
void SSD1306_Display::set_display_offset(uint8_t offset){
  if(offset >= 64)
    return;

  iface.WriteCommand(0xD3);
  iface.WriteCommand(offset);
}




 /**
 * @brief Sets the contrast of the display
 * 
 * @param[in] contrast                contrast to set [0x00 .. 0xFF]
 * @note                              Contrast increases as the value increases
 * @note                              RESET value = 0x7F
 */
void SSD1306_Display::set_contrast(uint8_t contrast){
    iface.WriteCommand(0x81);
    iface.WriteCommand(contrast);
}