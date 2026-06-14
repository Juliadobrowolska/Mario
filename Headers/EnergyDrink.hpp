#pragma once
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "MapManager.hpp"
#include "Student.hpp"

class Student;

class EnergyDrink
{
public:
	EnergyDrink(float i_x, float i_y);

	void update(unsigned int i_view_x, const MapManager& i_map_manager, Student& i_student);
	void draw(sf::RenderWindow& i_window, unsigned int i_view_x);

	sf::FloatRect get_hitbox() const;
	bool get_collected() const;

private:
	float x;
	float y;
	float vx;
	float vy;

	bool collected;
	bool spawning;      // true gdy puszka płynnie wysuwa się w górę z bloku
	float spawn_timer;

	sf::Sprite sprite;
	sf::Texture texture;
};