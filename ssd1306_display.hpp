#pragma once

#include "ssd1306_ll_interface.hpp"
#include "ssd1306_fonts.hpp"
#include "ssd1306_bitmaps.hpp"


#define SSD1306_PAGE_SIZE 8

enum class SSD1306_SCREEN_RESOLUTION{W128xH64, W128xH32, W64xH48, W64xH32};

enum SSD1306_ROW_COORDINATES {ROW1 = 0, ROW2 = 8, ROW3 = 16, ROW4 = 24, ROW5 = 32, ROW6 = 40, ROW7 = 48, ROW8 = 56};
enum SSD1306_PROW_INDEXES {PROW1 = 0, PROW2 = 1, PROW3 = 2, PROW4 = 3, PROW5 = 4, PROW6 = 5, PROW7 = 6, PROW8 = 7};     

enum class SSD1306_MIRROR_VERT{ SSD1306_MIRROR_VERT_OFF = 0, SSD1306_MIRROR_VERT_ON = 1};
enum class SSD1306_MIRROR_HORIZ{ SSD1306_MIRROR_HORIZ_OFF = 0, SSD1306_MIRROR_HORIZ_ON = 1};
enum class SSD1306_ADDR_MODE{HORIZONTAL = 0, VERTICAL = 1, PAGE = 2};

enum class SSD1306_FADE_FRAMES{F8, F16, F24, F32, F40, F48, F56, F64, F72, F80, F88, F96, F104, F112, F120, F128};


#define SSD1306_COLOR_NON_INV   true
#define SSD1306_COLOR_INV       false

enum class SSD1306_ITEM_SELECT_METHOD{                  // determines image method of selection menu item when flag "selected" equals true
    ARROW,                                              // the selected item is indicated by an arrow
    COLOR                                               // the selected item is indicated by color inversion
};


enum class SEGMENT_UPDATE_MODE{
    ON_DEMAND,                                          // screen updates after user calling "update" functions
    IMMEDIATELY                                         // screen updates immediately after calling "draw" functions (except primitives like "write_char", "write_string", "draw_pixel", "draw_hline", "draw_vline")
};                                                      // some primitive functions have their own implementations of immediate redraw - i.e. - "write_string_now", "write_num_now"





class SSD1306_Display;

class DispSegment
{
    public:
    const uint8_t id;                               // uniq segment id
    const SSD1306_ADDR_MODE addr_mode;

    private:
    SSD1306_Display& disp;
    uint8_t* const gram;                           // pointer to segment graphical memory
    const unsigned segment_sz;

    SEGMENT_UPDATE_MODE upd_mode;  
    SSD1306_ITEM_SELECT_METHOD select_method;                        

    // Segment coordinates
    const uint8_t cs, ce;                           // column start, column end     (x)
    const uint8_t ps, pe;                           // page start, page end         (y)

    public:
    const uint8_t sw;                               // segment width (in px)
    const uint8_t sh;                               // segment heigth (in pg)
    const uint8_t shp;                              // segment heigth (in px)

    uint8_t x, y = 0;                               // Cursor (px)

    public:
    DispSegment(uint8_t segment_id, SSD1306_ADDR_MODE _addr_mode, uint8_t col_start, uint8_t page_start, uint8_t col_end, uint8_t page_end, SSD1306_Display& display, uint8_t* segment_gram_ptr, unsigned segment_size) :
        id(segment_id), 
        addr_mode(_addr_mode), 
        cs(col_start), ps(page_start), ce(col_end), pe(page_end), 
        disp(display), gram(segment_gram_ptr), segment_sz(segment_size), 
        sw(col_end - col_start + 1), sh(page_end - page_start + 1), shp((page_end - page_start + 1)*8),
        upd_mode(SEGMENT_UPDATE_MODE::ON_DEMAND),
        select_method(SSD1306_ITEM_SELECT_METHOD::ARROW){}
    

    void set_segment_update_mode(SEGMENT_UPDATE_MODE mode){upd_mode = mode;}
    void set_segment_update_mode_on_demand(){upd_mode = SEGMENT_UPDATE_MODE::ON_DEMAND;}
    void set_segment_update_mode_immediately(){upd_mode = SEGMENT_UPDATE_MODE::IMMEDIATELY;}
    bool immediate_update_mode_enabled(){return upd_mode == SEGMENT_UPDATE_MODE::IMMEDIATELY;}

    void set_select_method(SSD1306_ITEM_SELECT_METHOD _select_method){select_method = _select_method;}

    void clear(bool color_noinv = true);
    void clear_row(uint8_t y_px, bool color_noinv = true);

    void draw_pixel(uint8_t x_px, uint8_t y_px, bool pix_en = true);

    inline bool set_cursor(uint8_t curs_x, uint8_t curs_y){if(curs_x<sw && curs_y<shp) {x = curs_x; y = curs_y; return true;} return false;}
    inline bool check_cursor(uint8_t curs_x, uint8_t curs_y) {return curs_x<sw && curs_y<shp;}
    inline bool check_font(uint8_t w, uint8_t h) {return ((x+w-1 < sw) && (y+h-1 < shp));}

    bool write_char(char ch, Font &font, bool color_noinv, bool no_interval = false);
    void write_string(uint8_t x_px, uint8_t y_px, const char* str, Font &font, bool color_noinv = true);
    void write_string_now(uint8_t x_px, uint8_t y_px, const char* str, Font &font, bool color_noinv = true);
    void write_num(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv = true);
    void write_num_now(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv = true);

    void clear_font_px(uint8_t x_px, uint8_t y_px, uint8_t sz_px, Font &font, bool color_noinv = true);
    void clear_font_symb(uint8_t x_px, uint8_t y_px, uint8_t sz_symb, Font &font, bool color_noinv = true);
    void clear_font_row(uint8_t y_px, Font &font, bool color_noinv = true);
    uint8_t get_string_size_px(const char* str, Font &font);

    
    void draw_hline(uint8_t x_px, uint8_t y_px, uint8_t w_px, bool color_noinv = true);
    void draw_vline(uint8_t x_px, uint8_t y_px, uint8_t h_px, bool color_noinv = true); 
    void draw_frame(uint8_t x_px, uint8_t y_px, uint8_t w_px, uint8_t h_px, bool color_noinv = true);
    void draw_box(uint8_t x_px, uint8_t y_px, uint8_t w_px, uint8_t h_px,  bool color_noinv = true);

    void draw_line(uint8_t x1_px, uint8_t y1_px, uint8_t x2_px, uint8_t y2_px, bool color_noinv = true);
    void draw_circle(uint8_t x_px,uint8_t y_px,uint8_t r_px, bool color_noinv = true);
    void draw_filled_circle(uint8_t x_px,uint8_t y_px,uint8_t r_px, bool color_noinv = true);
    
    

    void draw_param(uint8_t y_px, const char* str, Font &font, signed p_val, bool selected = false);
    void draw_param(uint8_t y_px, const char* str, Font &font, const char* p_str, bool selected = false);
    void draw_checkbox(uint8_t y_px, const char* str, Font &font, bool checked = false, bool selected = false);


    void select_item(uint8_t y_px, Font &font);
    void deselect_item(uint8_t y_px, Font &font);

    void draw_hbar(uint8_t y_px, unsigned percent, Font &font, bool color_noinv = true);

    void draw_x_tick_labels(uint8_t x_px, uint8_t y_px, uint8_t qnt, uint8_t x_step_px, const char *labels[], Font &font);

    void draw_accum(uint8_t x_px, uint8_t y_px, uint8_t charge);
    void draw_bitmap(uint8_t x_px, uint8_t y_px, Bitmap &bitmap);

    void update();
    void update_row(uint8_t y_px, Font &font);
    void update_part(uint8_t xs_px, uint8_t ys_px, uint8_t xe_px, uint8_t ye_px);
};





class DispLayout
{
    static uint8_t layouts_qnt;                                                     // numer of creates layouts             
    const uint8_t id;                                                               // id of current layout

    SSD1306_Display& disp;
    uint8_t* const gram;
    unsigned gram_available;
    uint8_t*  curr_segment_gram_ptr;

    uint8_t segments_qnt = 0;                                                       // number of segments in layout

    DispLayout(uint8_t layout_id, SSD1306_Display& display, uint8_t* const gram_ptr, unsigned gram_size) : id(layout_id), disp(display), gram(gram_ptr) {gram_available = gram_size; curr_segment_gram_ptr = gram_ptr;}

    public:
    static DispLayout* create_layout(SSD1306_Display& display);

    DispSegment* create_segment(SSD1306_ADDR_MODE addr_mode, uint8_t col_start_px, uint8_t page_start_pg, uint8_t col_end_px, uint8_t page_end_pg);
};






class SSD1306_Display
{
    friend DispLayout* DispLayout::create_layout(SSD1306_Display& display);
    friend class DispSegment;

    public:
    static uint8_t display_qnt;                                                     // number of created displays
    const uint8_t id;                                                               // id of current display

    const uint8_t WIDTH_PX;                                                         // Display width in pixels 
    const uint8_t HEIGHT_PX;                                                        // Display heigth in pixels 
    const uint8_t HEIGHT_PG;                                                        // Display heigth in pages 
   
                 
    private:
    uint8_t* const GRAM_PTR;                                                        // pointer to display graphic memory
    const unsigned GMEM_SZ;                                                         // graphic memory size

    const SSD1306_LL_INTERFACE iface;                                               // interface to ssd1306

    SSD1306_ADDR_MODE addr_mode;                                                    // current address mode

    uint8_t curr_segment_id;                                                        // The id of the segment whose graphic data was sent to the ssd1306
    bool segment_part_updated;                                                      // partial segment update Flag

                                                            
  
    public:
    DispLayout* const ddl;                                                          // default display layout;
    DispSegment* const dds;                                                         // default display segment;
    

    public:
    static SSD1306_Display* create(SSD1306_SCREEN_RESOLUTION resolution, void *interface, uint8_t address);


public:
    SSD1306_Display(uint8_t disp_id, uint8_t w, uint8_t h, void *interface, uint8_t address, uint8_t* gram_ptr) : 
        id(disp_id), 
        WIDTH_PX(w), HEIGHT_PX(h), HEIGHT_PG(h/8), 
        GMEM_SZ((w*h)/8), GRAM_PTR(gram_ptr), 
        iface(interface, address),
        ddl(create_layout()), 
        dds(ddl->create_segment(SSD1306_ADDR_MODE::HORIZONTAL, 0, 0, WIDTH_PX-1, HEIGHT_PG-1)),
        segment_part_updated(true),                                                                      // some fix to make the "update" function work correctly first time after init
        addr_mode(SSD1306_ADDR_MODE::HORIZONTAL){}

    
    void init(SSD1306_MIRROR_VERT v_mirror, SSD1306_MIRROR_HORIZ h_mirror);
    inline DispLayout* create_layout(void){ return DispLayout::create_layout(*this);}

   

    inline void set_segment_update_mode(SEGMENT_UPDATE_MODE mode){dds->set_segment_update_mode(mode);}
    inline void set_segment_update_mode_on_demand(){dds->set_segment_update_mode_on_demand();}
    inline void set_segment_update_mode_immediately(){dds->set_segment_update_mode_immediately();}
    inline bool immediate_update_mode_enabled(){return dds->immediate_update_mode_enabled();}


    inline void clear_screen(void){dds->clear(); update_screen();}
    inline void clear_row(uint8_t y_pg, bool color_noinv = true){dds->clear_row(y_pg, color_noinv);}
    inline void draw_pixel(uint8_t x_px, uint8_t y_px, bool color_noinv = true){dds->draw_pixel(x_px, y_px, color_noinv);}
    inline void set_cursor(uint8_t x, uint8_t y){dds->set_cursor(x, y);}

 
    inline bool write_char(char ch, Font &font, bool color_noinv = true, bool no_interval = false) {return dds->write_char(ch, font, color_noinv, no_interval);}
    inline void write_string(uint8_t x_px, uint8_t y_px, const char* str, Font &font, bool color_noinv = true) {dds->write_string(x_px, y_px, str, font, color_noinv);}
    inline void write_string_now(uint8_t x_px, uint8_t y_px, const char* str, Font &font, bool color_noinv = true) {dds->write_string_now(x_px, y_px, str, font, color_noinv);}
    inline void write_num(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv = true){dds->write_num(x_px, y_px, num,font,color_noinv);}
    inline void write_num_now(uint8_t x_px, uint8_t y_px, signed num, Font &font, bool color_noinv = true){dds->write_num_now(x_px, y_px, num,font,color_noinv);}

    inline void clear_font_px(uint8_t x_px, uint8_t y_px, uint8_t sz_px, Font &font, bool color_noinv = true){dds->clear_font_px(x_px, y_px, sz_px, font, color_noinv);}
    inline void clear_font_symb(uint8_t x_px, uint8_t y_px, uint8_t sz_symb, Font &font, bool color_noinv = true){dds->clear_font_symb(x_px, y_px, sz_symb, font, color_noinv);}
    inline void clear_font_row(uint8_t y_px, Font &font, bool color_noinv = true){dds->clear_font_row(y_px, font, color_noinv);}
    inline uint8_t get_string_size_px(char* str, Font &font){return dds->get_string_size_px(str, font);}

    void draw_hline(uint8_t x_px, uint8_t y_px, uint8_t w_px, bool color_noinv = true){dds->draw_hline(x_px, y_px, w_px, color_noinv);}
    void draw_vline(uint8_t x_px, uint8_t y_px, uint8_t h_px, bool color_noinv = true){dds->draw_vline(x_px, y_px, h_px, color_noinv);} 

    void draw_line(uint8_t x1_px, uint8_t y1_px, uint8_t x2_px, uint8_t y2_px, bool color_noinv = true){dds->draw_line(x1_px, y1_px, x2_px, y2_px, color_noinv);}
    void draw_frame(uint8_t x_px, uint8_t y_px, uint8_t w_px, uint8_t h_px, bool color_noinv = true){dds->draw_frame(x_px, y_px, w_px, h_px, color_noinv);}
    void draw_box(uint8_t x_px, uint8_t y_px, uint8_t w_px, uint8_t h_px, bool color_noinv = true){dds->draw_box(x_px, y_px, w_px, h_px, color_noinv);}

    void draw_circle(uint8_t x_px, uint8_t y_px, uint8_t r_px, bool color_noinv = true){dds->draw_circle(x_px, y_px, r_px, color_noinv);}
    void draw_filled_circle(uint8_t x_px, uint8_t y_px, uint8_t r_px, bool color_noinv = true){dds->draw_filled_circle(x_px, y_px, r_px, color_noinv);}


    void draw_param(uint8_t y_px, const char* str, Font &font, signed p_val, bool selected = false){dds->draw_param(y_px, str, font, p_val, selected);}
    void draw_param(uint8_t y_px, const char* str, Font &font, const char* p_str, bool selected = false){dds->draw_param(y_px, str, font, p_str, selected);}
    void draw_checkbox(uint8_t y_px, const char* str, Font &font, bool checked = false, bool selected = false){dds->draw_checkbox(y_px, str, font, checked, selected);}
    void draw_hbar(uint8_t y_px, unsigned percent, Font &font, bool color_noinv = true){dds->draw_hbar(y_px, percent, font,  color_noinv);}

    void set_select_method(SSD1306_ITEM_SELECT_METHOD _select_method){dds->set_select_method(_select_method);}
    void select_item(uint8_t y_px, Font &font){dds->select_item(y_px, font);}
    void deselect_item(uint8_t y_px, Font &font){dds->deselect_item(y_px, font);}


    void draw_bitmap(uint8_t x_px, uint8_t y_px, Bitmap &bitmap){dds->draw_bitmap(x_px, y_px, bitmap);}


    inline void update_screen(void){dds->update();}
    void update_row(uint8_t y_px, Font &font){dds->update_row(y_px, font);}
    

    
   
    void clear_screen_save_gram(bool color_noinv = true);

    void set_addr_mode(SSD1306_ADDR_MODE addr_mode);
    void set_hv_range(uint8_t x_start_px, uint8_t x_end_px, uint8_t y_start_pg, uint8_t y_end_pg);
    void set_page_range(uint8_t x_start_px,  uint8_t y_start_pg);

    inline void set_invert_color_mode(bool enable)          {(enable == false) ? iface.WriteCommand(0xA6) : iface.WriteCommand(0xA7);}
    inline void set_mirror_vert(SSD1306_MIRROR_VERT v_mirror_mode)  {(v_mirror_mode == SSD1306_MIRROR_VERT::SSD1306_MIRROR_VERT_OFF) ? iface.WriteCommand(0xC0) : iface.WriteCommand(0xC8);}
    inline void set_mirror_horiz(SSD1306_MIRROR_HORIZ h_mirror_mode){(h_mirror_mode == SSD1306_MIRROR_HORIZ::SSD1306_MIRROR_HORIZ_OFF) ? iface.WriteCommand(0xA0) : iface.WriteCommand(0xA1);}

    inline void disable_fade_out(){iface.WriteCommand(0x23); iface.WriteCommand(0x00);}
    inline void fade_out(SSD1306_FADE_FRAMES frames){iface.WriteCommand(0x23); iface.WriteCommand(0x20 | (uint8_t)frames);}
    inline void enable_continious_fade_out(SSD1306_FADE_FRAMES frames){iface.WriteCommand(0x23); iface.WriteCommand(0x30 | (uint8_t)frames);};
  
    inline void on(void){iface.WriteCommand(0xAF);}
    inline void off(void){iface.WriteCommand(0xAE);}

    inline void entire_on(){iface.WriteCommand(0xA5);}
    inline void entire_off(){iface.WriteCommand(0xA4);}

    void set_display_start_line(uint8_t start_line_px);
    void set_display_offset(uint8_t offset);
    void set_contrast(uint8_t contrast);
};