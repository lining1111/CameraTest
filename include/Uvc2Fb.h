//
// Created by lining on 12/15/21.
//

#ifndef _UVC2FB_H
#define _UVC2FB_H

int read_JPEG_file(const char *jpegData, char *rgbdata, int size);

void lcd_show_rgb(unsigned char *rgbdata, int w, int h);

int Uvc2Fb(void);

#endif //_UVC2FB_H
