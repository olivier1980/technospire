#pragma once

#include <string>
#include <sol/sol.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <iterator>




class Helper {
public:
    static std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    static SDL_Rect getSDLRect (const sol::table& luaVal, bool toScreen = false) {
        return SDL_Rect {
                luaVal.get<int>(1),
                luaVal.get<int>(2),
                luaVal.get<int>(3),
                luaVal.get<int>(4),
        };
    }

    static void getRekt (SDL_Rect &input) {
        input = {
                .x = input.x*PIXEL_SIZE,
                .y = input.y*PIXEL_SIZE,
                .w = input.w*PIXEL_SIZE,
                .h = input.h*PIXEL_SIZE,
        };
    };

    static std::string solToString(sol::object o)
    {
        return o.as<std::string>();
    }

    static std::string toLower(std::string upTxt) {
        for (char & i : upTxt) {
            i = tolower(i);
        }
        return upTxt;
    }

    static std::string UCFirst(std::string upTxt) {
        int j = 0;
        for (char & i : upTxt) {
            if (j == 0) {
                i = toupper(i);
            }
            ++j;
        }
        return upTxt;
    }



// get_pixel: Acquires a 32-bit pixel from a surface at given coordinates
    static Uint32 get_pixel( SDL_Surface* surface, int x, int y )
    {
        int bpp = surface->format->BytesPerPixel;

        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

        switch (bpp) {
            case 1:
                return *p;
            case 2:
                return *(Uint16 *) p;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    return p[0] << 16 | p[1] << 8 | p[2];
                else
                    return p[0] | p[1] << 8 | p[2] << 16;

            case 4:
                return *(Uint32 *) p;

            default:
                return 0;
        }
        //Convert the pixels to 32 bit
        Uint32 *pixels = (Uint32*)surface->pixels;

        //Get the requested pixelSDL_SRCCOLORKEY
        return pixels[ ( y * surface->w ) + x ];
    }

// put_pixel: Drops a 32-bit pixel onto a surface at given coordinates.
    static void put_pixel( SDL_Surface *surface, int x, int y, Uint32 c )
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
private:
    //https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
    template <typename Out>
    static void split(const std::string &s, char delim, Out result) {
        std::istringstream iss(s);
        std::string item;
        while (std::getline(iss, item, delim)) {
            *result++ = item;
        }
    }

};
