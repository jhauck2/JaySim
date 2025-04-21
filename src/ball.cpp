#include "ball.hpp"


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
    DrawSphere(this->position, radius*3.0f, RED);
}

void Ball::UpdateBall(double delta) {

}
