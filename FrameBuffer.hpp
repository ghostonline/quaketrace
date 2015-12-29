#pragma once

struct SDL_Window;
struct SDL_Surface;

class FrameBuffer
{
public:
    static FrameBuffer* createFromWindow(SDL_Window* window);
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getPitch() const { return pitch; }
    void* get();
    void flip();

private:
    FrameBuffer(SDL_Window* window, SDL_Surface* buffer, int width, int height, int pitch)
        : window(window)
        , buffer(buffer)
        , width(width)
        , height(height)
        , pitch(pitch)
    {}

    SDL_Window* window;
    SDL_Surface* buffer;
    const int width;
    const int height;
    const int pitch;
};
