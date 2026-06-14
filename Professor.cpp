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

Professor::Professor(float i_x, float i_y) :
	Enemy(i_x, i_y),
	animation_timer(0.0f),
	current_frame(0),
	is_hiding(true),
	state_timer(0.0f)
{
	vx = -0.8f; // Profesor chodzi trochę wolniej niż zwykła ocena
	vy = 0.0f;

	texture.loadFromFile("Resources/profesorhideleft1.png");
	sprite.setTexture(texture);
	sprite.setPosition(x, y);
}

void Professor::update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student)
{
	if (dead) return;

	// 1. Reakcja na nadeptanie przez Studenta
	if (check_student_stomp(i_student))
	{
		dead = true;
		return;
	}
	else if (get_hitbox().intersects(i_student.get_hitbox()))
	{
		i_student.die(false);
	}

	// 2. Cykl zachowania: Profesor co 3 sekundy chowa się do walizki lub z niej wychodzi
	state_timer += 1.0f / 60.0f;
	if (state_timer >= 3.0f)
	{
		is_hiding = !is_hiding;
		state_timer = 0.0f;
		current_frame = 0;
	}

	// Fizyka działa tylko wtedy, gdy profesor idzie (nie chowa się w walizce)
	if (!is_hiding)
	{
		vy += GRAVITY;
		if (vy > MAX_FALL_SPEED) vy = MAX_FALL_SPEED;

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

		// INTERAKCJA MIĘDZY WRONGAMI: Profesor sunący z walizką zgniata Ocene 2.0!
		for (auto& enemy : i_enemies)
		{
			if (enemy.get() != this && !enemy->get_dead())
			{
				if (hitbox.intersects(enemy->get_hitbox()))
				{
					// Trafiona Ocena 2.0 zmienia się w 2dead.png!
					// Wskaźnik bazy nie ma dostępu do sprajta, więc wywołujemy jej "stomp" pośrednio przez zabicie
					enemy->update(i_view_x, i_enemies, i_map_manager, i_student); 
				}
			}
		}
	}

	if (y > SCREEN_HEIGHT) dead = true;

	// 3. ANIMACJA PROFESORA
	animation_timer += 1.0f / 60.0f;
	if (animation_timer >= 0.15f)
	{
		animation_timer = 0.0f;
		if (is_hiding)
		{
			current_frame = (current_frame + 1) % 3; // 3 klatki chowania
		}
		else
		{
			current_frame = (current_frame + 1) % 2; // 2 klatki marszu
		}
	}

	// Ładowanie odpowiednich grafik na podstawie stanu i kierunku
	if (is_hiding)
	{
		if (vx < 0.0f)
			texture.loadFromFile("Resources/profesorhideleft" + std::to_string(current_frame + 1) + ".png");
		else
			texture.loadFromFile("Resources/profesorhideright" + std::to_string(current_frame + 1) + ".png");
	}
	else
	{
		if (vx < 0.0f)
			texture.loadFromFile("Resources/profesorleft" + std::to_string(current_frame + 1) + ".png");
		else
			texture.loadFromFile("Resources/profesorright" + std::to_string(current_frame + 1) + ".png");
	}

	sprite.setTexture(texture);
	sprite.setPosition(x, y);
}

sf::FloatRect Professor::get_hitbox() const
{
	return sf::FloatRect(x, y, CELL_SIZE, CELL_SIZE);
}

void Professor::draw(unsigned int i_view_x, sf::RenderWindow& i_window)
{
	if (dead) return;
	sf::Vector2f original_pos = sprite.getPosition();
	sprite.setPosition(original_pos.x - i_view_x, original_pos.y);
	i_window.draw(sprite);
	sprite.setPosition(original_pos);
}