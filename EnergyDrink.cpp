#include <vector>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/EnergyDrink.hpp"

EnergyDrink::EnergyDrink(float i_x, float i_y) :
	x(i_x),
	y(i_y),
	vx(1.5f), // Prędkość poruszania się puszki po ziemi
	vy(0.0f),
	collected(false),
	spawning(true),
	spawn_timer(0.0f),
	sprite(texture)
{
	// Dopasowanie do Twojej nazwy pliku: "Energydrink.png" (małe 'd')
	if (!texture.loadFromFile("Resources/Energydrink.png")) {}
	sprite.setTexture(texture);
	sprite.setPosition(sf::Vector2f{x, y});
}

void EnergyDrink::update(unsigned int i_view_x, const MapManager& i_map_manager, Student& i_student)
{
	if (collected) return;

	// 1. Logika wysuwania się z bloku (animacja spawnu)
	if (spawning)
	{
		y -= 0.5f; // Puszka płynnie jedzie w górę
		spawn_timer += 1.0f / 60.0f;
		
		if (spawn_timer >= 1.0f) // Po sekundzie kończy spawn i zaczyna żyć własnym życiem
		{
			spawning = false;
		}
		
		sprite.setPosition(sf::Vector2f{x, y});
		return;
	}

	// 2. Fizyka grawitacji i poruszania się
	vy += GRAVITY;
	if (vy > MAX_VERTICAL_SPEED) vy = MAX_VERTICAL_SPEED;

	// Ruch w poziomie i kolizje ze ścianami
	x += vx;
	sf::FloatRect hitbox = get_hitbox();
	auto x_collisions = i_map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform }, hitbox);
	if (!x_collisions.empty() && x_collisions[0] == 1)
	{
		x -= vx;
		vx = -vx; // Odbicie od ściany w drugą stronę
	}

	// Ruch w pionie i kolizje z podłogą
	y += vy;
	hitbox = get_hitbox();
	auto y_collisions = i_map_manager.map_collision({ Cell::Wall, Cell::Brick, Cell::Platform }, hitbox);
	if (!y_collisions.empty() && y_collisions[0] == 1)
	{
		y -= vy;
		vy = 0.0f;
	}

	// Przepaść (puszka spada z mapy)
	if (y > SCREEN_HEIGHT)
	{
		collected = true;
	}

	// 3. Kolizja ze Studentem (Zbieranie)
	auto overlap = hitbox.findIntersection(i_student.get_hitbox());
	if (overlap)
	{
		collected = true;
		i_student.collect_energy_drink();
	}

	sprite.setPosition(sf::Vector2f{x, y});
}

sf::FloatRect EnergyDrink::get_hitbox() const
{
	// Puszka ma standardowy rozmiar jednego kafelka mapy
	return sf::FloatRect(sf::Vector2f{x, y}, sf::Vector2f{static_cast<float>(CELL_SIZE), static_cast<float>(CELL_SIZE)});
}

bool EnergyDrink::get_collected() const
{
	return collected;
}

void EnergyDrink::draw(sf::RenderWindow& i_window, unsigned int i_view_x)
{
	if (collected) return;

	sf::Vector2f original_pos = sprite.getPosition();
	sprite.setPosition(sf::Vector2f{original_pos.x - static_cast<float>(i_view_x), original_pos.y});
	i_window.draw(sprite);
	sprite.setPosition(original_pos);
}