#include "GUI.hpp"
#include "Console.hpp"

#include "SDL.h" // Convert main into winmain if necessary

int main(int argc, char* argv[]) {
    return Console::runUntilFinished(argc, argv);
}
