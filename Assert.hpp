#if SHOW_GUI
#include <SDL_assert.h>

#define ASSERT SDL_assert
#else
#include <cassert>
#define ASSERT assert
#endif

#define STATIC_ASSERT_ARRAY_SIZE(array, size) static_assert(sizeof(array)/sizeof(array[0]) == size, "Array size does not match")
