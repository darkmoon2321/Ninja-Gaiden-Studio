#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>

class pixel
{
public:
    pixel();
    pixel operator = (pixel);
    bool operator == (pixel);
    bool operator != (pixel);
    uint16_t diff(pixel p);

    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

#endif // PIXEL_H
