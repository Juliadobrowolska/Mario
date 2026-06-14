#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/Enemy.hpp"

Enemy::Enemy(float i_x, float i_y) :
	x(i_x),
	y(i_y),
	vx(0.0f),
	vy(0.0f),
	dead(false)
{
}

bool Enemy::get_dead() const
{
	return dead;
}

float Enemy::get_x() const
{
	return x;
}

float Enemy::get_y() const
{
	return y;
}

bool Enemy::check_student_stomp(Student& i_student) const
{
	sf::FloatRect student_hitbox = i_student.get_hitbox();
	sf::FloatRect enemy_hitbox = get_hitbox();

	// Sprawdzamy kolizję poziomą oraz czy Student spada wprost na głowę wroga
	auto intersection = student_hitbox.findIntersection(enemy_hitbox);
	if (intersection)
	{
		// Jeśli spód hitboba Studenta jest blisko czubka głowy wroga, traktujemy to jako nadeptanie
		if (student_hitbox.position.y + student_hitbox.size.y <= enemy_hitbox.position.y + 8.0f)
		{
			return true;
		}
	}
	return false;
}