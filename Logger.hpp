#pragma once

#if SHOW_GUI
#include <SDL_log.h>

#define LOG SDL_Log
#else
#include <cstdio>

#define LOG std::printf
#endif
