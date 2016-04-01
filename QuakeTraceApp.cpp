#include "QuakeTraceApp.hpp"
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
#include "CommandLine.hpp"
#include "BackgroundTracer.hpp"

const int DEFAULT_SCREEN_WIDTH = 640;
const int DEFAULT_SCREEN_HEIGHT = 480;
const int DEFAULT_DETAIL_LEVEL = 1;
const int DEFAULT_SOFT_SHADOW_RAYS = 10;
const int DEFAULT_OCCLUSION_RAYS = 32;

const std::uint32_t COLOR_TRANSPARENT = 0xFF980088;

using namespace std;

#define DEFAULT_SCENE 0

void QuakeTraceApp::setIconFromAsset(SDL_Window* window, AssetHelper::ID id)
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

int QuakeTraceApp::runUntilFinished(int argc, char const * const * const argv)
{
    util::CommandLine cmd(argc, argv);
    std::string mapName;
    std::string imageName;
    if (!cmd.parse(&mapName, "input", 'i'))
    {
        SDL_Log("No input map file found");
        return EXIT_FAILURE;
    }
    if (!cmd.parse(&imageName, "output", 'o'))
    {
        SDL_Log("No output tga file found");
        return EXIT_FAILURE;
    }
    RayTracer::Config config;
    if (!cmd.parse(&config.width, "width", 'w')) { config.width = DEFAULT_SCREEN_WIDTH; }
    if (!cmd.parse(&config.height, "height", 'h')) { config.height = DEFAULT_SCREEN_HEIGHT; }
    if (!cmd.parse(&config.detail, "detail", 'd')) { config.detail = DEFAULT_DETAIL_LEVEL; }
    if (!cmd.parse(&config.occlusionRayCount, "occlusion")) { config.occlusionRayCount = DEFAULT_OCCLUSION_RAYS; }
    if (!cmd.parse(&config.softshadowRayCount, "shadows")) { config.softshadowRayCount = DEFAULT_SOFT_SHADOW_RAYS; }
    if (!cmd.parse(&config.threads, "threads", 'j')) { config.threads = 4; }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("QuakeTrace", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, config.width, config.height, 0);
    if (window == NULL) {
        SDL_Log("Failed to create a window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    setIconFromAsset(window, AssetHelper::ICON);

    auto fb = FrameBuffer::createFromWindow(window);

    auto font = Font::create();
#if DEFAULT_SCENE
    Scene scene;
    Scene::initDefault(&scene);
#else
    File mapFile = File::open(mapName.c_str());
    ASSERT(mapFile.isValid());
    size_t mapDataSize = mapFile.size();
    std::vector<std::uint8_t> mapData(mapDataSize);
    mapFile.read(mapData.data(), mapDataSize);
    Scene scene = BspLoader::createSceneFromBsp(mapData.data(), mapDataSize);
#endif
    // Correct for aspect ratio
    scene.camera.halfViewAngles.y *= config.height / static_cast<float>(config.width);

    BackgroundTracer engine(config);

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

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.clicks == 2)
            {
                //engine.setBreakPoint(mouseX, mouseY);
                //refreshCanvas = true;
            }
        }

        if (refreshCanvas)
        {
            renderStart = SDL_GetTicks();
            engine.startTrace(scene);

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
            auto tga = targa::encode(engine.getCanvas());
            File f = File::openW(imageName.c_str());
            f.write(tga.data(), tga.size());
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


            // Display mouse x, y
            {
                char renderMouseStr[50];
                sprintf(renderMouseStr, "%d, %d", mouseX, mouseY);
                font->blitString(fb, renderMouseStr, 0, 0);
                updateMouse = false;
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
            font->blitString(fb, renderProgressStr, 0, 10);
        }

        fb->flip();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
