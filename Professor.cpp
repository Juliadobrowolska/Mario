#include "Headers/Professor.hpp"
#include "Headers/Global.hpp"
#include "Headers/Mark2.hpp"

Professor::Professor(float i_x, float i_y) :
    Enemy(i_x, i_y),
    animation_timer(0.0f),
    current_frame(0),
    is_hiding(true),
    state_timer(0.0f),
    sprite(texture)
{
    vx = hard_mode ? -1.5f : -0.75f;
    vy = 0.0f;
    if(texture.loadFromFile("Resources/profesorhideleft1.png")) {
        sprite.setTexture(texture);
    }
}

void Professor::update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student)
{
    if (dead) return;

    if (check_student_stomp(i_student))
    {
        dead = true;
        return;
    }
    else if (get_hitbox().findIntersection(i_student.get_hitbox()))
    {
        i_student.die(false);
    }

    state_timer += 1.0f / 60.0f;
    if (state_timer >= 3.0f)
    {
        is_hiding = !is_hiding;
        state_timer = 0.0f;
    }

    if (!is_hiding)
    {
        vy += GRAVITY;
        if (vy > MAX_VERTICAL_SPEED) vy = MAX_VERTICAL_SPEED;

        x += vx;
        sf::FloatRect hitbox = get_hitbox();
        auto x_collisions = i_map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform }, hitbox);
        if (!x_collisions.empty() && x_collisions[0] == 1)
        {
            x -= vx;
            vx = -vx;
        }

        y += vy;
        hitbox = get_hitbox();
        auto y_collisions = i_map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform }, hitbox);
        if (!y_collisions.empty() && y_collisions[0] == 1)
        {
            y -= vy;
            vy = 0.0f;
        }

        for (auto& enemy : i_enemies)
        {
            if (enemy.get() != this && !enemy->get_dead())
            {
                if (get_hitbox().findIntersection(enemy->get_hitbox()))
                {
                    auto mark = std::dynamic_pointer_cast<Mark2>(enemy);
                    if (mark) mark->crush_by_professor();
                }
            }
        }
    }

    animation_timer += 1.0f / 60.0f;
    const unsigned char frame_count = is_hiding ? 3 : 2;
    if (animation_timer >= 0.15f)
    {
        animation_timer = 0.0f;
        current_frame = (current_frame + 1) % frame_count;
    }

    static std::string last_path = "";
    static std::string last_failed_path = "";
    std::string path = "Resources/profesor" + std::string(is_hiding ? "hide" : "") +
                       (vx < 0 ? "left" : "right") +
                       std::to_string((current_frame % frame_count) + 1) + ".png";

    if (path != last_path && path != last_failed_path)
    {
        if (texture.loadFromFile(path))
        {
            sprite.setTexture(texture);
            last_path = path;
        }
        else
        {
            last_failed_path = path;
        }
    }
    sprite.setPosition(sf::Vector2f(x, y));
}

sf::FloatRect Professor::get_hitbox() const {
    return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)});
}

void Professor::draw(unsigned int i_view_x, sf::RenderWindow& i_window)
{
    if (dead) return;
    sprite.setPosition(sf::Vector2f(x - (float)i_view_x, y));
    i_window.draw(sprite);
}