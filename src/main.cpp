#include "ball.hpp"
//#include <boost/thread.hpp>
#include "button.hpp"
#include "dynamics.hpp"
#include <iostream>
#include <mutex>
#include "shotData.hpp"
#include "shotParser.hpp"
#include <stdio.h>
#include <thread>
#include "TCPSocket.hpp"

#ifdef PLATFORM_LINUX
#include <raylib.h>
#include <raymath.h>
#endif

#ifdef PLATFORM_WINDOWS
#include "raylib.h"
#include "raymath.h"
#endif

//#include "jayShader.hpp"

//#define RLIGHTS_IMPLEMENTATION
//#include "rlights.h"

#define GLSL_VERSION 330

// Display Settings
int display_units = 1; // 0 -> Meters, 1 -> Yards/MPH
bool dynamics_debug = false; // when true, displays velocity and angular velocity vectors

std::string version = "V0.0.1";


// Button Functions
// ------------------------------------------------------------------------
void resetBall(std::any b) {
    // Ball initial conditions
    Vector3 pos0 = {0.0f, 0.05f, 0.0f};
    Vector3 vel0 = {0.0f, 0.0f, 0.0f};
    Vector3 omg0 = {0.0f, 0.0f, 0.0f};

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
    ball->state = Ball::REST;
    ball->carry = 0.0;
    ball->ClearTrail();
}

void hitBall(std::any b) {
    // 8 iron test shot - 100 mph, 20.8 deg launch, 1.7 deg horz launch, 7494 rpm, 2.7 degree spin axis offset
    Vector3 pos0 = {0.0f, 0.05f, 0.0f};
    Vector3 velh = {44.7f*cos(20.8f*PI/180.0f)*cos(1.7f*PI/180.0f), 44.7f*sin(20.8f*PI/180.0f), 44.7f*(float)sin(1.7*PI/180.0f)};
    Vector3 omgh = {0.0f, 784.0f*(float)sin(2.7*PI/180.0), 784.0f*(float)cos(2.7*PI/180.0)};

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
    ball->carry = 0.0;
    ball->ClearTrail();
    ball->state = Ball::FLIGHT;
    ball->AddTrailPoint(ball->position);
}

void hitBallJson(std::any b) {
    Vector3 pos0 = {0.0f, 0.05f, 0.0f};

    Ball *ball;
    try {
        ball = std::any_cast<Ball*>(b);
    }
    catch (const std::bad_any_cast& e) {
        std::cout << "2) " << e.what() << '\n';
    }

    // Load shot data from file
    // This is just a test for now
    std::string path = "Resources/json/test_shot.json";
    t_shot_data shot_data = parse_json_shot_file(path);
    ball->position = pos0;
    ball->velocity = (Vector3){shot_data.ball_data.speed, 0.0f, 0.0f};
    ball->velocity = Vector3RotateByAxisAngle(ball->velocity, (Vector3){0.0f, 0.0f, 1.0f}, shot_data.ball_data.VLA*PI/180.0);
    ball->velocity = Vector3RotateByAxisAngle(ball->velocity, (Vector3){0.0f, 1.0f, 0.0f}, shot_data.ball_data.HLA*PI/180.0);
    ball->velocity = Vector3Scale(ball->velocity, 0.44704); // convert to m/s
    ball->omega = (Vector3){0.0f, 0.0f, shot_data.ball_data.totalSpin};
    ball->omega = Vector3RotateByAxisAngle(ball->omega, (Vector3){1.0f, 0.0f, 0.0f}, -shot_data.ball_data.spinAxis*PI/180.0);
    ball->omega = Vector3Scale(ball->omega, 0.10472); // convert to rad/s

    ball->state = Ball::FLIGHT;
    ball->carry = 0.0;
    ball->ClearTrail();
    ball->AddTrailPoint(ball->position);
}

void hitFromBallData(Ball *ball, t_ball_data *ball_data) {
    Vector3 pos0 = {0.0f, 0.05f, 0.0f};

    ball->position = pos0;
    ball->velocity = (Vector3){ball_data->speed, 0.0f, 0.0f};
    ball->velocity = Vector3RotateByAxisAngle(ball->velocity, (Vector3){0.0f, 0.0f, 1.0f}, ball_data->VLA*PI/180.0);
    ball->velocity = Vector3RotateByAxisAngle(ball->velocity, (Vector3){0.0f, 1.0f, 0.0f}, ball_data->HLA*PI/180.0);
    ball->velocity = Vector3Scale(ball->velocity, 0.44704); // convert to m/s
    ball->omega = (Vector3){0.0f, 0.0f, ball_data->totalSpin};
    ball->omega = Vector3RotateByAxisAngle(ball->omega, (Vector3){1.0f, 0.0f, 0.0f}, -ball_data->spinAxis*PI/180.0);
    ball->omega = Vector3Scale(ball->omega, 0.10472); // convert to rad/s

    ball->carry = 0.0;
    ball->ClearTrail();
    ball->state = Ball::FLIGHT;
    ball->AddTrailPoint(ball->position);
}

// Main
// ------------------------------------------------------------------------
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
    
    Camera camera = { {0} };
    camera.position = (Vector3){ -10.0f, 5.0f, 0.0f };
    camera.target = (Vector3){ 40.0f, 0.0f, 0.0f };
    camera.up = (Vector3){0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
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
    
    Button hitJsonButton((Vector2){screenWidth-120, 120}, (Vector2){100, 30}, "Hit Json");
    hitJsonButton.callback = hitBallJson;

    // Init LM connection indicator
    Texture2D no_lm_tex = LoadTexture("Resources/Sprites/No_LM_Indicator_small.png");
    Texture2D lm_tex = LoadTexture("Resources/Sprites/LM_Indicator_small.png");

    // Load Shaders
    // ------------------------------------------------------------------------
    // Not yet implemented

    // Load in the course
    // ------------------------------------------------------------------------
    // Load Skybox
    Model skybox = LoadModel("Resources/Models/Skybox.glb"); // skybox  should be rendered at the camera position
    // Load Range
    Model range = LoadModel("Resources/Models/Range.glb");
    Vector3 range_pos = {180.0, 0.0, 0.0};
    range.transform = MatrixRotateXYZ((Vector3){0.0f, PI/2.0f, 0.0f});

    // Set up TCP Socket
    TCPSocket socket;
    socket.init_socket();

    t_shared_data shared_thread_data;
    t_ball_data ball_data; // ball data to be shared between main thread and socket thread
    ball_data.status = STALE;
    bool close_socket = false; // how we tell the socket to close
    LM_status lm_connection_status = NOT_CONNECTED;
    //provide mutexes for ball data and socket close indicator
    std::mutex ball_data_mtx;
    std::mutex close_socket_mtx;
    std::mutex lm_status_mtx;
    shared_thread_data.ball_data = &ball_data;
    shared_thread_data.should_close = &close_socket;
    shared_thread_data.lm_status = &lm_connection_status;
    shared_thread_data.ball_mtx = &ball_data_mtx;
    shared_thread_data.close_mtx = &close_socket_mtx;
    shared_thread_data.status_mtx = &lm_status_mtx;

    // run this as a thread
    std::thread socket_thread(&TCPSocket::run_socket, &socket, &shared_thread_data);


    SetTargetFPS(60);
    
    // Main Loop
    while (!WindowShouldClose()) {
        // Update
        // -------------------------------------------------------------------
        float delta = GetFrameTime();

        //UpdateCamera(&camera, CAMERA_ORBITAL);

        // Some kind of state machine 
        Dynamics::rk4(&ball1, delta);
        // Dynamics::simple_integral(&ball1, delta);
        ball1.UpdateBall(delta);

        // Update button states
        resetButton.UpdateButton(&ball1);
        hitButton.UpdateButton(&ball1);
        hitJsonButton.UpdateButton(&ball1);
         

        // Handle shot from TCPSocket
        if (ball_data_mtx.try_lock()){
            if (ball1.state == Ball::REST){
                if (ball_data.status == INUSE) ball_data.status = STALE;
                else if (ball_data.status == VALID) {
                    printf("Valid ball data recieved from socket\n");
                    hitFromBallData(&ball1, &ball_data);
                    ball_data.status = INUSE;
                }
            }
            ball_data_mtx.unlock();
        } // Otherwise, the socket still has a lock on the ball data
        
        
        // Draw
        // -------------------------------------------------------------------
        BeginDrawing();
            
            ClearBackground(BLACK);

            BeginMode3D(camera);

                DrawModel(range, range_pos, 1.0f, WHITE);
                DrawModel(skybox, camera.position, 1.0f, WHITE);
                ball1.DrawBall();
                
            EndMode3D();

            char vel_text[50];
            char spin_text[50];
            char dist_text[50];
            char carry_text[50];
            if (display_units == 0) {
                sprintf(vel_text, "vel: %8.4f %8.4f %8.4f m/s", ball1.velocity.x, ball1.velocity.y, ball1.velocity.z);
                sprintf(dist_text, "distance: %d m", (int)Vector3Length(ball1.position));
                sprintf(carry_text, "carry: %d m", (int)ball1.carry);
            }
            else {
                Vector3 vel_mph = Vector3Scale(ball1.velocity, 2.237);
                sprintf(vel_text, "vel: %8.4f %8.4f %8.4f mph", vel_mph.x, vel_mph.y, vel_mph.z);
                sprintf(dist_text, "distance: %d yd", (int)(Vector3Length(ball1.position) * 1.09361));
                sprintf(carry_text, "carry: %d yd", (int)(ball1.carry*1.09361));
            }
            sprintf(spin_text, "omg: %8.4f %8.4f %8.4f rad/s", ball1.omega.x, ball1.omega.y, ball1.omega.z);

            DrawText(carry_text, 20, 20, 14, BLACK);
            DrawText(dist_text, 20, 40, 14, BLACK);
            if (dynamics_debug) {
                DrawText(vel_text, 20, 70, 14, BLACK);
                DrawText(spin_text, 20, 90, 14, BLACK);
            }
            

            // Draw Buttons
            resetButton.DrawButton();
            hitButton.DrawButton();
            hitJsonButton.DrawButton();

            // Draw LM status indicator
            Vector2 indicator_pos = {screenWidth - 84, screenHeight - 84};
            lm_status_mtx.lock();
            if (lm_connection_status == NOT_CONNECTED){
                DrawTexture(no_lm_tex, indicator_pos.x, indicator_pos.y, RED);
            }
            else if (lm_connection_status == CONNECTED) {
                DrawTexture(lm_tex, indicator_pos.x, indicator_pos.y, GREEN);
            }
            else if (lm_connection_status == READY) {
                DrawTexture(lm_tex, indicator_pos.x, indicator_pos.y, GREEN);
            }
            lm_status_mtx.unlock();
            
            

        EndDrawing();
        // -------------------------------------------------------------------
    }

    // De-Initialization
    // ------------------------------------------------------------------------
    // Tell the socket thread to close
    printf("Telling the socket thread to close\n");
    close_socket_mtx.lock();
        close_socket = true;
    close_socket_mtx.unlock();

    // join the socket thread
    socket_thread.join();
    printf("Socket thread has closed\n");

    UnloadModel(range);

    CloseWindow();
    // ------------------------------------------------------------------------
    

	return 0;
}

