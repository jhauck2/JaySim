#ifndef __BALL_H__
#define __BALL_H__

#ifdef PLATFORM_LINUX
#include <raylib.h>
#endif

#ifdef PLATFORM_WINDOWS
#include "raylib.h"
#include "raymath.h"
#endif

#define MAX_TRAIL_POINTS 100

// A Singleton class of a golf ball
class Ball {
public:

    typedef enum ball_state{
        REST,
        FLIGHT,
        ROLLOUT
    } ball_state;
    ball_state state = REST;

    // Physics Properties
    // -------------------------------------------------------------------------------------------------
    static constexpr double mass = 0.04592623; // Ball mass (kg)
    static constexpr double radius = 0.021335; // Ball radius (m)
    static constexpr double A = PI*radius*radius; // Cross-sectional area (m^2)
    static constexpr double I = 0.4*mass*radius*radius; // Moment of inertia
    static constexpr double u_k = 0.4; // Friction coefficent with ground
    static constexpr double u_kr = 0.2; // Friction of rolling while not slipping

    // Dynamics
    // -------------------------------------------------------------------------------------------------
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 velocity = {0.0f, 0.0f, 0.0f};
    Vector3 omega = {0.0f, 0.0f, 0.0f};

    float carry = 0.0f;

    bool on_ground = false;

    // Trail
    Vector3 trail_points[MAX_TRAIL_POINTS];
    int num_trail_points = 0;
    static float constexpr trail_resolution = 0.1f; // time resolution at which points are added to the trail
    float trail_time_accumulator = 0.0f; // time accumulator to trigger adding points to trail

    // Functions
    // -------------------------------------------------------------------------------------------------
    void SetPosition(Vector3 pos);
    Vector3 GetPosition();
    Vector3 GetVelocity();
    Vector3 GetOmega();
    void DrawBall();
    void UpdateBall(double delta);
    void SetState(ball_state st);
    void DrawTrail();
    void UpdateTrail(float delta);
    void ClearTrail();
    void AddTrailPoint(Vector3 point);
};

#endif
