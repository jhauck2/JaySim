#include <iostream>
#include <raylib.h>
#include "ball.hpp"


std::string version = "V0.0.1";


int main() {

    // Initialization
    // ------------------------------------------------------------------------
    
    const int screenWidth = 800;
    const int screenHeight = 450;

    std::string title = "JaySim - " + version;

    InitWindow(screenWidth, screenHeight, title.c_str());

    SetTargetFPS(60);

    // Define camera
    Camera camera = { { 0.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f }, 45.0f, 0 };
    
    // Create ball
    Ball ball1;

    // Load in the course
    // ------------------------------------------------------------------------
    
    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // -------------------------------------------------------------------
        // Some kind of state machine 
        //
        
        // Draw
        // -------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(WHITE);

            BeginMode3D(camera);

                DrawGrid(10, 1.0f);
                ball1.DrawBall();

            EndMode3D();

            //DrawText("Congrats! You created your first window!", 220, 40, 20, LIGHTGRAY);

        EndDrawing();
        // -------------------------------------------------------------------
    }

    // De-Initialization
    // ------------------------------------------------------------------------
    CloseWindow();
    // ------------------------------------------------------------------------
    

	return 0;
}

