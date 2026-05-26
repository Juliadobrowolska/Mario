#pragma once

#include <cstdint>

class Animation
{
public:
    Animation() : speed(0.0f) {}
    void change_frame() {}
    void set_animation_speed(float i_speed) { speed = i_speed; }
    void update() {}

private:
    float speed;
};
  