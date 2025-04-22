#ifndef __BALL_H__
#define __BALL_H__

#include <raylib.h>

// A Singleton class of a golf ball
class Ball {
public:
    static constexpr double Cd = 0.2; // Drag coefficient
    static constexpr double mass = 0.04592623; // Ball mass (kg)
    static constexpr double radius = 0.021335; // Ball radius (m)
    static constexpr double A = PI*radius*radius; // Cross-sectional area (m^2)
    static constexpr double I = 0.4*mass*radius*radius; // Moment of inertia
    static constexpr double S = 8.0/3.0*PI*radius*radius*radius; // Magnus coefficient
    static constexpr double u_k = 0.4; // Friction coefficent with ground
    static constexpr double u_kr = 0.2; // Friction of rolling while not slipping
    bool slipping = true;
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 velocity = {0.0f, 0.0f, 0.0f};
    Vector3 omega = {0.0f, 0.0f, 0.0f};

    bool on_ground = false;

    void SetPosition(Vector3 pos);
    Vector3 GetPosition();
    Vector3 GetVelocity();
    Vector3 GetOmega();
    void DrawBall();
    void UpdateBall(double delta);
};

#endif
