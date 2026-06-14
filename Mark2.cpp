#include <vector>
#include <cmath>
#include <algorithm>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Mark2.hpp"

Mark2::Mark2(float i_x, float i_y) :
	Enemy(i_x, i_y),
	animation_timer(0.0f),
	current_frame(0),
	crushed_by_prof(false),
	death_timer(0.0f),
	sprite(texture)
{
	// Prędkość zależna od trybu trudności
	vx = hard_mode ? -1.0f : -0.5f; 
	vy = 0.0f;

	texture.loadFromFile("Resources/grade2left1.png");
	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f{x, y});
}

void Mark2::crush_by_professor()
{
	if (!dead)
	{
		dead = true;
		crushed_by_prof = true;
		texture.loadFromFile("Resources/2dead.png"); 
		sprite.setTexture(texture);
	}
}

void Mark2::update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student)
{
	if (dead)
	{
		death_timer += 1.0f / 60.0f;
		return;
	}

	if (check_student_stomp(i_student))
	{
		dead = true;
		texture.loadFromFile("Resources/2jumpeddead.png"); 
		sprite.setTexture(texture);
		return;
	}
	else if (get_hitbox().findIntersection(i_student.get_hitbox()))
	{
		i_student.die(false); 
	}

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

	if (y > SCREEN_HEIGHT)
	{
		dead = true;
		return;
	}

	animation_timer += 1.0f / 60.0f;
	if (animation_timer >= 0.2f)
	{
		current_frame = (current_frame + 1) % 2; 
		animation_timer = 0.0f;
	}

	if (vx < 0.0f)
		texture.loadFromFile("Resources/grade2left" + std::to_string(current_frame + 1) + ".png");
	else
		texture.loadFromFile("Resources/grade2right" + std::to_string(current_frame + 1) + ".png");

	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f{x, y});
}

sf::FloatRect Mark2::get_hitbox() const { return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)}); }

void Mark2::draw(unsigned int i_view_x, sf::RenderWindow& i_window)
{
	if (dead && death_timer > 2.0f) return;
	sf::Vector2f pos = sprite.getPosition();
	sprite.setPosition(sf::Vector2f{pos.x - static_cast<float>(i_view_x), pos.y});
	i_window.draw(sprite);
	sprite.setPosition(pos);
}