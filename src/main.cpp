#include <stdio.h>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "ball.hpp"
#include "dynamics.hpp"
#include "button.hpp"
#include "jayShader.hpp"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#define GLSL_VERSION 330



std::string version = "V0.0.1";

// Ball initial conditions
Vector3 pos0 = {0.0f, 0.05f, 0.0f};
Vector3 vel0 = {0.0f, 0.0f, 0.0f};
Vector3 omg0 = {0.0f, 0.0f, 0.0f};

Vector3 velh = {40.0f, 20.0f, 0.0f};
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
}


int main() {

    // Initialization
    // ------------------------------------------------------------------------
    
    const int screenWidth = 1500;
    const int screenHeight = 825;

    SetConfigFlags(FLAG_MSAA_4X_HINT); // Enable Multi Sampling Anti Aliasing 4x (if available)

    std::string title = "JaySim - " + version;

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetWindowMonitor(0);

    // Define camera {position}, {look at}, {up direction}, FOV
    
    Camera camera = { 0 };
    camera.position = (Vector3){ -30.0f, 20.0f, 0.0f };
    camera.target = (Vector3){ 50.0f, 0.0f, 0.0f };
    camera.up = (Vector3){0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Create ball
    Ball ball1;
    resetBall(&ball1);

    // create buttons
    //Button resetButton((Vector2){20, screenWidth-100}, (Vector2){30, 80}, "Button Test");
    Button resetButton((Vector2){screenWidth-120, 20}, (Vector2){100, 30}, "Reset Ball");
    resetButton.callback = resetBall;
    
    Button hitButton((Vector2){screenWidth-120, 70}, (Vector2){100, 30}, "Hit Ball");
    hitButton.callback = hitBall;

    // Load Shaders
    // ------------------------------------------------------------------------
    // Load PBR shader and setup all required locations
    Shader shader;
    initShader(&shader, "Resources/Shaders/pbr.vs", "Resources/Shaders/pbr.fs");


    // Load in the course
    // ------------------------------------------------------------------------
    // Load Skybox
    Model skybox = LoadModel("Resources/Models/Skybox.glb"); // skybox  should be rendered at the camera position
    // Load Range
    Model range = LoadModel("Resources/Models/Range.glb");
    range.materials[0].shader = shader;

    // Setup materials[0].maps default parameters
    range.materials[0].maps[MATERIAL_MAP_ALBEDO].color = WHITE;
    range.materials[0].maps[MATERIAL_MAP_METALNESS].value = 0.5f;
    range.materials[0].maps[MATERIAL_MAP_ROUGHNESS].value = 0.5f;
    range.materials[0].maps[MATERIAL_MAP_OCCLUSION].value = 1.0f;
    range.materials[0].maps[MATERIAL_MAP_EMISSION].color = (Color){ 255, 162, 0, 255 };

    Vector3 range_pos = {180.0, 0.0, 0.0};
    range.transform = MatrixRotateXYZ((Vector3){0.0f, PI/2.0f, 0.0f});
    Model monke = LoadModel("Resources/Models/Monke.glb");
    Vector3 monke_pos = {0.0f, 1.0f, 2.0f};

    // Create some lights
    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -1.0f, 1.0f, -2.0f }, (Vector3){ 0.0f, 0.0f, 0.0f }, YELLOW, 4.0f, shader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2.0f, 1.0f, 1.0f }, (Vector3){ 0.0f, 0.0f, 0.0f }, GREEN, 3.3f, shader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -2.0f, 1.0f, 1.0f }, (Vector3){ 0.0f, 0.0f, 0.0f }, RED, 8.3f, shader);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 1.0f, 1.0f, -2.0f }, (Vector3){ 0.0f, 0.0f, 0.0f }, BLUE, 2.0f, shader);


    SetTargetFPS(60);
    
    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // -------------------------------------------------------------------
        float delta = GetFrameTime();

        //UpdateCamera(&camera, CAMERA_ORBITAL);

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

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
            
            ClearBackground(BLACK);

            BeginMode3D(camera);

                // Set range model texture tiling and emissive color parameters on shader
                Vector4 rangeEmissiveColor = ColorNormalize(range.materials[0].maps[MATERIAL_MAP_EMISSION].color);
                SetShaderValue(shader, emissiveColorLoc, &rangeEmissiveColor, SHADER_UNIFORM_VEC4);
                float emissiveIntensity = 0.01f;
                SetShaderValue(shader, emissiveIntensityLoc, &emissiveIntensity, SHADER_UNIFORM_FLOAT);

                DrawModel(range, range_pos, 1.0f, WHITE);

                //DrawModel(monke, monke_pos, 1.0f, WHITE);
                DrawModel(skybox, camera.position, 1.0f, WHITE);
                ball1.DrawBall();
                
            EndMode3D();
            char vel_text[50];
            char spin_text[50];
            char dist_text[50];
            sprintf(vel_text, "vel: %8.4f %8.4f %8.4f", ball1.velocity.x, ball1.velocity.y, ball1.velocity.z);
            sprintf(spin_text, "omg: %8.4f %8.4f %8.4f", ball1.omega.x, ball1.omega.y, ball1.omega.z);
            sprintf(dist_text, "distance: %d", (int)Vector3Length(ball1.position));

            DrawText(vel_text, 20, 20, 14, BLACK);
            DrawText(spin_text, 20, 40, 14, BLACK);
            DrawText(dist_text, 20, 60, 14, BLACK);

            // Draw Buttons
            resetButton.DrawButton();
            hitButton.DrawButton();

        EndDrawing();
        // -------------------------------------------------------------------
    }

    // De-Initialization
    // ------------------------------------------------------------------------
    range.materials[0].shader = (Shader){ 0 };
    UnloadMaterial(range.materials[0]);
    range.materials[0].maps = NULL;
    UnloadModel(range);
    
    UnloadShader(shader);       // Unload Shader

    CloseWindow();
    // ------------------------------------------------------------------------
    

	return 0;
}

