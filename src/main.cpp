#include <stdio.h>
#include <iostream>
#include <raylib.h>
#include "ball.hpp"
#include "dynamics.hpp"
#include "button.hpp"


std::string version = "V0.0.1";

// Ball initial conditions
Vector3 pos0 = {0.0f, 0.05f, 0.0f};
Vector3 vel0 = {0.0f, 0.0f, 0.0f};
Vector3 omg0 = {0.0f, 0.0f, 0.0f};

Vector3 velh = {20.0f, 15.0f, 0.0f};
Vector3 omgh = {0.0f, 0.0f, 600.0f};

void resetBall(std::any b) {
    Ball *ball;
    try {
        ball = std::any_cast<Ball*>(b);
    }
    catch (const std::bad_any_cast& e) {
        std::cout << "1) " << e.what() << '\n';
    }
    ball->position = pos0;
    ball->velocity = vel0;
    ball->omega = omg0;
}

void hitBall(std::any b) {
    Ball *ball;
    try {
        ball = std::any_cast<Ball*>(b);
    }
    catch (const std::bad_any_cast& e) {
        std::cout << "1) " << e.what() << '\n';
    }
    ball->position = pos0;
    ball->velocity = velh;
    ball->omega = omgh;
    ball->slipping = true;
}


int main() {

    // Initialization
    // ------------------------------------------------------------------------
    
    const int screenWidth = 1500;
    const int screenHeight = 825;

    std::string title = "JaySim - " + version;

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetWindowMonitor(0);

    SetTargetFPS(60);

    // Define camera {position}, {look at}, {up direction}, FOV
    Camera camera = { { -20.0f, 20.0f, 20.0f }, { 10.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f }, 80.0f, 0 };
    
    // Create ball
    Ball ball1;
    resetBall(&ball1);

    // create buttons
    //Button resetButton((Vector2){20, screenWidth-100}, (Vector2){30, 80}, "Button Test");
    Button resetButton((Vector2){screenWidth-120, 20}, (Vector2){100, 30}, "Reset Ball");
    resetButton.callback = resetBall;
    
    Button hitButton((Vector2){screenWidth-120, 70}, (Vector2){100, 30}, "Hit Ball");
    hitButton.callback = hitBall;

    // Load in the course
    // ------------------------------------------------------------------------
    // Load monke
    Model monke = LoadModel("Resources/Monke.glb");
    Vector3 monke_pos = {0.0f, 0.0f, 10.0f};
    BoundingBox monke_bounds = GetMeshBoundingBox(monke.meshes[0]);
    
    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // -------------------------------------------------------------------
        float delta = GetFrameTime();

        // Some kind of state machine 
        Dynamics::rk4(&ball1, delta);
        // Dynamics::simple_integral(&ball1, delta);
        ball1.UpdateBall(delta);

        // Handle button clicks
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse_pos = GetMousePosition();
            // Reset Button
            if (CheckCollisionPointRec(mouse_pos, (Rectangle){resetButton.position.x, resetButton.position.y, resetButton.size.x, resetButton.size.y})) {
                resetButton.Click();
                (*(resetButton.callback))(&ball1);
            }
            // Hit Button
            if (CheckCollisionPointRec(mouse_pos, (Rectangle){hitButton.position.x, hitButton.position.y, hitButton.size.x, hitButton.size.y})) {
                hitButton.Click();
                (*(hitButton.callback))(&ball1);
            }
        }
        

        // Draw
        // -------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(WHITE);

            BeginMode3D(camera);

                DrawGrid(100, 5.0f);
                ball1.DrawBall();
                DrawModel(monke, monke_pos, 1.0f, WHITE);

            EndMode3D();
            char vel_text[50];
            char spin_text[50];
            sprintf(vel_text, "vel: %8.4f %8.4f %8.4f", ball1.velocity.x, ball1.velocity.y, ball1.velocity.z);
            sprintf(spin_text, "omg: %8.4f %8.4f %8.4f", ball1.omega.x, ball1.omega.y, ball1.omega.z);

            DrawText(vel_text, 20, 20, 14, BLACK);
            DrawText(spin_text, 20, 40, 14, BLACK);

            // Draw Buttons
            resetButton.DrawButton();
            hitButton.DrawButton();

        EndDrawing();
        // -------------------------------------------------------------------
    }

    // De-Initialization
    // ------------------------------------------------------------------------
    CloseWindow();
    // ------------------------------------------------------------------------
    

	return 0;
}

