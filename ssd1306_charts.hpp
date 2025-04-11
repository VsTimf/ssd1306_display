#pragma once

#include "ssd1306_display.hpp"

class BarChart
{
    DispSegment* ds;        // pointer to display segment
    signed* lv;             // last charts values

    uint8_t x, y;           // base coordinates (px)
    uint8_t w, sp, h;       // width of single chart (px), space between charts (px), max height (px)

    uint8_t cnum;           // charts num
    signed imin, imax;      // min & max values of charts input values




    public:

    BarChart();
    BarChart(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px);
    BarChart(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px, signed input_min, signed input_max);
    BarChart(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max);
    BarChart(DispSegment* segment, signed* last_vals, uint8_t x_px, uint8_t y_px, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max);
   


    void init(DispSegment* segment, signed* last_vals, uint8_t x_px, uint8_t y_px, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max);

    void init_with_fit_into_segment(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px);
    void init_with_fit_into_segment(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px, signed input_min, signed input_max);

    inline void set_input_range(signed input_min, signed input_max){imin = input_min; imax = input_max;}

    void show(signed* nv, bool part_update = false);

    static void draw(DispSegment* segment, signed* nv, uint8_t x_px, uint8_t y_px, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max);
    static void draw(DispSegment* segment, signed* nv, uint8_t charts_num, uint8_t spaceing_px);
    static void draw(DispSegment* segment, signed* nv, uint8_t charts_num, uint8_t spaceing_px, signed input_min, signed input_max);
};






class Plot
{
    DispSegment* ds;        // pointer to display segment
    signed* lv;             // last plot values 


    uint8_t w, h;           // plot full width and height with axes & titles (px)
    uint8_t wp, hp;         // plot width and height (only graphical part) (px)
    uint8_t x0, y0;         // Plot zero coordinates (px)

    signed xmin, xmax;
    signed ymin, ymax;
    
    char* xtitle;
    char* ytitle;


    
    public:
    Plot();
    Plot(DispSegment* ds, signed xmin, signed xmax, signed ymin, signed ymax, char* xtitle, char* ytitle);

    void init(DispSegment* ds, signed xmin, signed xmax, signed ymin, signed ymax, char* xtitle, char* ytitle);
    void show();
    void update_plot(signed* data, uint8_t data_qnt);

    static void plot(DispSegment* ds, signed* data, uint8_t data_qnt, signed xmin, signed xmax, signed ymin, signed ymax, char* xtitle, char* ytitle);
    static void plot(DispSegment* ds, signed* data, uint8_t data_qnt, signed xmin, signed xmax, signed ymin, signed ymax);
    static void plot(DispSegment* ds, signed* data, uint8_t data_qnt, signed ymin, signed ymax, char* xtitle, char* ytitle);
    static void plot(DispSegment* ds, signed* data, uint8_t data_qnt, char* xtitle, char* ytitle);
    static void plot(DispSegment* ds, signed* data, uint8_t data_qnt);

    private:
    static unsigned find_scale(unsigned origin_scale);
    static void calc_x_limits(DispSegment* ds, uint8_t data_qnt, signed* xmin, signed* xmax);
    static void calc_y_limits(DispSegment* ds, signed* data, uint8_t data_qnt, signed* ymin, signed* ymax);

    static void draw_data(DispSegment* ds, signed* data, uint8_t data_qnt, uint8_t x0, uint8_t y0, uint8_t hp, signed ymin, signed ymax);
};
    
