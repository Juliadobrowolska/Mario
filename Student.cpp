#include <algorithm>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/MapCollision.hpp"
#include "Headers/Student.hpp"

constexpr float STUDENT_FRICTION = 0.20f;

Student::Student() :
    is_dead(false),
    crouching(false),
    flipped(false),
    on_ground(false),
    can_jump(true),
    enemy_bounce_speed(0.0f),
    horizontal_speed(0.0f),
    vertical_speed(0.0f),
    x(0.0f),
    y(0.0f),
    jump_timer(0),
    powerup_state(0),
    death_timer(0),
    turbo_student_timer(0),
    immunity_timer(0),
    walk_frame_counter(0),
    game_over_text(game_over_font, "", 30),
    sprite(tex_idle)
{
}

bool Student::load_textures()
{
    std::vector<std::string> right_runs = {"Resources/run1.png", "Resources/run2.png", "Resources/run3.png"};
    tex_normal_walk.clear();
    for (const auto& path : right_runs)
    {
        sf::Texture tex;
        if (!tex.loadFromFile(path)) return false;
        tex_normal_walk.push_back(tex);
    }

    std::vector<std::string> left_runs = {"Resources/left1.png", "Resources/left2.png"};
    tex_big_walk.clear();
    for (const auto& path : left_runs)
    {
        sf::Texture tex;
        if (!tex.loadFromFile(path)) return false;
        tex_big_walk.push_back(tex);
    }

    if (!tex_jump_up.loadFromFile("Resources/jump1.png")) return false;
    if (!tex_jump_down.loadFromFile("Resources/jump2.png")) return false;
    if (!tex_death.loadFromFile("Resources/gameover.png")) return false;
    if (!tex_idle.loadFromFile("Resources/juststudent.png")) return false;

    if (!game_over_font.openFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        return false;
    }

    game_over_text.setFont(game_over_font);
    game_over_text.setString("GAME OVER");
    game_over_text.setCharacterSize(80);
    game_over_text.setFillColor(sf::Color::Red);
    game_over_text.setStyle(sf::Text::Bold);

    sf::FloatRect textRect = game_over_text.getLocalBounds();
    game_over_text.setOrigin(sf::Vector2f{textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});

    sprite.setTexture(tex_idle);
    sprite.setOrigin(sf::Vector2f{CELL_SIZE / 2.0f, CELL_SIZE / 2.0f});

    return true;
}

void Student::update(const unsigned i_view_x, MapManager& i_map_manager)
{
    if (is_dead)
    {
        return;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        horizontal_speed = std::max(horizontal_speed - STUDENT_ACCELERATION, -STUDENT_WALK_SPEED);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        horizontal_speed = std::min(horizontal_speed + STUDENT_ACCELERATION, STUDENT_WALK_SPEED);
    }
    else
    {
        if (horizontal_speed > 0)
        {
            horizontal_speed = std::max(0.0f, horizontal_speed - STUDENT_FRICTION);
        }
        else if (horizontal_speed < 0)
        {
            horizontal_speed = std::min(0.0f, horizontal_speed + STUDENT_FRICTION);
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
    {
        if (on_ground)
        {
            vertical_speed = STUDENT_JUMP_SPEED;
            on_ground = false;
        }
    }

    vertical_speed = std::min(vertical_speed + GRAVITY, MAX_VERTICAL_SPEED);

    std::vector<Cell> solid_cells = {Cell::Floor1, Cell::Floor2, Cell::Brick, Cell::MailBlock, Cell::ActivatedMailBlock, Cell::Platform};

    y += vertical_speed;
    unsigned char y_collision = map_collision(x, y, solid_cells, i_map_manager);

    if (y_collision > 0)
    {
        if (vertical_speed > 0)
        {
            if ((y_collision & 4) || (y_collision & 8))
            {
                y = std::floor(y / CELL_SIZE) * CELL_SIZE;
                vertical_speed = 0.0f;
                on_ground = true;
            }
        }
        else if (vertical_speed < 0)
        {
            if ((y_collision & 1) || (y_collision & 2))
            {
                y = std::ceil(y / CELL_SIZE) * CELL_SIZE;
                vertical_speed = 0.0f;
            }
        }
    }
    else
    {
        on_ground = false;
    }

    x += horizontal_speed;

    if (x < i_view_x)
    {
        x = static_cast<float>(i_view_x);
        horizontal_speed = 0.0f;
    }

    unsigned char x_collision = map_collision(x, y, solid_cells, i_map_manager);

    if (x_collision > 0)
    {
        if (horizontal_speed > 0)
        {
            if ((x_collision & 2) || (x_collision & 8))
            {
                x = std::floor(x / CELL_SIZE) * CELL_SIZE;
                horizontal_speed = 0.0f;
            }
        }
        else if (horizontal_speed < 0)
        {
            if ((x_collision & 1) || (x_collision & 4))
            {
                x = std::ceil(x / CELL_SIZE) * CELL_SIZE;
                horizontal_speed = 0.0f;
            }
        }
    }

    if (y > SCREEN_HEIGHT)
    {
        is_dead = true;
        return;
    }

    if (!on_ground)
    {
        if (vertical_speed < -1.0f)
        {
            sprite.setTexture(tex_jump_up);
        }
        else
        {
            sprite.setTexture(tex_jump_down);
        }
    }
    else if (0.0f == horizontal_speed)
    {
        sprite.setTexture(tex_idle);
    }
    else
    {
        int animation_delay = 100;
        if (std::abs(horizontal_speed) > 0)
        {
            animation_delay = static_cast<int>(100.0f * (STUDENT_WALK_SPEED / std::abs(horizontal_speed)));
        }

        if (animation_clock.getElapsedTime().asMilliseconds() > animation_delay)
        {
            if (horizontal_speed < 0)
            {
                walk_frame_counter = (walk_frame_counter + 1) % tex_big_walk.size();
                sprite.setTexture(tex_big_walk[walk_frame_counter]);
            }
            else
            {
                walk_frame_counter = (walk_frame_counter + 1) % tex_normal_walk.size();
                sprite.setTexture(tex_normal_walk[walk_frame_counter]);
            }
            animation_clock.restart();
        }
    }

    if (immunity_timer > 0)
    {
        --immunity_timer;
    }
}

void Student::draw(sf::RenderWindow& i_window, unsigned int i_camera_x)
{
    if (is_dead)
    {
        sprite.setTexture(tex_death);
        sprite.setPosition(sf::Vector2f{((x - i_camera_x) + CELL_SIZE / 2.0f) * SCREEN_RESIZE, (y + CELL_SIZE / 2.0f) * SCREEN_RESIZE});
        i_window.draw(sprite);

        sf::Vector2f window_center = i_window.getView().getCenter();
        game_over_text.setPosition(window_center);
        i_window.draw(game_over_text);
        return;
    }

    if (immunity_timer > 0 && (immunity_timer / 4) % 2 == 0)
    {
        return;
    }

    sprite.setScale(sf::Vector2f{SCREEN_RESIZE, SCREEN_RESIZE});
    sprite.setPosition(sf::Vector2f{((x - i_camera_x) + CELL_SIZE / 2.0f) * SCREEN_RESIZE, (y + CELL_SIZE / 2.0f) * SCREEN_RESIZE});
    i_window.draw(sprite);
}

void Student::set_position(const float i_x, const float i_y)
{
    x = i_x;
    y = i_y;
    vertical_speed = 0.0f;
    horizontal_speed = 0.0f;
}
 
float Student::get_vertical_speed() const
{
    return vertical_speed;
}

float Student::get_x() const
{
    return x;
}

sf::FloatRect Student::get_hit_box() const
{
    return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)});
}

void Student::set_vertical_speed(const float i_value)
{
    vertical_speed = i_value;
}

void Student::die(const bool i_instant_death)
{
    (void)i_instant_death;
    is_dead = true;
}

void Student::reset()
{
    is_dead = false;
    x = 0.0f;
    y = 0.0f;
    horizontal_speed = 0.0f;
    vertical_speed = 0.0f;
    on_ground = false;
    immunity_timer = 0;
}

void Student::draw_energy_drinks(const unsigned i_view_x, sf::RenderWindow& i_window)
{
    (void)i_view_x;
    (void)i_window;
}