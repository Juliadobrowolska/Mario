#pragma once

// Rozmiar pojedynczego kafelka (bloku) w pikselach
constexpr unsigned char CELL_SIZE = 16;

// Stałe dotyczące ekranu gry
constexpr unsigned short SCREEN_HEIGHT = 240;
constexpr unsigned short SCREEN_WIDTH = 320;

// Margines aktualizacji obiektów poza widocznym ekranem
constexpr unsigned short ENTITY_UPDATE_AREA = 64;

// Fizyka gry
constexpr float GRAVITY = 0.25f;
constexpr float MAX_VERTICAL_SPEED = 4.0f;

// Stałe fizyczne dla Studenta
constexpr float STUDENT_BOUNCE_SPEED = 4.0f;
constexpr float STUDENT_JUMP_SPEED = -4.0f;
constexpr float STUDENT_WALK_SPEED = 1.5f;

// Stałe dla Oceny 2.0 (dawna Goomba)
constexpr float GOOMBA_SPEED = 0.5f;
constexpr unsigned char GOOMBA_WALK_ANIMATION_SPEED = 8;
constexpr unsigned short GOOMBA_DEATH_DURATION = 32;

// Stałe dla Profesora z walizką (dawna Koopa)
constexpr float KOOPA_SPEED = 0.75f;
constexpr float KOOPA_SHELL_SPEED = 4.0f;
constexpr unsigned char KOOPA_GET_OUT_ANIMATION_SPEED = 4;
constexpr unsigned char KOOPA_WALK_ANIMATION_SPEED = 8;
constexpr unsigned short KOOPA_GET_OUT_DURATION = 512;

// --- TWOJE BLOKI NA MAPIE ---
enum class Cell
{
	ActivatedMailBlock, // Szary mailblock0 (po uderzeniu głową)
	Brick,              // Czerwony wiszący block1 (do niszczenia w stanie Turbo)
	Empty,              // Czyste powietrze (niebieski piksel)
	Platform,           // Żółty bookblock (zwykła stała platforma)
	MailBlock,          // Fioletowy mailblock3 (skrzynka z losowym łupem)
	Wall                // Czarny block1 (stabilna podłoga) lub bordowy block2 (dolna podłoga)
};

// Globalna flaga poziomu trudności (Normalny / Trudny)
extern bool hard_mode;