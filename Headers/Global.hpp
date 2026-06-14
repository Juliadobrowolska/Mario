#pragma once
#include <chrono>
#include <vector>
#include <array>

constexpr unsigned char  CELL_SIZE     = 80;
constexpr unsigned char  SCREEN_RESIZE = 1; 
constexpr unsigned short SCREEN_HEIGHT = 1280; // 16 kratek * 80 pikseli = 1280
constexpr unsigned short SCREEN_WIDTH  = 1280; // 16 kratek * 80 pikseli = 1280

// Stałe silnika fizyki dla Studenta
constexpr float GRAVITY               = 0.25f;
constexpr float MAX_VERTICAL_SPEED    = 8.0f;
constexpr float STUDENT_WALK_SPEED    = 4.0f;
constexpr float STUDENT_ACCELERATION  = 0.25f;
constexpr float STUDENT_JUMP_SPEED    = -12.0f;

// Stałe dla mechaniki i fizyki przeciwników oraz obiektów
constexpr float BRICK_PARTICLE_SPEED  = 4.0f;
constexpr float ECTS_JUMP_SPEED       = -6.0f; // Prędkość wyskakującego punktu ECTS z bloku poczty
constexpr float GRADESHEET_SPEED      = 2.0f;  // Prędkość podstawowego przeciwnika (Karta ocen)
constexpr float DEAN_SPEED            = 2.0f;  // Prędkość Dziekana
constexpr float DEAN_LUGGAGE_SPEED    = 5.0f;  // Prędkość rzucanego przez Dziekana bagażu

// Mechaniki Studenta i Timery
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

// Czas klatki (60 FPS)
constexpr std::chrono::microseconds FRAME_DURATION(16667);

enum class Cell : unsigned char
{
    Empty = 0,
    Floor1,             // Ciemnoczerwony (podłoże - block1.png)
    Brick,              // Jasnoczerwony (cegła - block2.png)
    Platform,           // Żółty (książki - bookblock.png)
    MailBlock,          // Fioletowy (poczta studencka - mailblock0-3.png)
    ActivatedMailBlock, // Stan po uderzeniu skrzynki (mailblock4.png)
    Ects,               // Jasnoniebieski (punkty ECTS do zebrania - ECTS1-7.png)
    Wall
};

struct Object
{
    float horizontal_speed;
    float vertical_speed;
    float x;
    float y;
    unsigned int current_anim_frame = 0;

    explicit Object(const float i_x, const float i_y, const float i_horizontal_speed = 0.0f, const float i_vertical_speed = 0.0f) :
        horizontal_speed(i_horizontal_speed),
        vertical_speed(i_vertical_speed),
        x(i_x),
        y(i_y)
    {}{}
};

using Map = std::vector<std::array<Cell, SCREEN_HEIGHT / CELL_SIZE>>;