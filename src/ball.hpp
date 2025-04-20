#include <raylib.h>

// A Singleton class of a golf ball
class Ball {
private:
    float Cd = 0.2; // Drag coefficient
    float mass = 0.04592623; // Ball mass (kg)
    float radius = 0.021335; // Ball radius (m)
    float A = PI*radius*radius; // Cross-sectional area (m^2)
    float I = 0.4*mass*radius*radius; // Moment of inertia
    float S = 8.0/3.0*PI*radius*radius*radius; // Magnus coefficient
    float u_k = 0.4; // Friction coefficent with ground
    Vector3 position = {0.0f, 0.0f, 0.0f};
    Vector3 velocity = {0.0f, 0.0f, 0.0f};
    Vector3 omega = {0.0f, 0.0f, 0.0f};

public:
    void SetPosition(Vector3 pos);
    void DrawBall();
    void UpdateBall(float delta);
};


    

