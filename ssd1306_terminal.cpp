#pragma once

#include <stdlib.h>
#include <string.h>
#include "ssd1306_terminal.hpp"


/*Terminal::Terminal()
{
    ds = NULL;
    LINE_WIDTH = LINES_QNT = 0;
}*/


Terminal::Terminal(SSD1306_Display* display) : Terminal(display->dds){}


Terminal::Terminal(DispSegment* _ds)
{
    ds = _ds;

    LINES_QNT = ds->shp / font8.height;
    LINE_WIDTH = ds->sw / (font8.width + font8.interval);

    buf = (char**)malloc(LINES_QNT * sizeof(char*)); // Allocate memory for array of char*

     // Allocate memory for each string
    for (int i = 0; i < LINES_QNT; i++) 
    {
        // Allocate memory for a string (including space for null terminator)
        buf[i] = (char*)malloc((LINE_WIDTH+1) * sizeof(char)); // Assuming max length of each string is 99 characters

        if (buf[i] == NULL) 
          while(1);

        for(unsigned c = 0; c<=LINE_WIDTH; c++)
          buf[i][c] = 0;
    
    }
}

Terminal::~Terminal()
{
    // Free the allocated memory
    for (int i = 0; i < LINES_QNT; i++) {
        free(buf[i]); // Free each individual string
    }
    free(buf); // Free the array of pointers
}





void Terminal::scroll()
{
    if(first_line < LINES_QNT-1)
    {
        new_line = first_line;
        first_line++;
    }
    else
    {
        first_line = 0;
        new_line = LINES_QNT - 1;
    }
}



void Terminal::out(const char* str)
{
    if(display_line < LINES_QNT)
    {
        strncpy(buf[new_line], str, LINE_WIDTH);
        display_line++;
        new_line++;
    }
    else
    {
        scroll();
        strncpy(buf[new_line], str, LINE_WIDTH);
    }


    display();
}




void Terminal::indent(){   out(" ");   }

void Terminal::separator() {   out("--------------------");  }


void Terminal::clear()
{
    first_line = 0;
    new_line = 0;
    display_line = 0;


    for (int i = 0; i < LINES_QNT; i++)
        for(int j = 0; j < LINE_WIDTH; j++)
            buf[i][j] = 0;

    display();
}



void Terminal::display()
{  
    ds->clear();
    
    unsigned ypos = 0;
    
    for(unsigned l = first_line; l<LINES_QNT; l++)
    {
        ds->write_string(0, ypos, buf[l], font8);
        ypos += font8.height;
    }

    for(unsigned l = 0; l<first_line; l++)
    {
        ds->write_string(0, ypos, buf[l], font8);
        ypos += font8.height;
    }

    ds->update();
}
