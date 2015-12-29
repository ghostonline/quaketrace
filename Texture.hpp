#pragma once

struct SDL_Surface;

class Texture
{
public:
    static Texture* create(const char* file);
    static Texture* createFromSurface(SDL_Surface* surface);

    Texture(SDL_Surface* surface);
    ~Texture();

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getPitch() const { return pitch; }
    void* getPixels() const { return pixels; }


private:
    SDL_Surface* surface;
    int width;
    int height;
    int pitch;
    void* pixels;
}
;