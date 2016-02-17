#include <SDL_assert.h>

#define ASSERT SDL_assert
#define STATIC_ASSERT_ARRAY_SIZE(array, size) static_assert(sizeof(array)/sizeof(array[0]) == size, "Array size does not match")
