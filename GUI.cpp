#include "GUI.hpp"
#include "SDL.h"
#include "FrameBuffer.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include "Font.hpp"
#include "GraphicTool.hpp"
#include "Assert.hpp"
#include "Util.hpp"
#include "BspLoader.hpp"
#include "Targa.hpp"
#include "File.hpp"
#include "BackgroundTracer.hpp"
#include "AppConfig.hpp"
#include "Common.hpp"

const std::uint32_t COLOR_TRANSPARENT = 0xFF980088;

using namespace std;

#define DEFAULT_SCENE 0

void GUI::setIconFromAsset(SDL_Window* window, AssetHelper::ID id)
{
    auto iconSrc = AssetHelper::loadSurface(id);
    auto icon = SDL_ConvertSurfaceFormat(iconSrc, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(iconSrc);
    uint32_t* pixels = reinterpret_cast<uint32_t*>(icon->pixels);
    int pitch = icon->pitch >> 2;
    for (int x = icon->w - 1; x >= 0; --x)
    {
        for (int y = icon->h - 1; y >= 0; --y)
        {
            if (pixels[x + y * pitch] == COLOR_TRANSPARENT)
            {
                pixels[x + y * pitch] = 0;
            }
        }
    }
    SDL_SetWindowIcon(window, icon);
    SDL_FreeSurface(icon);
}

int GUI::runUntilFinished(int argc, char const * const * const argv)
{
    AppConfig config;
    {
        auto parseResult = config.parse(argc, argv);
        if (!parseResult.success)
        {
            SDL_Log("Parse failed: %s", parseResult.error.c_str());
            return EXIT_FAILURE;
        }
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("QuakeTrace", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config.width, config.height, 0);
    if (window == NULL) {
        SDL_Log("Failed to create a window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    setIconFromAsset(window, AssetHelper::ICON);

    auto fb = FrameBuffer::createFromWindow(window);

    auto font = Font::create();
    Scene scene;
#if DEFAULT_SCENE
    Scene::initDefault(&scene);
#else
    if (!common::loadBSP(config.mapFile.c_str(), &scene, config.width, config.height))
    {
        SDL_Log("Could not open map file: %s", config.mapFile.c_str());
        return EXIT_FAILURE;
    }
#endif

    if (config.cameraList)
    {
        SDL_Log("%lu", scene.cameras.size());
        return EXIT_SUCCESS;
    }

    RayTracer::Config traceConfig = common::parseRayTracerConfig(config);
    BackgroundTracer engine(traceConfig);

    bool finished = false;
    int mouseX = 0, mouseY = 0;
    bool refreshCanvas = true;
    bool updateMouse = true;
    bool updateScene = false;
    bool wasTracing = false;
    uint32_t renderTime = 0;
    uint32_t renderStart = 0;
    while (!finished)
    {
        // Process all events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            const bool quitEvent =
            (event.type == SDL_QUIT) || // Window close or ALT+F4
            (event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE); // Esc KEY
            if (quitEvent)
            {
                finished = true;
            }

            if (event.type == SDL_MOUSEMOTION)
            {
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                updateMouse = true;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.clicks == 2 && !updateScene)
            {
                engine.setBreakPoint(mouseX, mouseY);
                refreshCanvas = true;
            }
        }

        if (refreshCanvas)
        {
            renderStart = SDL_GetTicks();
            size_t cameraIdx = math::clamp<size_t>(config.cameraIdx, 0, scene.cameras.size());
            engine.startTrace(scene, scene.cameras[cameraIdx]);

            refreshCanvas = false;
        }
        else
        {
            SDL_Delay(updateScene ? 500 : 250);
        }

        wasTracing = updateScene;
        updateScene = engine.isTracing();

        if (!engine.isTracing() && wasTracing)
        {
            renderTime = SDL_GetTicks() - renderStart;
            if (!common::writeToTGA(engine.getCanvas(), config.imageFile.c_str()))
            {
                SDL_Log("Could not write to file: %s", config.imageFile.c_str());
                return EXIT_FAILURE;
            }
        }

        if (updateMouse || updateScene || wasTracing)
        {
            {
                const auto& canvas = engine.getCanvas();
                uint8_t* fbPixels = static_cast<uint8_t*>(fb->get());
                const int rowLength = canvas.width * canvas.getPixelSize();

                if (fb->getPitch() == rowLength)
                {
                    // Just copy over the full buffer in one go
                    memcpy(fbPixels, canvas.pixels.data(), canvas.pixels.size());
                }
                else
                {
                    for (int ii = 0; ii < canvas.height; ++ii)
                    {
                        uint8_t* fbRow = fbPixels + (ii * fb->getPitch());
                        const uint8_t* canvasRow = canvas.pixels.data() + (ii * canvas.width);
                        memcpy(fbRow, canvasRow, rowLength);
                    }
                }
            }


            // Display progress
            char renderProgressStr[50];
            if (engine.isTracing())
            {
                int percentage = engine.getProgress() * 100;
                sprintf(renderProgressStr, "%d%%", percentage);
            }
            else
            {
                sprintf(renderProgressStr, "%d ms", renderTime);
            }
            font->blitString(fb, renderProgressStr, 0, 0);

            // Display mouse x, y
            if (!engine.isTracing())
            {
                char renderMouseStr[50];
                sprintf(renderMouseStr, "%d, %d", mouseX, mouseY);
                font->blitString(fb, renderMouseStr, 0, 10);
                updateMouse = false;
            }
            
        }
        
        fb->flip();
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
