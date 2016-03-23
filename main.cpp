#include "QuakeTraceApp.hpp"

#include "SDL.h" // Convert main into winmain if necessary

int main(int argc, char* argv[]) {
    return QuakeTraceApp::runUntilFinished(argc, argv);
}
