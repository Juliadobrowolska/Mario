#pragma once

constexpr unsigned char CELL_SIZE = 80;

constexpr unsigned short SCREEN_HEIGHT = 240;
constexpr unsigned short SCREEN_WIDTH = 320;

constexpr unsigned short ENTITY_UPDATE_AREA = 64;

constexpr float GRAVITY = 0.5f;
constexpr float MAX_VERTICAL_SPEED = 8.0f;

constexpr float STUDENT_BOUNCE_SPEED = 8.0f;
constexpr float STUDENT_JUMP_SPEED = -8.0f;
constexpr float STUDENT_WALK_SPEED = 3.0f;

constexpr float GOOMBA_SPEED = 1.0f;
constexpr unsigned char GOOMBA_WALK_ANIMATION_SPEED = 8;
constexpr unsigned short GOOMBA_DEATH_DURATION = 32;

constexpr float KOOPA_SPEED = 1.5f;
constexpr float KOOPA_SHELL_SPEED = 8.0f;
constexpr unsigned char KOOPA_GET_OUT_ANIMATION_SPEED = 4;
constexpr unsigned char KOOPA_WALK_ANIMATION_SPEED = 8;
constexpr unsigned short KOOPA_GET_OUT_DURATION = 512;

enum class Cell
{
    ActivatedMailBlock,
    Brick,
    Empty,
    Platform,
    MailBlock,
    Wall
};

extern bool hard_mode;