#pragma once

#if SHOW_GUI
#include <SDL.h>
#define BRPT() SDL_TriggerBreakpoint()
#else
#define BRPT() {}
#endif