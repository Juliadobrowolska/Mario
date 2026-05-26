#pragma once
#include <chrono>
#include <vector>
#include <array>

constexpr unsigned char  CELL_SIZE     = 80;
constexpr unsigned char  SCREEN_RESIZE = 1;
constexpr unsigned short SCREEN_HEIGHT = 1280; // 16 komórek * 80 pikseli = 1280 pikseli
constexpr unsigned short SCREEN_WIDTH  = 1280; // 16 komórek * 80 pikseli = 1280 pikseli

// Timery i Mechaniki Studenta
constexpr unsigned char STUDENT_BLINKING = 4;
constexpr unsigned short STUDENT_DEATH_DURATION = 32;
constexpr unsigned char STUDENT_TURBO_DURATION = 64; 
constexpr unsigned char STUDENT_IMMUNITY_DURATION = 128; 
constexpr unsigned char STUDENT_JUMP_TIMER = 8;

// Animacje i Przeciwnicy
constexpr unsigned short ECTS_ANIMATION_SPEED = 4;
constexpr unsigned short ENTITY_UPDATE_AREA = 64;
constexpr unsigned short GRADESHEET_DEATH_DURATION = 32;
constexpr unsigned short GRADESHEET_WALK_ANIMATION_SPEED = 32;
constexpr unsigned short DEAN_GET_OUT_ANIMATION_SPEED = 4;
constexpr unsigned short DEAN_GET_OUT_DURATION = 512;
constexpr unsigned short DEAN_WALK_ANIMATION_SPEED = 32;
constexpr unsigned short STUDENT_WALK_ANIMATION_SPEED = 4;
constexpr unsigned short MAIL_BLOCK_ANIMATION_SPEED = 8;

// Czas trwania klatki (60 FPS)
constexpr std::chrono::microseconds FRAME_DURATION(16667);

constexpr float GRAVITY               = 0.25f;
constexpr float MAX_VERTICAL_SPEED    = 8.0f;

constexpr float STUDENT_WALK_SPEED    = 2.0f;
constexpr float STUDENT_ACCELERATION  = 0.25f;
constexpr float STUDENT_JUMP_SPEED    = -4.0f;

constexpr float GRADESHEET_SPEED      = 1.0f;
constexpr float DEAN_SPEED            = 1.0f;
constexpr float DEAN_LUGGAGE_SPEED    = 4.0f;
constexpr float ENERGYDRINK_SPEED     = 1.0f;
constexpr float BRICK_PARTICLE_SPEED  = 4.0f;
constexpr float ECTS_JUMP_SPEED       = -6.0f;

enum class Cell : unsigned char
{
    Empty = 0,
    Floor1, 
    Floor2,
    Floor3, 
    Platform, 
    MailBlock, 
    ActivatedMailBlock,
    Wall 
};

struct Object
{
    float x;
    float y;
    float horizontal_speed;
    float vertical_speed;
    unsigned int current_anim_frame = 0;

    explicit Object(float i_x, float i_y, float i_horizontal_speed = 0.0f, float i_vertical_speed = 0.0f)
        : x(i_x)
        , y(i_y)
        , horizontal_speed(i_horizontal_speed)
        , vertical_speed(i_vertical_speed)
    {}
};

using Map = std::vector<std::array<Cell, SCREEN_HEIGHT / CELL_SIZE>>;