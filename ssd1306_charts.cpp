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

