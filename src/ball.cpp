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
    DrawSphere(Vector3Add(this->position, (Vector3){0.0f,radius*3.0f,0.0f}), radius*3.0f, WHITE);
    // Draw Shadow on the ground
    Vector3 shadow = this->position;
    shadow.y = radius*3;
    DrawSphere(shadow, radius*3.0f, DARKGRAY);
    // Draw ball trail
    this->DrawTrail();
}

void Ball::UpdateBall(double delta) {
    // Update the trail
    if (this->state != REST) this->UpdateTrail(delta);

    // Check if ball is onground 
    if (this->position.y < Ball::radius) {
        if (this->state == FLIGHT) this->SetState(ROLLOUT);
        this->on_ground = true;
        this->position.y = Ball::radius;
        // bounce - for bounce dynamics see References/Golf ball landing bounce and roll on turf
        if (this->velocity.y < -1.0f) {
            // print initial velocities
            float vel = Vector3Length(this->velocity);
            float theta_1 = Vector3Angle(this->velocity, (Vector3){0.0f, -1.0f, 0.0}); // Angle between incoming velocity and vertical in rads
    
            float theta_c = 15.4f * vel * theta_1 / 18.6f / 44.4f; // Eq 18 from reference
            // initial horizontal velocity (x+z)
            float v1h = Vector2Length((Vector2){this->velocity.x, this->velocity.z});
            // initial horizontal rotation
            float w1h = Vector2Length((Vector2){this->omega.x, this->omega.z});
            // final horizontal velocity
            float v2h = 5.0/7.0*vel*sin(theta_1-theta_c) - 2.0*radius*w1h/7.0; // eq 16_1 from reference
            // Horizontal restitution
            float eh = v1h < 0.01 ? 0.0f : v2h / v1h;
            // final horizontal rotation
            float w2h = v2h/radius; // eq 16_4
            // horizontal omega restitution
            float ewh = w1h < 0.1 ? 0.0f : w2h/w1h;


            float e = 0.0f;
            if (this->velocity.y < -20.0f) e = 0.120;
            else e = 0.510f + 0.0375*this->velocity.y + 0.000903*this->velocity.y*this->velocity.y; // Eq 17 from reference
            this->velocity.y *= -e;
            this->velocity.x *= eh; //0.8f;
            this->velocity.z *= eh; //0.8f;

            this->omega.x *= ewh;
            this->omega.z *= ewh;

        }
        else {
            this->velocity.y = 0.0f;
        }

        if (Vector3Length(this->velocity) < 0.1 && Vector3Length(this->omega) < 1.0f) { // stop rolling
            this->velocity = Vector3Zero();
            this->omega = Vector3Zero();
            this->SetState(REST);
        }
    }
    else this->on_ground = false;
}


void Ball::SetState(ball_state st) {
    if (st == ROLLOUT) {
        this->carry = (int)Vector3Length(this->position);
    }

    this->state = st;
}

void Ball::DrawTrail() {
    if (this->num_trail_points > 1) {
        for(int i=0; i<num_trail_points-1; i++) {
            DrawLine3D(trail_points[i], trail_points[i+1], RED);
        }
    }

}

void Ball::UpdateTrail(float delta) {
    this->trail_time_accumulator += delta;
    if (this->trail_time_accumulator >= trail_resolution) { // Add a point to the trail point array
        this->AddTrailPoint(this->position);
        this->trail_time_accumulator = 0.0f;
    }

}

void Ball::ClearTrail() {
    this->num_trail_points = 0;
    this->trail_time_accumulator = 0.0f;
}

void Ball::AddTrailPoint(Vector3 point) {
    if (this->num_trail_points >= MAX_TRAIL_POINTS) return; // Don't add more than the maximum number of points
    this->trail_points[num_trail_points] = point;
    num_trail_points++;
}
