#ifndef MODE7MYSDL_FUNCTION_H
#define MODE7MYSDL_FUNCTION_H

#include <SDL_ttf.h>

// get_pixel: Acquires a 32-bit pixel from a surface at given coordinates
Uint32 get_pixel( SDL_Surface* surface, int x, int y )
{
    int bpp = surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *) p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *) p;
            break;

        default:
            return 0;
    }
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)surface->pixels;

    //Get the requested pixelSDL_SRCCOLORKEY
    return pixels[ ( y * surface->w ) + x ];
}

// put_pixel: Drops a 32-bit pixel onto a surface at given coordinates.
void put_pixel( SDL_Surface *surface, int x, int y, Uint32 c )
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            *p = c;
            break;

        case 2:
            *(Uint16 *) p = c;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (c >> 16) & 0xff;
                p[1] = (c >> 8) & 0xff;
                p[2] = c & 0xff;
            } else {
                p[0] = c & 0xff;
                p[1] = (c >> 8) & 0xff;
                p[2] = (c >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *) p = c;
            break;
    }
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = c;
}

#endif //MODE7MYSDL_FUNCTION_H
