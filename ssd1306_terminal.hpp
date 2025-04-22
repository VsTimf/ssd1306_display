#ifndef TERMINAL_H
#define TERMINAL_H

#include "ssd1306_display.hpp"

class Terminal
{
    unsigned LINES_QNT;
    unsigned LINE_WIDTH;

    char** buf;
    unsigned first_line = 0;    // id массива указывающий на строку c которой будет начинаться вывод данных
    unsigned new_line = 0;     // id массива на строку в которую будут записываться новые данные
    unsigned display_line = 0;  // вертикальная координата экрана куда будет выводиться новая строка данных


    DispSegment* ds;


    void scroll();
    void display();

    public:

    void out(const char* str);
    void indent();
    void separator();
    void clear();




  //  Terminal();
    Terminal(SSD1306_Display* display);
    Terminal(DispSegment* _ds);
    ~Terminal();


};

#endif // TERMINAL_H
