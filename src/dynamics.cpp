#include "dynamics.hpp"
#include "raymath.h"
#include <stdio.h>


int print_timer = 0;


std::vector<Vector3> get_x_dot(Vector3 pos, Vector3 vel, Vector3 om, bool on_ground) {
    // Floor normal. Assume flat ground but this should work with more complex geometry
    Vector3 floor_norm = {0.0f, 1.0f, 0.0f};

    Vector3 F_g = {0.0f, -9.81f*Ball::mass, 0.0f}; // Gravity
    Vector3 F_m = {0.0f, 0.0f, 0.0f}; // Magnus force
    Vector3 F_d = {0.0f, 0.0f, 0.0f}; // Drag force
    Vector3 F_f = {0.0f, 0.0f, 0.0f}; // Frictional force
    Vector3 F_gd = {0.0f, 0.0f, 0.0f}; // Grass drag force

    Vector3 T_d = {0.0f, 0.0f, 0.0f}; // Viscous torque
    Vector3 T_f = {0.0f, 0.0f, 0.0f}; // Frictional torque
    Vector3 T_g = {0.0f, 0.0f, 0.0f}; // Grass frictional torque

    

    if (on_ground) {
        // Force of viscous drag from grass
        F_gd = Vector3Scale(vel, -6*PI*Ball::radius*Dynamics::nu_g);
        F_gd.y = 0;

        // velocity of the bottom of the ball relative to the ground with spin included
        Vector3 b_vel = Vector3CrossProduct(floor_norm, om);
        b_vel = Vector3Scale(b_vel, Ball::radius);
        b_vel = Vector3Add(b_vel, vel);
        if (Vector3Length(b_vel) < 0.05f) { // rolling without slipping
            b_vel = Vector3Normalize(vel);
            F_f = Vector3Scale(b_vel, -Ball::u_k*Ball::mass*9.81);
        }
        else { // Ball slipping on the ground
            // normalize this velocity to get direction
            b_vel = Vector3Normalize(b_vel); 
            F_f = Vector3Scale(b_vel, -Ball::u_k*Ball::mass*9.81);
            // torque on the ball from the frictional force
            T_f = Vector3CrossProduct(Vector3Scale(floor_norm, -Ball::radius), F_f);
        }

        // Viscoous torque
        T_g = Vector3Normalize(om);
        T_g = Vector3Scale(T_g, -6.0f*PI*Ball::radius*Dynamics::nu_g);

    }
    else { // Ball in air
        float speed = Vector3Length(vel);
        float spin = 0.0f;
        if (speed > 0.5f) spin = Vector3Length(om)*Ball::radius/speed;

        float Re = speed*Ball::radius*2/Dynamics::nu_k; //Reynolds Number

        // Magnus and drag coefficients derived from References/proceedings-02-00238-v2.pdf
        float S = 0.5*Dynamics::rho*PI*Ball::radius*Ball::radius*Ball::radius*(-3.25f*spin + 1.99); // Magnus Coefficient
        float Cd = 0.0f; // Drag Coefficient
        if (Re < 87500.0f) {
            Cd = 0.000000000129f*Re*Re - 0.0000259f*Re + 1.50f;
        }
        else {
            Cd = 0.0000000000191f*Re*Re - 0.0000054f*Re + 0.56f;
        }

        // Magnus Force
        F_m = Vector3CrossProduct(om, vel);
        F_m = Vector3Scale(F_m, S);
        // Viscous torque - factor of 480 used to get correct decay rate
        // Reference 3D Golf Ball Simulation pdf
        float Cdm = Cd / 3.0f;
        T_d = Vector3Normalize(om);
        T_d = Vector3Scale(T_d, -0.5f*Cdm*Dynamics::rho*Ball::A);
        //T_d = Vector3Scale(om, S*480.0f*Dynamics::nu);
        // Drag
        F_d = Vector3Multiply(vel, vel);
        F_d = Vector3Scale(F_d, Cd*Dynamics::rho*Ball::A/2.0f);
    }

    Vector3 F = Vector3Subtract(F_g, F_d); // Total forces
    F = Vector3Add(F, F_m);
    F = Vector3Add(F, F_f); 
    F = Vector3Add(F,F_gd); 
    
    Vector3 T = Vector3Zero(); // Total Torque
    T = Vector3Add(T, T_d); 
    T = Vector3Add(T, T_f); 
    T = Vector3Add(T, T_g);


    Vector3 pos_dot = vel;
    Vector3 vel_dot = Vector3Scale(F, 1.0f/Ball::mass);
    Vector3 om_dot = Vector3Scale(T, 1.0f/Ball::I);
    
    std::vector<Vector3> x_dot = {pos_dot, vel_dot, om_dot};

    return x_dot;
}


// Runge-Kutta 4th degree Inegration Method
void Dynamics::rk4(Ball *ball, double delta_time) {
    // Make a copy of initial ball conditions
    std::vector<Vector3> x0 = {ball->position, ball->velocity, ball->omega};
    std::vector<Vector3> x = {ball->position, ball->velocity, ball->omega};
    
    std::vector<Vector3> K1 = { {0.0f, 0.0f, 0.0f},
                                {0.0f, 0.0f, 0.0f},
                                {0.0f, 0.0f, 0.0f} };
    std::vector<Vector3> K2;
    std::vector<Vector3> K3;
    std::vector<Vector3> K4;

    K2.assign(K1.begin(), K1.end());
    K3.assign(K1.begin(), K1.end());
    K4.assign(K1.begin(), K1.end());

    K1 = get_x_dot(x0[0], x0[1], x0[2], ball->on_ground);

    // K2 = step*get_x_dot(x0+0.5*K1)
    for (int i=0; i<3; i++) {
        K1[i] = Vector3Scale(K1[i], delta_time);
        x[i] = Vector3Scale(K1[i], 0.5f);
        x[i] = Vector3Add(x[i], x0[i]);
    }

    K2 = get_x_dot(x[0], x[1], x[2], ball->on_ground);

    // K3 = step*get_x_dot(x0 + 0.5*K2)
    for (int i=0; i<3; i++) {
        K2[i] = Vector3Scale(K2[i], delta_time);
        x[i] = Vector3Scale(K2[i], 0.5f);
        x[i] = Vector3Add(x[i], x0[i]);
    }

    K3 = get_x_dot(x[0], x[1], x[2], ball->on_ground);
    for (int i=0; i<3; i++) {
        K3[i] = Vector3Scale(K3[i], delta_time);
        x[i] = Vector3Add(K3[i], x0[i]);
    }

    K4 = get_x_dot(x[0], x[1], x[2], ball->on_ground);
    for (int i=0; i<3; i++) {K4[i] = Vector3Scale(K4[i], delta_time);}

    // value = x0 + K1/6 + K2 /3 + K3/3 + K4/6
    for (int i=0; i<3; i++) {
        K1[i] = Vector3Scale(K1[i], 1.0f/6.0f);
        K2[i] = Vector3Scale(K2[i], 1.0f/3.0f);
        K3[i] = Vector3Scale(K3[i], 1.0f/3.0f);
        K4[i] = Vector3Scale(K4[i], 1.0f/6.0f);

        x[i] = Vector3Add(x0[i], K1[i]);
        x[i] = Vector3Add(x[i], K2[i]);
        x[i] = Vector3Add(x[i], K3[i]);
        x[i] = Vector3Add(x[i], K4[i]);
    }

    // put the values of x into ball
    ball->position = x[0];
    ball->velocity = x[1];
    ball->omega = x[2];

}


void Dynamics::simple_integral(Ball *ball, double delta_time) {
    // Make a copy of initial ball conditions
    std::vector<Vector3> x0 = {ball->position, ball->velocity, ball->omega};
    std::vector<Vector3> x1 = {ball->position, ball->velocity, ball->omega};

    std::vector<Vector3> x_dot = { {0.0f, 0.0f, 0.0f},
                                 {0.0f, 0.0f, 0.0f},
                                 {0.0f, 0.0f, 0.0f} };
    
    x_dot = get_x_dot(x0[0], x0[1], x0[2], ball->on_ground);
    for (int i=0; i<3; i++) {
        x1[i] = Vector3Scale(x_dot[i], delta_time);
        x1[i] = Vector3Add(x1[i], x0[i]);
    }

    ball->position = x1[0];
    ball->velocity = x1[1];
    ball->omega = x1[2];

}
