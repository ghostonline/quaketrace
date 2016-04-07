#include "GUI.hpp"
#include "Console.hpp"

#if TARGET_WIN32
#include "SDL.h" // Convert main into winmain if necessary
#endif

int main(int argc, char* argv[]) {
#if SHOW_GUI
    return GUI::runUntilFinished(argc, argv);
#else
    return Console::runUntilFinished(argc, argv);
#endif
}
