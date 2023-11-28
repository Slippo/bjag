/*
 *
 * A program that demonstrates camera control and a scene graph architecture
 *
 * Copyright (c) 2018-2019 Oliver van Kaick <Oliver.vanKaick@carleton.ca>, David Mould <mould@scs.carleton.ca>
 *
 */


#include <iostream>
#include <exception>
#include "game.h"

// Macro for printing exceptions
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

// Main function that builds and runs the game
int main(){

    // Initialize freeglut with default parameters
    int argc = 1;
    char* argv[1] = { (char*)"N/A" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    game::Game app; // Game application 

    try {
        // Initialize game
        app.Init();
        // Setup the main resources and scene in the game
        app.SetupResources();
        app.SetupScene();
        // Run game
        app.MainLoop();
    }
    catch (std::exception &e){
        PrintException(e);
    }

    return 0;
}
