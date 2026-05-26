#include "Headers/Global.hpp"
#include "Headers/Animation.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/EnergyDrink.hpp"
#include "Headers/Student.hpp"

#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// Konstruktor klasy Student - ustawia domyślne statystyki i stan postaci na starcie gry
Student::Student() :
    can_jump(0),          // Na starcie nie można skoczyć (aż dotkniemy ziemi)
    dead(0),              // Student zaczyna grę żywy
    flipped(0),           // Patrzy w prawo (brak odbicia lustrzanego tekstury)
    on_ground(0),         // Zaczyna w powietrzu, zanim opadnie na platformę
    crouching(0),         // Nie kuca
    death_timer(0),       // Reset licznika czasu śmierci
    turbo_student_timer(0), // Reset czasu bycia turbo studentem (speed-boosta)
    immunity_timer(0),      // Reset licznika nietykalności po oberwaniu
    powerup_state(0),     // Stan początkowy: brak ulepszeń (czysty student)
    x(100),               // Startujemy kawałek od lewej krawędzi screena
    y(100),               // Startujemy w powietrzu, żeby sprawdzić spadanie
    enemy_bounce_speed(0),// Brak siły odbicia od wroga
    horizontal_speed(0),  // Postać stoi w miejscu (brak ruchu w poziomie)
    vertical_speed(0),    // Postać nie spada ani nie leci w górę
    sprite(texture)
{
}

void Student::update(const unsigned i_view_x, MapManager& i_map_manager)
{
    // Aktualizacja tymczasowych energetyków 
    for (EnergyDrink& energy_drink : energy_drinks)
    {
        energy_drink.update();
    }

    if (0 == dead)
    {
        bool moving = 0;

        // Ustalamy maksymalną prędkość w zależności od energetyka
        float current_max_speed = STUDENT_WALK_SPEED;
        if (0 < powerup_state)
        {
            current_max_speed = STUDENT_WALK_SPEED * 1.5f; // +50% prędkości po kofeinie
        }

        // Obsługa ruchu w lewo/prawo za pomocą strzałek
        if (0 == crouching)
        {
            if (0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) &&
                1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                moving = 1;
                horizontal_speed = std::max(horizontal_speed - STUDENT_ACCELERATION, -current_max_speed);
            }

            if (0 == sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) &&
                1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                moving = 1;
                horizontal_speed = std::min(STUDENT_ACCELERATION + horizontal_speed, current_max_speed);
            }
        }

        // Hamowanie (powrót do zera), gdy puszcisz klawisze
        if (0 == moving)
        {
            if (0 < horizontal_speed)
            {
                horizontal_speed = std::max<float>(0, horizontal_speed - STUDENT_ACCELERATION);
            }
            else if (0 > horizontal_speed)
            {
                horizontal_speed = std::min<float>(0, STUDENT_ACCELERATION + horizontal_speed);
            }
        }

        // --- RUCH I FIZYKA KOLIZJI W POZIOMIE (OŚ X) ---
        x += horizontal_speed;

        // Wektor przechowujący typy klocków stałych (podłogi, regały, maile)
        std::vector<Cell> solid_cells = { Cell::Floor1, Cell::Floor2, Cell::Floor3, Cell::MailBlock, Cell::ActivatedMailBlock };
        
        // Sprawdzanie kolizji bocznych
        std::vector<unsigned char> x_collisions = i_map_manager.map_collision(solid_cells, get_hit_box());
        
        for (unsigned char collision_mask : x_collisions)
        {
            if (0 != collision_mask)
            {
                // Jeśli nastąpiła kolizja podczas ruchu w prawo, cofnij i zatrzymaj postać
                if (0 < horizontal_speed)
                {
                    x = CELL_SIZE * floor(x / CELL_SIZE);
                }
                // Jeśli nastąpiła kolizja podczas ruchu w lewo, cofnij i zatrzymaj postać
                else if (0 > horizontal_speed)
                {
                    x = CELL_SIZE * ceil(x / CELL_SIZE);
                }
                horizontal_speed = 0;
                break;
            }
        }

        // --- RUCH I FIZYKA KOLIZJI W PIONIE (OŚ Y) ---
        // Obsługa grawitacji i skoku
        if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
        {
            if (0 == vertical_speed && 1 == can_jump && 1 == on_ground)
            {
                vertical_speed = STUDENT_JUMP_SPEED;
                jump_timer = STUDENT_JUMP_TIMER;
                can_jump = 0;
            }
            else if (0 < jump_timer)
            {
                vertical_speed = STUDENT_JUMP_SPEED;
                jump_timer--;
            }
            else
            {
                vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
            }
        }
        else
        {
            vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
            jump_timer = 0;
            can_jump = 1;
        }

        y += vertical_speed;
        on_ground = 0; // Resetujemy stan stania na ziemi przed testem kolizji pionowych

        // Sprawdzanie kolizji pionowych (góra/dół)
        std::vector<sf::Vector2i> collision_tiles;
        std::vector<unsigned char> y_collisions = i_map_manager.map_collision(solid_cells, collision_tiles, get_hit_box());

        for (size_t i = 0; i < y_collisions.size(); i++)
        {
            if (0 != y_collisions[i])
            {
                // Kolizja podczas spadania (lądujemy na ziemi)
                if (0 < vertical_speed)
                {
                    y = CELL_SIZE * floor(y / CELL_SIZE);
                    vertical_speed = 0;
                    jump_timer = 0;
                    on_ground = 1;
                }
                // Kolizja podczas lotu w górę (uderzenie głową w blok)
                else if (0 > vertical_speed)
                {
                    y = CELL_SIZE * ceil(y / CELL_SIZE);
                    vertical_speed = 0;

                    // Interakcja z uderzonym blokiem środowiskowym (np. aktywacja maila)
                    if (!collision_tiles.empty())
                    {
                        sf::Vector2i hit_tile = collision_tiles[0];
                        Cell hit_cell_type = i_map_manager.get_cell_at(hit_tile.x, hit_tile.y);

                        if (Cell::MailBlock == hit_cell_type)
                        {
                            i_map_manager.set_map_cell(hit_tile.x, hit_tile.y, Cell::ActivatedMailBlock);
                            i_map_manager.add_question_block_coin(hit_tile.x * CELL_SIZE, hit_tile.y * CELL_SIZE);
                        }
                    }
                }
                break;
            }
        }

        // --- OBRACANIE I ANIMOWANIE POSTACI ---
        // Obracanie sprite'a w zależności od kierunku poruszania się
        if (0 == horizontal_speed)
        {
            if (1 == moving)
            {
                flipped = 1 - flipped;
            }
        }
        else if (0 < horizontal_speed)
        {
            flipped = 0;
        }
        else if (0 > horizontal_speed)
        {
            flipped = 1;
        }

        // Tymczasowe zbieranie energetyków
        for (EnergyDrink& energy_drink : energy_drinks)
        {
            if (get_hit_box().findIntersection(energy_drink.get_hit_box()).has_value())
            {
                energy_drink.set_dead(true);

                if (0 == powerup_state)
                {
                    powerup_state = 1;
                    turbo_student_timer = STUDENT_TURBO_DURATION; // Aktywacja stanu turbo
                }
            }
        }

        if (0 < immunity_timer) // Odliczanie czasu nietykalności
        {
            immunity_timer--;
        }

        // Odliczanie czasu działania energetyka (turbo studenta)
        if (0 < turbo_student_timer)
        {
            turbo_student_timer--;
            if (0 == turbo_student_timer)
            {
                powerup_state = 0;
            }
        }

        // Spadek w przepaść (śmierć)
        if (y >= SCREEN_HEIGHT)
        {
            die(1);
        }

        // Aktualizacja klatek w obiekcie klasy Animation w zależności od prędkości biegu
        if (0 != horizontal_speed)
        {
            walk_animations[powerup_state].set_animation_speed(STUDENT_WALK_ANIMATION_SPEED * current_max_speed / std::abs(horizontal_speed));
            walk_animations[powerup_state].update();
        }
    }
    else
    {
        // Logika po śmierci
        if (0 == death_timer)
        {
            vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
            y += vertical_speed;
        }
        else if (1 == death_timer)
        {
            vertical_speed = STUDENT_JUMP_SPEED;
        }

        death_timer = std::max(0, death_timer - 1);
    }

    // Czyszczenie zebranych energetyków z pamięci
    energy_drinks.erase(std::remove_if(energy_drinks.begin(), energy_drinks.end(), [](const EnergyDrink& i_energy_drink)
    {
        return i_energy_drink.get_dead();
    }), energy_drinks.end());
}

sf::FloatRect Student::get_hit_box() const
{
    return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{CELL_SIZE, CELL_SIZE});
}

void Student::set_position(const float i_x, const float i_y)
{
    x = i_x;
    y = i_y;
}

void Student::draw(sf::RenderWindow& i_window)
{
    // Efekt migania: jeśli ma nietykalność, ukrywamy sprite co kilka klatek
    if (immunity_timer > 0 && (immunity_timer / 4) % 2 == 0)
    {
        return;
    }

    // Przypisanie pozycji oraz kierunku obrotu (flipped) do właściwej instancji klasy Animation
    walk_animations[powerup_state].set_position(x, y);
    walk_animations[powerup_state].set_flipped(flipped);
    
    // Klasa Animation zajmuje się teraz bezpośrednim renderowaniem aktualnej klatki Studenta
    walk_animations[powerup_state].draw(i_window);
}

void Student::die(const bool i_instant_death)
{
    if (1 == i_instant_death)
    {
        dead = 1;
        death_timer = STUDENT_DEATH_DURATION;
    }
    // Obsługa obrażeń od wrogów
    else if (0 == dead && 0 == immunity_timer) // Można zranić tylko przy braku nietykalności
    {
        if (0 < powerup_state)
        {
            powerup_state = 0;
            turbo_student_timer = 0; 
            immunity_timer = STUDENT_IMMUNITY_DURATION; // Uruchomienie nietykalności po utracie turbo
        }
        else
        {
            dead = 1;
            death_timer = STUDENT_DEATH_DURATION;
        }
    }
}