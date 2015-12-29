#include "FrameBuffer.hpp"
#include "SDL.h"

FrameBuffer* FrameBuffer::createFromWindow(SDL_Window* window)
{
    SDL_Surface* buffer = SDL_GetWindowSurface(window);
    if (buffer == nullptr)
    {
        SDL_Log("Failed to obtain framebuffer");
        return nullptr;
    }

    // Make sure that pixel format is as expected
    SDL_assert(buffer->format->BitsPerPixel == 32);
    SDL_assert(buffer->format->Rmask == 0x00FF0000);
    SDL_assert(buffer->format->Gmask == 0x0000FF00);
    SDL_assert(buffer->format->Bmask == 0x000000FF);

    return new FrameBuffer(window, buffer, buffer->w, buffer->h, buffer->pitch);
}

void* FrameBuffer::get()
{
    return buffer->pixels;
}

void FrameBuffer::flip()
{
    SDL_UpdateWindowSurface(window);
}
