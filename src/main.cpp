#include <iostream>
#include <raylib.h>


std::string version = "V0.0.1";


int main() {

    // Initialization
    // ------------------------------------------------------------------------
    
    const int screenWidth = 800;
    const int screenHeight = 450;

    std::string title = "JaySim - " + version;

    InitWindow(screenWidth, screenHeight, title.c_str());

    SetTargetFPS(60);
    // ------------------------------------------------------------------------
    
    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // Some kind of state machine 
        // -------------------------------------------------------------------
        
        // Draw
        // -------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(RAYWHITE);

            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        // -------------------------------------------------------------------
    }

    // De-Initialization
    // ------------------------------------------------------------------------
    CloseWindow();
    // ------------------------------------------------------------------------
    

	return 0;
}

