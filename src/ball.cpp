#include "ball.hpp"


void Ball::SetPosition(Vector3 pos) {
    position.x = pos.x;
    position.y = pos.y;
    position.z = pos.z;
}

void Ball::DrawBall() {
    DrawSphere(position, radius*3.0f, RED);
}

void Ball::UpdateBall(float delta) {

}
