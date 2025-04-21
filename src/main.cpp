#include <stdio.h>
#include <iostream>
#include <raylib.h>
#include "ball.hpp"
#include "dynamics.hpp"


std::string version = "V0.0.1";


int main() {

    // Initialization
    // ------------------------------------------------------------------------
    
    const int screenWidth = 1200;
    const int screenHeight = 675;

    std::string title = "JaySim - " + version;

    InitWindow(screenWidth, screenHeight, title.c_str());

    SetTargetFPS(60);

    // Define camera
    Camera camera = { { 0.0f, 10.0f, 10.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f }, 45.0f, 0 };
    
    // Create ball
    Ball ball1;
    ball1.position.y = 5.05f;
    ball1.position.x = -4.0f;
    ball1.velocity.x = 1.0f;
    ball1.omega.z = 0.0f;

    // Load in the course
    // ------------------------------------------------------------------------
    
    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // -------------------------------------------------------------------
        float delta = GetFrameTime();

        // Some kind of state machine 
        Dynamics::rk4(&ball1, delta);
        // Dynamics::simple_integral(&ball1, delta);
        // Check if ball is onground 
        if (ball1.position.y < Ball::radius) {
            ball1.on_ground = true;
            ball1.position.y = Ball::radius;
            if (ball1.velocity.y < -1.0f) {
                ball1.velocity.y *= -0.7f;
            }
            else {
                ball1.velocity.y = 0.0f;
            }
        }
        else ball1.on_ground = false;
        // Draw
        // -------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(WHITE);

            BeginMode3D(camera);

                DrawGrid(10, 1.0f);
                ball1.DrawBall();

            EndMode3D();
            char vel_text[50];
            char spin_text[50];
            sprintf(vel_text, "vel: %8.4f %8.4f %8.4f", ball1.velocity.x, ball1.velocity.y, ball1.velocity.z);
            sprintf(spin_text, "omg: %8.4f %8.4f %8.4f", ball1.omega.x, ball1.omega.y, ball1.omega.z);

            DrawText(vel_text, 20, 20, 14, BLACK);
            DrawText(spin_text, 20, 40, 14, BLACK);

        EndDrawing();
        // -------------------------------------------------------------------
    }

    // De-Initialization
    // ------------------------------------------------------------------------
    CloseWindow();
    // ------------------------------------------------------------------------
    

	return 0;
}

