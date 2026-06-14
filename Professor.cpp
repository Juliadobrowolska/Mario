#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Professor.hpp"
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
	texture.loadFromFile("Resources/profesorhideleft1.png");
	sprite.setTexture(texture);
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

		// Interakcja z Ocenami (zgniatanie)
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
	if (animation_timer >= 0.15f)
	{
		animation_timer = 0.0f;
		current_frame = (current_frame + 1) % (is_hiding ? 3 : 2);
	}

	if (is_hiding)
		texture.loadFromFile("Resources/profesorhide" + std::string(vx < 0 ? "left" : "right") + std::to_string(current_frame + 1) + ".png");
	else
		texture.loadFromFile("Resources/profesor" + std::string(vx < 0 ? "left" : "right") + std::to_string(current_frame + 1) + ".png");

	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f{x, y});
}

sf::FloatRect Professor::get_hitbox() const { return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)}); }

void Professor::draw(unsigned int i_view_x, sf::RenderWindow& i_window)
{
	if (dead) return;
	sf::Vector2f pos = sprite.getPosition();
	sprite.setPosition(sf::Vector2f{pos.x - static_cast<float>(i_view_x), pos.y});
	i_window.draw(sprite);
	sprite.setPosition(pos);
}