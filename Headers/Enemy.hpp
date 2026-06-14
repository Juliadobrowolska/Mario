#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Global.hpp"
#include "MapManager.hpp"
#include "Student.hpp"

class Student;

class Enemy
{
public:
	Enemy(float i_x, float i_y);
	virtual ~Enemy() = default;

	// Wirtualne metody, które nadpiszemy w Mark2 i Professor
	virtual void update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student) = 0;
	virtual void draw(unsigned int i_view_x, sf::RenderWindow& i_window) = 0;
	
	virtual sf::FloatRect get_hitbox() const = 0;
	
	// Wspólne gettery dla wszystkich przeciwników
	bool get_dead() const;
	float get_x() const;
	float get_y() const;

protected:
	float x;
	float y;
	float vx;
	float vy;
	bool dead;

	// Pomocnicza metoda do sprawdzania, czy wróg został nadeptany od góry (jak w Mario)
	bool check_student_stomp(Student& i_student) const;
};