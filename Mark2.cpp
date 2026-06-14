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
	death_timer(0.0f)
{
	vx = -1.0f; // Rozpoczęcie marszu w lewo
	vy = 0.0f;

	texture.loadFromFile("Resources/grade2left1.png");
	sprite.setTexture(texture);
	sprite.setPosition(x, y);
}

void Mark2::crush_by_professor()
{
	if (!dead)
	{
		dead = true;
		crushed_by_prof = true;
		texture.loadFromFile("Resources/2dead.png"); // Śmierć od walizki profesora
		sprite.setTexture(texture);
	}
}

void Mark2::update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student)
{
	// Logika dla martwej oceny (odliczanie czasu do znikania z ekranu)
	if (dead)
	{
		death_timer += 1.0f / 60.0f;
		return;
	}

	// 1. Kolizja ze Studentem (Nadeptanie od góry)
	if (check_student_stomp(i_student))
	{
		dead = true;
		texture.loadFromFile("Resources/2jumpeddead.png"); // Śmierć przez nadeptanie przez Studenta
		sprite.setTexture(texture);
		return;
	}
	else if (get_hitbox().intersects(i_student.get_hitbox()))
	{
		i_student.die(false); // Student ginie przy zderzeniu bocznym
	}

	// 2. Grawitacja i ruch kafelkowy
	vy += GRAVITY;
	if (vy > MAX_FALL_SPEED) vy = MAX_FALL_SPEED;

	x += vx;
	sf::FloatRect hitbox = get_hitbox();
	auto x_collisions = i_map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform }, hitbox);
	if (!x_collisions.empty() && x_collisions[0] == 1)
	{
		x -= vx;
		vx = -vx; // Odbicie od ściany
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

	// 3. Animacja marszu w lewo/prawo na bazie Twoich klatek
	animation_timer += 1.0f / 60.0f;
	if (animation_timer >= 0.2f)
	{
		current_frame = (current_frame + 1) % 2; // Naprzemiennie klatka 1 i 2
		animation_timer = 0.0f;
	}

	if (vx < 0.0f)
	{
		texture.loadFromFile("Resources/grade2left" + std::to_string(current_frame + 1) + ".png");
	}
	else
	{
		texture.loadFromFile("Resources/grade2right" + std::to_string(current_frame + 1) + ".png");
	}

	sprite.setTexture(texture);
	sprite.setPosition(x, y);
}

sf::FloatRect Mark2::get_hitbox() const
{
	// Standardowy rozmiar kafelka dla Oceny 2.0
	return sf::FloatRect(x, y, CELL_SIZE, CELL_SIZE);
}

void Mark2::draw(unsigned int i_view_x, sf::RenderWindow& i_window)
{
	// Wyświetlaj zwłoki przez 2 sekundy, zanim całkowicie znikną z pamięci ekranu
	if (dead && death_timer > 2.0f) return;

	sf::Vector2f original_pos = sprite.getPosition();
	sprite.setPosition(original_pos.x - i_view_x, original_pos.y);
	i_window.draw(sprite);
	sprite.setPosition(original_pos);
}