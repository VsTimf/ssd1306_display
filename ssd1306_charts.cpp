#include "ssd1306_charts.hpp"


/**
 * @brief Construct a new empty Bar Chart
 */
BarChart::BarChart(){
  BarChart((DispSegment*)0, (signed*)0, 0, 0, 0, 0, 0, 0, 0, 0);}




 /**
  * @brief Construct a new Bar Chart. Fits the entire bar chart into a segment. Sets input range to [0 .. 100]
  * 
  * @param segment                    display segment to display chart
  * @param last_vals                  pointer to array for storing last chart values
  * @param charts_num                 quantity of charts
  * @param spacing                    spacing between chart items, px
  */
 BarChart::BarChart(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px)
{
  uint8_t calc_chart_width = (segment->sw - (charts_num-1)*spaceing_px) / charts_num;
  BarChart(segment, last_vals, 0, segment->shp-1, charts_num, calc_chart_width, spaceing_px, segment->shp, 0, 100);
}




/**
 * @brief Construct a new Bar Chart object
 * 
 * @param segment                       display segment to display chart
 * @param last_vals                     pointer to array for storing last chart values
 * @param charts_num                    quantity of charts
 * @param spaceing_px                   spacing between chart items, px
 * @param input_min                     minimum input numer corresponds to zero chart level          
 * @param input_max                     maximum input number corresponds to "height_px" chart level
 */
BarChart::BarChart(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px, signed input_min, signed input_max)
{
  uint8_t calc_chart_width = (segment->sw - (charts_num-1)*spaceing_px) / charts_num;
  BarChart(segment, last_vals, 0, segment->shp-1, charts_num, calc_chart_width, spaceing_px, segment->shp, input_min, input_max);
}




/**
 * @brief Construct a new Bar Chart object
 * 
 * @param segment                       display segment to display chart
 * @param last_vals                     pointer to array for storing last chart values
 * @param charts_num                    quantity of charts
 * @param width_px                      width of chart item, px
 * @param spaceing_px                   spacing between chart items, px
 * @param height_px                     max heigth of chart item, px 
 * @param input_min                     minimum input numer corresponds to zero chart level          
 * @param input_max                     maximum input number corresponds to "height_px" chart level
 */
BarChart::BarChart(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max)
{
  BarChart(segment, last_vals, 0, segment->shp-1, charts_num, width_px, spaceing_px, height_px, input_min, input_max);
}




/**
 * @brief Construct a new Bar Chart
 * 
 * @param segment                       display segment to display chart
 * @param last_vals                     pointer to array for storing last chart values
 * @param x_px                          x-coordinate of Bar Chart, px 
 * @param y_px                          y-coordinate of Bar Chart, px 
 * @param charts_num                    quantity of charts
 * @param width_px                      width of chart item, px
 * @param spaceing_px                   spacing between chart items, px
 * @param height_px                     max heigth of chart item, px 
 * @param input_min                     minimum input numer corresponds to zero chart level          
 * @param input_max                     maximum input number corresponds to "height_px" chart level
 */
BarChart::BarChart(DispSegment* segment, signed* last_vals, uint8_t x_px, uint8_t y_px, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max) : 
    ds(segment), 
    lv(last_vals),
    x(x_px), y(y_px),
    cnum(charts_num),
    w(width_px), sp(spaceing_px), h(height_px),
    imin(input_min), imax(input_max)
    {
      for(uint8_t i = 0; i<cnum; i++)
        last_vals[i] = 0;
    }






    void BarChart::init(DispSegment* segment, signed* last_vals, uint8_t x_px, uint8_t y_px, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max){
      BarChart(segment, last_vals, x_px, y_px, charts_num, width_px, spaceing_px, height_px, input_min, input_max);
    }

    void BarChart::init_with_fit_into_segment(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px){
      BarChart(segment, last_vals, charts_num, spaceing_px);
    }

    void BarChart::init_with_fit_into_segment(DispSegment* segment, signed* last_vals, uint8_t charts_num, uint8_t spaceing_px, signed input_min, signed input_max){
      BarChart(segment, last_vals, charts_num, spaceing_px, input_min, input_max);
    }

    



/**
 * @brief Displays bar chart with the specified values
 * 
 * @param nv                          array of new numeric values to display
 * @param part_update                 (optional, def = false) if true, updates only changed part of chart that can significantly increase display update speed. Works only in immediate mode
 */
void BarChart::show(signed* nv, bool part_update)
{
  uint8_t diff;
  uint8_t x_pos = x;
  uint8_t y_st;
  
  uint8_t max_val = y+1;                    
  uint8_t min_val = y-h+1;                            
  

  uint8_t nv_px;

  for(uint8_t ch = 0; ch < cnum; ch++)
  {
    nv_px = ((nv[ch] - imin) * h) / (imax - imin);

    if(nv_px > lv[ch])  // добавить пикселей к графику
    {
      diff = nv_px - lv[ch];
      y_st =  y-nv_px+1;
      
      for(uint8_t xch = x_pos; xch < x_pos + w; xch++)
        ds->draw_vline(xch, y_st, diff);
    }
    else                // убрать верхние пиксели
    {
      diff = lv[ch] - nv_px;
      y_st = y - lv[ch]+1;
    
      for(uint8_t xch = x_pos; xch < x_pos + w; xch++)
        ds->draw_vline(xch, y_st, diff, false);
    }
    
    // for optimized screen update
    max_val = max_val > y_st ? y_st : max_val;
    min_val = min_val < y_st - diff ? y_st - diff : min_val;

    
    lv[ch] = nv_px;
    x_pos += sp + w;
  }

  if(ds->immediate_update_mode_enabled())
  {
    if(part_update)
      ds->update_part(x, max_val, x+(w+sp)*cnum-sp-1, min_val);
    else
      ds->update_part(x, y+1-h, x+(w+sp)*cnum-sp-1, y);
  }
}





/**
 * @brief Draws Chart in specified display segment
 * 
 * @param segment                       display segment to display chart
 * @param nv                            pointer to data array to display
 * @param x_px                          x-coordinate of Bar Chart, px 
 * @param y_px                          y-coordinate of Bar Chart, px 
 * @param charts_num                    quantity of charts
 * @param width_px                      width of chart item, px
 * @param spaceing_px                   spacing between chart items, px
 * @param height_px                     max heigth of chart item, px 
 * @param input_min                     minimum input numer corresponds to zero chart level          
 * @param input_max                     maximum input number corresponds to "height_px" chart level
 */
void BarChart::draw(DispSegment* segment, signed* nv, uint8_t x_px, uint8_t y_px, uint8_t charts_num, uint8_t width_px, uint8_t spaceing_px, uint8_t height_px, signed input_min, signed input_max)
{
  uint8_t x_pos = x_px;
  uint8_t y_up = y_px - height_px + 1;
  uint8_t y_st;
           
  uint8_t nv_px, rest_px;

  for(uint8_t ch = 0; ch < charts_num; ch++)
  {
    nv_px = ((nv[ch] - input_min) * height_px) / (input_max - input_min);
    y_st = y_px - nv_px + 1;
    rest_px = height_px - nv_px;


    for(uint8_t xch = x_pos; xch < x_pos + width_px; xch++)
    {
      segment->draw_vline(xch, y_up, rest_px, false);
      segment->draw_vline(xch, y_st, nv_px);
    }

    x_pos += width_px;

    for(uint8_t xch = x_pos; xch < x_pos + spaceing_px; xch++)
      segment->draw_vline(xch, y_up, height_px, false);

    x_pos += spaceing_px;
  }

  if(segment->immediate_update_mode_enabled())
    segment->update_part(x_px, y_px+1 - height_px, x_px + (width_px + spaceing_px) * charts_num-spaceing_px-1, y_px);
} 


void BarChart::draw(DispSegment* segment, signed* nv, uint8_t charts_num, uint8_t spaceing_px, signed input_min, signed input_max)
{
  uint8_t calc_chart_width = (segment->sw - (charts_num-1)*spaceing_px) / charts_num;
  BarChart::draw(segment, nv, 0, segment->shp-1, charts_num, calc_chart_width, spaceing_px, segment->shp, input_min, input_max);
}


void BarChart::draw(DispSegment* segment, signed* nv, uint8_t charts_num, uint8_t spaceing_px)
{
  BarChart::draw(segment, nv, charts_num, spaceing_px, 0, 100);
}







/**
 * @brief Construct a new Plot object
 */
Plot::Plot() : ds((DispSegment*)0){}



/**
 * @brief Construct a new Plot object
 * 
 * @param _ds                       // display segment
 * @param _xmin                     // x-axis min
 * @param _xmax                     // x-axis max
 * @param _ymin                     // y-axis min
 * @param _ymax                     // y-axis max 
 * @param _xtitle                   // x-title
 * @param _ytitle                   // y-title 
 */
Plot::Plot(DispSegment* _ds, signed _xmin, signed _xmax, signed _ymin, signed _ymax, char* _xtitle, char* _ytitle) : 
ds(_ds), 
xmin(_xmin), xmax(_xmax),
ymin(_ymin), ymax(_ymax),
xtitle(_xtitle), ytitle(_ytitle)
{}



/**
 * @brief 
 * 
 * @param ds                          // display segment
 * @param xmin                        // x-axis min
 * @param xmax                        // x-axis max
 * @param ymin                        // y-axis min
 * @param ymax                        // y-axis max 
 * @param xtitle                      // x-title
 * @param ytitle                      // y-title 
 */
void Plot::init(DispSegment* ds, signed xmin, signed xmax, signed ymin, signed ymax, char* xtitle, char* ytitle)
{
  this->ds = ds;

  // widjet dimensions
  w = ds->sw;
  h = ds->shp;
  
  // plot zero point
  x0 = font5.height + 3;
  y0 = (ds->shp - 1) - font5.height - 3;
  
  // plot dimensions
  wp = w - x0 + 1;
  hp = y0 + 1;


  this->xmin = xmin;
  this->xmax = xmax;

  this->ymin = ymin;
  this->ymax = ymax;

  this->xtitle = xtitle;
  this->ytitle = ytitle;
}


/**
 * @brief Shows plot area: axes, titles, ticks
 */
void Plot::show()
{
  Plot::plot(ds, (signed*)0, 0, xmin, xmax, ymin, ymax, xtitle, ytitle);
}


/**
 * @brief Draws data points
 * 
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 */
void Plot::update_plot(signed* data, uint8_t data_qnt)
{
  ds->clear_part(8,0, w-1, y0, true);
  draw_data(ds, data, data_qnt, x0, y0, hp, ymin, ymax);
  ds->update_part(8,0, w-1, y0);
}






/**
 * @brief Calculates aligned scale for y-axis
 * 
 * @param origin_scale                // Original scale, based on input raw data
 * @return unsigned                   // returns aligned scale
 */
unsigned Plot::find_scale(unsigned origin_scale)
{
  static const unsigned pattern_scale_k[3] = {1, 2, 5};

  int pattern_scale_mult = 1;
  int pattern_scale = 1;

  while(1)
  {
    for(uint8_t i = 0; i<3; i++)
    {
      pattern_scale = pattern_scale_mult * pattern_scale_k[i];

      if (origin_scale <= pattern_scale)
        return pattern_scale;
    }
    pattern_scale_mult *= 10;
  }
}



/**
 * @brief 
 * 
 * @param ds                          // display segment
 * @param data_qnt                    // amount of data to plot
 * @param xmin                        // pointer to variable to save calculated xmin
 * @param xmax                        // pointer to variable to save calculated xmax
 */
void Plot::calc_x_limits(DispSegment* ds, uint8_t data_qnt, signed* xmin, signed* xmax)
{
  uint8_t wp = ds->sw - (font5.height + 3) + 1;

  *xmin = 0;
  *xmax = (data_qnt > wp) ? wp : data_qnt;
}



/**
 * @brief Calculates y limits for plot
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 * @param ymin                        // pointer to variable to save calculated ymin
 * @param ymax                        // pointer to variable to save calculated ymax
 */
void Plot::calc_y_limits(DispSegment* ds, signed* data, uint8_t data_qnt, signed* ymin, signed* ymax)
{
  uint8_t hp = ((ds->shp - 1) - font5.height - 3) + 1;

  // Real min, max values of input data
  signed imin = data[0];
  signed imax = data[0];

  int scale;                      // scale of y-axis
  int k;                          // support variable

  for(uint8_t i=1; i < data_qnt; i++)
  {
    if(imin > data[i])
      imin = data[i];

    if(imax < data[i])
      imax = data[i];
  }


  scale = Plot::find_scale((imax - imin) / hp);


  k = (imax > 0) ? ((imax % scale != 0) ? ((imax / scale) + 1) : (imax / scale)) : (imax / scale);
  *ymax = scale * k;


  k = (imin < 0) ? ((imax % scale != 0) ? ((imin / scale) - 1) : (imin / scale)) : (imin / scale);
  *ymin = scale * k;
}




/**
 * @brief Draws plot in the specified display segment. Auto scale. No titles
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 */
void Plot::plot(DispSegment* ds, signed* data, uint8_t data_qnt){
  plot(ds, data, data_qnt, (char*)0, (char*)0);
}



/**
 * @brief Draws plot in the specified display segment. Auto scale
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 * @param xtitle                      // x-title
 * @param ytitle                      // y-title
 */
void Plot::plot(DispSegment* ds, signed* data, uint8_t data_qnt, char* xtitle, char* ytitle)
{
  signed xmin, xmax, ymin, ymax;

  Plot::calc_x_limits(ds, data_qnt, &xmin, &xmax);
  Plot::calc_y_limits(ds, data, data_qnt, &ymin, &ymax);

  plot(ds, data, data_qnt, xmin, xmax, ymin, ymax, xtitle, ytitle);
}



/**
 * @brief Draws plot in the specified display segment. Auto x scale
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 * @param ymin                        // y-axis min
 * @param ymax                        // y-axis max
 * @param xtitle                      // x-title
 * @param ytitle                      // y-title
 */
void Plot::plot(DispSegment* ds, signed* data, uint8_t data_qnt, signed ymin, signed ymax, char* xtitle, char* ytitle)
{
  signed xmin, xmax;

  Plot::calc_x_limits(ds, data_qnt, &xmin, &xmax);
  plot(ds, data, data_qnt, xmin, xmax, ymin, ymax, xtitle, ytitle);
}



/**
 * @brief Draws plot in the specified display segment. No titles
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 * @param xmin                        // x-axis min
 * @param xmax                        // x-axis max
 * @param ymin                        // y-axis min
 * @param ymax                        // y-axis max
 */
void Plot::plot(DispSegment* ds, signed* data, uint8_t data_qnt, signed xmin, signed xmax, signed ymin, signed ymax){
  plot(ds, data, data_qnt, xmin, xmax, ymin, ymax, (char*)0, (char*)0);
}



/**
 * @brief Draws plot in the specified display segment
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 * @param xmin                        // x-axis min
 * @param xmax                        // x-axis max
 * @param ymin                        // y-axis min
 * @param ymax                        // y-axis max
 * @param xtitle                      // x-title
 * @param ytitle                      // y-title
 */
void Plot::plot(DispSegment* ds, signed* data, uint8_t data_qnt, signed xmin, signed xmax, signed ymin, signed ymax, char* xtitle, char* ytitle)
{
  // widjet dimensions
  uint8_t w = ds->sw;
  uint8_t h = ds->shp;

  // plot dimensions
  uint8_t x0 = font5.height + 3;
  uint8_t y0 = (ds->shp - 1) - font5.height - 3;

  uint8_t wp = w - x0 + 1;
  uint8_t hp = y0 + 1;

  
  ds->clear();

  // x labels
  if(xtitle)
    ds->write_string(x0 + (w - x0)/2 - ds->get_string_size_px(xtitle, font5)/2, y0+3, xtitle, font5);

  ds->write_num(x0 + 3, y0 + 3, 0, font5);
  ds->write_num(w - ds->get_num_string_size_px(xmax, font5), y0 + 3, xmax, font5);


 // y labels
  ds->set_text_vertical_mode();
  if(ytitle)
    ds->write_string(0, h/2 + ds->get_string_size_px(ytitle, font5)/2, ytitle, font5);
  ds->write_num(0, ds->get_num_string_size_px(ymax, font5), ymax, font5);
  ds->write_num(0, 63, ymin, font5);
  ds->set_text_horizontal_mode();



  ds->draw_hline(x0-1, y0+1, wp);    // x axis
  ds->draw_vline(x0-1, 0, h);        // y axis
  

  draw_data(ds, data, data_qnt, x0, y0, hp, ymax, ymin);

  ds->update();
}



/**
 * @brief Draws given data points to plot area
 * 
 * @param ds                          // display segment
 * @param data                        // data to plot
 * @param data_qnt                    // amount of data to plot
 * @param x0                          // left coordinate of plot area
 * @param y0                          // bottom coordinate of plot area
 * @param hp                          // height of plot area
 * @param ymin                        // lowest value among "data" to plot
 * @param ymax                        // highest value among "data" to plot
 */
void Plot::draw_data(DispSegment* ds, signed* data, uint8_t data_qnt, uint8_t x0, uint8_t y0, uint8_t hp, signed ymin, signed ymax)
{
  signed range = ymax - ymin;                           // input range
  uint8_t px_out;                                       // data value expressed in pixels
  signed inp_val;
  for(uint8_t i = 0; i < data_qnt; i++)
  {
    inp_val = *(data+i);
    if((inp_val >= ymin) && (inp_val <= ymax))
    {
      px_out = ((inp_val - ymin) * (hp-1)) / range;
      ds->draw_pixel(i+x0, y0 - px_out);
    }
  }
}
