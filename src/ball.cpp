#define EPSILON 0.001f

#include "ball.hpp"
#include "raymath.h"
#include <stdio.h>


void Ball::SetPosition(Vector3 pos) {
    position.x = pos.x;
    position.y = pos.y;
    position.z = pos.z;
}

Vector3 Ball::GetPosition() {
    return this->position;
}

Vector3 Ball::GetVelocity() {
    return this->velocity;
}

Vector3 Ball::GetOmega() {
    return this->omega;
}

void Ball::DrawBall() {
    DrawSphere(this->position, radius*5.0f, RED);
    // Draw Shadow on the ground
    Vector3 shadow = this->position;
    shadow.y = 0;
    DrawSphere(shadow, radius*3.0f, GRAY);
}

void Ball::UpdateBall(double delta) {
    // Check if ball is onground 
    if (this->position.y < Ball::radius) {
        this->on_ground = true;
        this->position.y = Ball::radius;
        //slipping
        // velocity of the bottom of the ball relative to the ground with spin included
        Vector3 b_vel = Vector3RotateByAxisAngle(this->omega, (Vector3){0.0,1.0,0.0}, PI/2.0);
        b_vel = Vector3Scale(b_vel, Ball::radius);
        b_vel = Vector3Add(b_vel, this->velocity);
        b_vel.y = 0;
        if (this->slipping && Vector3Equals(b_vel, (Vector3){0.0f,0.0f,0.0f}) ) { 
            this->slipping = false; 
            printf("rollng without slipping\n");
        }// rolling without slipping

        // bounce
        if (this->velocity.y < -1.0f) {
            float e = 0.0f;
            if (this->velocity.y < -20.0f) e = 0.120;
            else e = 0.510f + 0.0375*this->velocity.y + 0.000903*this->velocity.y*this->velocity.y;
            this->velocity.y *= -e;
            this->velocity.x *= 0.8f;
            this->velocity.z *= 0.8f;

            this->omega = Vector3Scale(this->omega, 0.8);
        }
        else {
            this->velocity.y = 0.0f;
        }

        if (Vector3Length(this->velocity) < 0.5) {
            this->velocity = Vector3Scale(this->velocity, 0.9);
        }
    }
    else this->on_ground = false;
}
