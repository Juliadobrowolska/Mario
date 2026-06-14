#include <array>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"


void Student::die(const bool i_instant_death)
{
	if (true == i_instant_death)
	{
		dead = true;
		texture.loadFromFile("Resources/Images/student_death.png");
	}
	// *Jeśli student ma power-up (jest odporny), to zamiast umierać, traci go i dostaje chwilę nietykalności*
	else if (0 == growth_timer && 0 == invincible_timer)
	{
		if (false == is_older_student)
		{
			dead = true;
			texture.loadFromFile("Resources/Images/student_death.png");
		}
		else
		{
			*is_older_student = false;* // *Utrata power-upa*
			invincible_timer = STUDENT_IMMUNITY_DURATION;
			
			// *Wzrost się nie zmienia, więc nie musimy manipulować pozycją Y postaci!*
			if (true == crouching) crouching = false;
		}
	}
}

void Student::update(const unsigned i_view_x, MapManager& i_map_manager)
{
	if (0 != enemy_bounce_speed)
	{
		vertical_speed = enemy_bounce_speed;
		enemy_bounce_speed = 0;
	}

	for (Index& index : indexes)
	{
		index.update(i_view_x, i_map_manager);
	}

	if (false == dead)
	{
		bool moving = false;
		std::vector<unsigned char> collision;
		std::vector<sf::Vector2i> cells;
		sf::FloatRect hit_box = get_hit_box();

		on_ground = false;

		// *USTALANIE PRĘDKOŚCI: Jeśli student posiada indeks, jego maksymalna prędkość rośnie dwukrotnie*
		*float current_max_speed = is_older_student ? (STUDENT_WALK_SPEED * 2.f) : STUDENT_WALK_SPEED;*
		*float current_acceleration = is_older_student ? (STUDENT_ACCELERATION * 2.f) : STUDENT_ACCELERATION;*

		if (false == crouching)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				moving = true;
				*horizontal_speed = std::max(horizontal_speed - current_acceleration, -current_max_speed);*
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				moving = true;
				*horizontal_speed = std::min(current_acceleration + horizontal_speed, current_max_speed);*
			}
		}

		if (false == moving)
		{
			if (0 < horizontal_speed)       *horizontal_speed = std::max<float>(0, horizontal_speed - current_acceleration);*
			else if (0 > horizontal_speed)  *horizontal_speed = std::min<float>(0, current_acceleration + horizontal_speed);*
		}

		// Kolizje poziome
		hit_box.left += horizontal_speed;
		collision = i_map_manager.map_collision({Cell::ActivatedMailBlock, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::Wall}, hit_box);

		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value) { return 0 == i_value; }))
		{
			moving = false;
			if (0 < horizontal_speed)       x = CELL_SIZE * (ceil((horizontal_speed + x) / CELL_SIZE) - 1);
			else if (0 > horizontal_speed)  x = CELL_SIZE * (1 + floor((horizontal_speed + x) / CELL_SIZE));
			horizontal_speed = 0;
		}
		else
		{
			x += horizontal_speed;
		}

		// Skoki Studenta
		hit_box = get_hit_box();
		hit_box.top++;
		collision = i_map_manager.map_collision({Cell::ActivatedMailBlock, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::Wall}, hit_box);
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (0 == vertical_speed && 0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value) { return 0 == i_value; }))
			{
				vertical_speed = STUDENT_JUMP_SPEED;
				jump_timer = STUDENT_JUMP_TIMER;
			}
			else if (0 < jump_timer)
			{
				vertical_speed = STUDENT_JUMP_SPEED;
				jump_timer--;
			}
		}
		else
		{
			vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
			jump_timer = 0;
		}

		// Kolizje pionowe
		hit_box = get_hit_box();
		hit_box.top += vertical_speed;
		collision = i_map_manager.map_collision({Cell::ActivatedMailBlock, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::Wall}, hit_box);
		
		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value) { return 0 == i_value; }))
		{
			if (0 > vertical_speed)
			{
				// *Niszczenie klocków: teraz niszczymy je, jeśli mamy power-up (is_older_student), wzrost nie ma znaczenia*
				*if (false == crouching && true == is_older_student)*
				{
					i_map_manager.map_collision({Cell::Brick}, cells, hit_box);
					for (const sf::Vector2i& cell : cells)
					{
						i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
						i_map_manager.add_brick_particles(CELL_SIZE * cell.x, CELL_SIZE * cell.y);
					}
				}

				i_map_manager.map_collision({Cell::MailBlock}, cells, hit_box);
				for (const sf::Vector2i& cell : cells)
				{
					i_map_manager.set_map_cell(cell.x, cell.y, Cell::ActivatedMailBlock);
					
					if (rand() % 2 == 0)
					{
						indexes.push_back(Index(CELL_SIZE * cell.x, CELL_SIZE * cell.y));
					}
					else
					{
						i_map_manager.add_mail_block_ects(CELL_SIZE * cell.x, CELL_SIZE * cell.y);
					}
				}
				y = CELL_SIZE * (1 + floor((vertical_speed + y) / CELL_SIZE));
			}
			else if (0 < vertical_speed)
			{
				y = CELL_SIZE * (ceil((vertical_speed + y) / CELL_SIZE) - 1);
			}
			jump_timer = 0;
			vertical_speed = 0;
		}
		else
		{
			y += vertical_speed;
		}

		if (0 < horizontal_speed)       flipped = false;
		else if (0 > horizontal_speed)  flipped = true;

		// Sprawdzenie podłoża
		hit_box = get_hit_box();
		hit_box.top++;
		collision = i_map_manager.map_collision({Cell::ActivatedMailBlock, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::Wall}, hit_box);
		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value) { return 0 == i_value; }))
		{
			on_ground = true;
		}

		// Kolizja z Indeksem (Aktywacja turbo-prędkości)
		for (Index& index : indexes)
		{
			if (get_hit_box().intersects(index.get_hit_box()))
			{
				index.set_dead(true);
				if (false == is_older_student)
				{
					is_older_student = true;
					*growth_timer = STUDENT_GROWTH_DURATION;* // *Uruchamia efekt "błyskania" przy aktywacji*
				}
			}
		}

		// Zbieranie ECTS
		hit_box = get_hit_box();
		i_map_manager.map_collision({Cell::Ects}, cells, hit_box);
		for (const sf::Vector2i& cell : cells)
		{
			i_map_manager.set_map_cell(cell.x, cell.y, Cell::Empty);
		}

		if (0 < invincible_timer) invincible_timer--;
		if (0 < growth_timer) growth_timer--;

		if (y >= SCREEN_HEIGHT - get_hit_box().height) die(true);

		// *Animacja dopasowuje swoją szybkość do faktycznej prędkości biegu Studenta!*
		walk_animation.set_animation_speed(STUDENT_WALK_ANIMATION_SPEED * STUDENT_WALK_SPEED / abs(horizontal_speed));
		walk_animation.update();
	}
	else
	{
		if (0 == death_timer)
		{
			vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);
			y += vertical_speed;
		}
		else if (1 == death_timer)
		{
			vertical_speed = STUDENT_JUMP_SPEED;
		}
		death_timer = std::max(0, death_timer - 1);
	}

	indexes.erase(remove_if(indexes.begin(), indexes.end(), [](const Index& i_index)
	{
		return true == i_index.get_dead();
	}), indexes.end());
}

sf::FloatRect Student::get_hit_box() const
{
	// *Hitbox zawsze ma stały rozmiar 1 kafelka, bo Student nie rośnie wzwyż!*
	*return sf::FloatRect(x, y, CELL_SIZE, CELL_SIZE);*
}