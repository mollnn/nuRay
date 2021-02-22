#ifndef _IMAGEWND_HPP_
#define _IMAGEWND_HPP_

#include "uint.h"
#include "vec3.hpp"
#include "image.hpp"
#include "timer.hpp"
#include <SDL2/SDL.h>
#include "color.hpp"

/////////////////////////////////////

void PutPixel24(SDL_Surface *surface, int x, int y, Uint32 color)
{
    Uint8 *pixel = (Uint8 *)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 4);

    pixel[0] = (color >> 0) & 0xFF;  //a
    pixel[1] = (color >> 8) & 0xFF;  //r
    pixel[2] = (color >> 16) & 0xFF; //g
    pixel[3] = (color >> 24) & 0xFF; //b
}

void showImageWindow(Image &image)
{

    int img_width = image.size_x;
    int img_height = image.size_y;

    //init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        return;
    }
    SDL_Window *window = SDL_CreateWindow("Image Viewer", 100, 100, img_width, img_height, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Quit();
        return;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    //Create SW surface
    SDL_Surface *surface = SDL_CreateRGBSurface(0, img_width, img_height, 24,
                                                0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000);
    if (!surface)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    SDL_LockSurface(surface);
    for (int y = 0; y < img_height; y++)
    {
        for (int x = 0; x < img_width; x++)
        {
            vec3 c = image.Get(x, y);
            auto [r, g, b] = colorFloatToUint8(c);
            uint32 color = SDL_MapRGB(surface->format, r, g, b);
            PutPixel24(surface, x, y, color);
        }
    }
    SDL_UnlockSurface(surface);

    //Create HW surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!surface)
    {
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    SDL_FreeSurface(surface);

    //Main loop
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //Draw BMP
        SDL_Rect dst;
        dst.x = 0;
        dst.y = 0;
        dst.w = img_width;
        dst.h = img_height;
        SDL_RenderCopy(renderer, texture, NULL, &dst);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

/////////////////////////////////////

#endif