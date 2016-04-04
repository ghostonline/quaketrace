#include "GUI.hpp"

#include "SDL.h" // Convert main into winmain if necessary

int main(int argc, char* argv[]) {
    return GUI::runUntilFinished(argc, argv);
}
