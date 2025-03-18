/**
  ******************************************************************************
  * @brief   SSD1306  tests
  *  Implements differents tests to check library and to show most commonly used use cases
*/

#include "ssd1306_tests.hpp"


#define RUN_DEFAULT_TESTS               // switch default tests and user tests


/**
 * @brief Runs all driver tests
 * 
 * @param display                     pointer to "Display" instance
 */
void ssd1306_run_tests(SSD1306_Display* display)
{
    #ifdef RUN_DEFAULT_TESTS

    ssd1306_font_test(display);
    HAL_Delay(3000);

    ssd1306_primitives_test(display);
    HAL_Delay(3000);

    ssd1306_menu_test11(display);
    HAL_Delay(3000);
    ssd1306_menu_test12(display);
    HAL_Delay(3000);

    ssd1306_menu_test21(display);
    HAL_Delay(3000);
    ssd1306_menu_test22(display);
    HAL_Delay(3000);

    ssd1306_chart_test(display);

    ssd1306_layout_test(display);
    HAL_Delay(3000);

    ssd1306_bitmap_test(display);
    HAL_Delay(3000);

    ssd1306_end_test(display);

    #else
   
    my_test(display);
    
    #endif
}



/**
 * @brief Font test
 */
void ssd1306_font_test(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "Font Test", font16);

    display->write_string(0, ROW3, "Font-8", font8);
    display->write_string(0, ROW4, "Шрифт-8", font8);

    display->write_string(0, ROW5, "Font-16", font16);
    display->write_string(0, ROW7, "Шрифт-16", font16);

    display->update_screen();
}




/**
 * @brief Lines, rectangles, circles test
 */
void ssd1306_primitives_test(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "PrimitivesTest", font16);


    display->draw_hline(0, display->HEIGHT_PX - 1, display->WIDTH_PX);
    display->draw_vline(0, ROW3, display->HEIGHT_PX - ROW3);

    display->draw_frame(10, ROW3, 16, 16);
    display->draw_box(10, ROW6, 16, 16);

    display->draw_circle(56, ROW6, 16);
    display->draw_pixel(56, ROW6);
    
    display->draw_filled_circle(90, ROW6, 16);
    display->draw_pixel(90, ROW6, false);

    display->update_screen();
}




/**
 * @brief Menu, drawed with font8 test
 */
void ssd1306_menu_test11(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "Menu Test #1.1", font16);

    display->set_select_method(SSD1306_ITEM_SELECT_METHOD::ARROW);
    display->draw_param(ROW3, "Параметр 1", font8, 100);
    display->draw_param(ROW4, "Параметр 2", font8, 200, true);

    display->set_select_method(SSD1306_ITEM_SELECT_METHOD::COLOR);
    display->draw_param(ROW6, "Параметр 3", font8, "выкл");
    display->draw_param(ROW7, "Параметр 4", font8, "вкл", true);

    display->update_screen();
}




/**
 * @brief Menu, drawed with font16 test
 */
void ssd1306_menu_test12(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "Menu Test #1.2", font16);

    display->set_select_method(SSD1306_ITEM_SELECT_METHOD::ARROW);
    display->draw_param(ROW3, "Param 1", font16, 100);
    display->draw_param(ROW5, "Param 2", font16, 200, true);

    display->set_select_method(SSD1306_ITEM_SELECT_METHOD::COLOR);
    display->draw_param(ROW7, "Param 3", font16, "On", true);

    display->update_screen();
}




/**
 * @brief Menu (checkbox & bar), drawed with font8 test
 */
void ssd1306_menu_test21(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "Menu Test #2.1", font16);

    display->set_select_method(SSD1306_ITEM_SELECT_METHOD::ARROW);
    display->draw_checkbox(ROW3, "Disabled", font8, false);
    display->draw_checkbox(ROW4, "Enabled", font8, true, true);

    display->draw_hbar(ROW6, 50, font8);
    display->draw_hbar(ROW7, 70, font8);

    display->update_screen();
}




/**
 * @brief Menu (checkbox & bar), drawed with font16 test
 */
void ssd1306_menu_test22(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "Menu Test #2.2", font16);

    display->draw_checkbox(ROW3, "Disabled", font16, false);
    display->draw_checkbox(ROW5, "Enabled", font16, true, true);

    display->draw_hbar(ROW7, 70, font16);
    

    display->update_screen();
}




/**
 * @brief Charts test
 */
void ssd1306_chart_test(SSD1306_Display* display)
{
    BarChart* bar_chart1;
    BarChart* bar_chart2;
    BarChart* bar_chart3;
    signed vals[3] = {30,60,90};



    display->clear_screen();

    display->write_string(0, ROW1, "Chart Test", font16);
    display->update_row(ROW1, font16);
    
    bar_chart1 = new BarChart(display->dds, 0, 63, 3, 10, 2, 8*6, 0, 100);
    bar_chart2 = new BarChart(display->dds, 58, 63, 3, 5, 1, 8*3, 0, 100);
    bar_chart3 = new BarChart(display->dds, 90, 63, 3, 2, 1, 8*6, 0, 100);
   
    display->set_segment_update_mode_immediately();
    
    bar_chart1->show(vals);
    bar_chart2->show(vals);
    bar_chart3->show(vals);


    unsigned count = 10;
    while(count--)
    {
        for(unsigned i = 0; i<30; i++)
        {
            vals[0]--;
            vals[1]-=2;
            vals[2]-=3;

            bar_chart1->show(vals);
        }

        for(unsigned i = 0; i<30; i++)
        {
            vals[0]++;
            vals[1]+=2;
            vals[2]+=3;

            bar_chart1->show(vals);
        }
    }
    display->set_segment_update_mode_on_demand();
}




/**
 * @brief Layout test
 */
void ssd1306_layout_test(SSD1306_Display* display)
{
    DispLayout* test_layout;

    DispSegment* test_title;
    DispSegment* test_horizontal_area;
    DispSegment* test_verctical_area;
  
    BarChart* bar_chart;
    signed vals[3] = {30,60,90};
    const char *lbl[] = {"1", "2", "3"};

    display->clear_screen();
  
    test_layout = display->create_layout();
    
    test_title  = test_layout->create_segment(SSD1306_ADDR_MODE::HORIZONTAL, 0, 0, display->WIDTH_PX-1, 1);
    test_horizontal_area  = test_layout->create_segment(SSD1306_ADDR_MODE::HORIZONTAL, 16, 2, display->WIDTH_PX-1, display->HEIGHT_PG-1);
    test_verctical_area = test_layout->create_segment(SSD1306_ADDR_MODE::HORIZONTAL, 0, 2, 15, display->HEIGHT_PG-1);
  
  
    test_title->write_string(0, ROW1, "Layout Test", font16);
  
    test_horizontal_area->draw_param(ROW1, "Числ. параметр", font8, 101);
    test_horizontal_area->draw_param(ROW2, "Строк. параметр", font8, "вкл", true);
    test_horizontal_area->draw_checkbox(ROW3, "Checkbox", font8, "вкл", true);
    test_horizontal_area->draw_hbar(ROW6, 64, font8);
  

  
    test_horizontal_area->draw_bitmap(68, ROW5-5, accum[0]);
    test_horizontal_area->draw_bitmap(84, ROW5-5, accum[2]);
    test_horizontal_area->draw_bitmap(98, ROW5-5, accum[3]);
  
  
    bar_chart = new BarChart(test_verctical_area, 0, ROW6-2, 3, 4, 1, 39, 0, 100); 
    bar_chart->show(vals);

    test_verctical_area->draw_x_tick_labels(0, ROW6, 3, 0, lbl, font8);
  
    test_title->update();
    test_horizontal_area->update();
    test_verctical_area->update();
}




/**
 * @brief Picture test
 */
void ssd1306_bitmap_test(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string(0, ROW1, "Bitmap Test", font16);

    display->draw_bitmap(0, ROW3, big_car);

    display->update_screen();
}




/**
 * @brief Some ssd1306 hardware features test
 */
void ssd1306_end_test(SSD1306_Display* display)
{
   // Fade out test
   display->clear_font_row(ROW1, font16);
   display->update_row(ROW1, font16); 
   display->write_string_now(0, ROW1, "Fade out", font16); 
   display->fade_out(SSD1306_FADE_FRAMES::F8);
   HAL_Delay(2500);
   display->disable_fade_out();
   
   

   // Contrast test
   signed param = 0xFF;
   do
   {
    display->draw_param(ROW1, "Contrast", font16, param);
    display->update_row(ROW1, font16);   

    display->set_contrast(param--);
    HAL_Delay(25);
   } while(param>=0);
   display->set_contrast(0x7F);


   // Start line test
   param = 63;   
   do
   {
    display->draw_param(ROW1, "Start line", font16, param);
    display->update_row(ROW1, font16); 

    display->set_display_start_line(param--);
    HAL_Delay(100);

   }while(param>=0);

   
   // Entire display On/Off
   display->entire_on();
   HAL_Delay(2000);
   display->entire_off();
   HAL_Delay(2000);


   display->clear_screen();
   display->write_string(24, ROW3, "Test END", font16);
   display->update_row(ROW3, font16);    
}




/**
 * @brief User test
 */
void my_test(SSD1306_Display* display)
{
    display->clear_screen();

    display->write_string_now(0, ROW1, "My Test", font16);
    display->update_screen();
}