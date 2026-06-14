#include <array>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/EnergyDrink.hpp"

Student::Student() :
	crouching(0),
	dead(0),
	big(0),
	sugar_overdose(0),
	flipped(0),
	on_ground(0),
	energy_drinks_drunk(0),
	enemy_spawn_timer(0),
	x(0),
	y(0),
	horizontal_speed(0),
	vertical_speed(0),
	// Inicjalizacja animacji bezpośrednio z folderu Resources
	big_walk_animation({"Resources/bigrun1.png", "Resources/bigrun2.png"}, 4),
	small_walk_animation({"Resources/run1.png", "Resources/run2.png", "Resources/run3.png"}, 4)
{
	// Domyślna tekstura na start gry
	texture.loadFromFile("Resources/juststudent.png");
	sprite.setTexture(texture);
}

// Pobieranie pozycji i stanu fizycznego
float Student::get_x() const { return x; }
float Student::get_vertical_speed() const { return vertical_speed; }
bool Student::get_dead() const { return dead; }
bool Student::is_sugar_overdose() const { return sugar_overdose; }

void Student::set_vertical_speed(const float i_value)
{
	vertical_speed = i_value;
}

void Student::reset_stats()
{
	crouching = 0;
	dead = 0;
	big = 0;
	sugar_overdose = 0;
	flipped = 0;
	on_ground = 0;
	energy_drinks_drunk = 0;
	horizontal_speed = 0;
	vertical_speed = 0;
	texture.loadFromFile("Resources/juststudent.png");
	sprite.setTexture(texture);
}

void Student::die(const bool i_instant_death)
{
	if (0 == dead)
	{
		if (1 == big && 0 == i_instant_death)
		{
			// Student traci stan Turbo zamiast natychmiast ginąć
			big = 0;
			energy_drinks_drunk = std::max(0, energy_drinks_drunk - 1);
		}
		else
		{
			dead = 1;
			vertical_speed = STUDENT_JUMP_SPEED;
			texture.loadFromFile("Resources/gameover.png");
		}
	}
}

// Liczenie wypitych puszek i aplikowanie logiki przedawkowania cukru
void Student::drink_energy()
{
	energy_drinks_drunk++;
	big = 1; // Przejście w stan Turbo (duży student)

	if (energy_drinks_drunk >= 3)
	{
		sugar_overdose = 1;
		die(1); // Natychmiastowa śmierć z przedawkowania cukru
	}
}

sf::FloatRect Student::get_hit_box() const
{
	// Hitbox dopasowany do wzrostu postaci (stan Turbo zwiększa go dwukrotnie)
	if (1 == big)
	{
		return sf::FloatRect(x, y, CELL_SIZE, 2 * CELL_SIZE);
	}
	else
	{
		return sf::FloatRect(x, y + CELL_SIZE, CELL_SIZE, CELL_SIZE);
	}
}

void Student::update(const unsigned short i_map_width, MapManager& i_map_manager, std::vector<EnergyDrink>& i_energy_drinks, std::vector<std::shared_ptr<Enemy>>& i_enemies)
{
	// Logika grawitacji
	vertical_speed = std::min(GRAVITY + vertical_speed, MAX_VERTICAL_SPEED);

	if (0 == dead)
	{
		// Zwiększenie prędkości poruszania się w zależności od liczby wypitych energetyków
		float current_walk_speed = STUDENT_WALK_SPEED + (energy_drinks_drunk * 0.5f);
		
		crouching = 0;

		// Sterowanie ruchami
		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			flipped = 1;
			horizontal_speed = std::max(horizontal_speed - 0.125f, -current_walk_speed);
		}
		else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			flipped = 0;
			horizontal_speed = std::min(0.125f + horizontal_speed, current_walk_speed);
		}
		else
		{
			// Płynne hamowanie postaci
			if (0 < horizontal_speed)
			{
				horizontal_speed = std::max(0.0f, horizontal_speed - 0.125f);
			}
			else if (0 > horizontal_speed)
			{
				horizontal_speed = std::min(0.0f, 0.125f + horizontal_speed);
			}
		}

		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			if (1 == big)
			{
				crouching = 1;
				horizontal_speed = 0;
			}
		}

		if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || 1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
		{
			if (1 == on_ground && 0 == crouching)
			{
				on_ground = 0;
				vertical_speed = STUDENT_JUMP_SPEED - (energy_drinks_drunk * 0.25f); // Wyższy skok po cukrze
			}
		}

		// Obsługa kolizji pionowych (góra/dół)
		std::vector<unsigned char> collision;
		sf::FloatRect hit_box = get_hit_box();
		hit_box.top += vertical_speed;

		collision = i_map_manager.map_collision({Cell::ActivatedMailBlock, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::Wall}, hit_box);

		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value) { return 0 == i_value; }))
		{
			if (0 > vertical_speed)
			{
				// Uderzenie głową w blok od spodu
				unsigned short check_x = floor((hit_box.left + 0.5f * hit_box.width) / CELL_SIZE);
				unsigned short check_y = floor((hit_box.top) / CELL_SIZE);

				Cell hit_cell = i_map_manager.get_cell(check_x, check_y);

				if (Cell::Brick == hit_cell)
				{
					if (1 == big)
					{
						// Stan Turbo niszczy czerwony wiszący kloc
						i_map_manager.set_cell(check_x, check_y, Cell::Empty);
					}
				}
				else if (Cell::MailBlock == hit_cell)
				{
					// Uderzenie w skrzynkę MailBlock - aktywacja procentowego łupu
					i_map_manager.set_cell(check_x, check_y, Cell::ActivatedMailBlock);
					
					unsigned char loot_chance = rand() % 100;

					if (loot_chance < 40)
					{
						// 40% szans na wypadnięcie puszki energetyka
						i_energy_drinks.push_back(EnergyDrink(CELL_SIZE * check_x, CELL_SIZE * check_y));
					}
					else if (loot_chance < 50)
					{
						// 10% szans na dopisanie punktu ECTS (obsługiwane w main.cpp przez zliczanie zdarzeń)
					}
					else if (loot_chance < 80)
					{
						// 30% szans na natychmiastowy spawn wroga prosto pod sufit!
						// 0 = Ocena 2.0, 1 = Profesor
						unsigned char enemy_type = (rand() % 2 == 0) ? 0 : 1;
						// Spawnowany wróg pojawia się tuż nad skrzynką
						enemy_spawn_queue.push_back({enemy_type, static_cast<float>(CELL_SIZE * check_x), static_cast<float>(CELL_SIZE * (check_y - 1))});
					}
					// Pozostałe 20% to pusta skrzynka (tylko zmiana wyglądu)
				}

				y = CELL_SIZE * (1 + floor((vertical_speed + y) / CELL_SIZE));
			}
			else
			{
				// Bezpieczne wylądowanie na podłożu
				on_ground = 1;
				y = CELL_SIZE * (ceil((vertical_speed + y) / CELL_SIZE) - 1);
			}

			vertical_speed = 0;
		}
		else
		{
			on_ground = 0;
			y += vertical_speed;
		}

		// Obsługa kolizji poziomych (lewo/prawo)
		hit_box = get_hit_box();
		hit_box.left += horizontal_speed;

		collision = i_map_manager.map_collision({Cell::ActivatedMailBlock, Cell::Brick, Cell::Platform, Cell::MailBlock, Cell::Wall}, hit_box);

		if (0 == std::all_of(collision.begin(), collision.end(), [](const unsigned char i_value) { return 0 == i_value; }))
		{
			if (0 < horizontal_speed)
			{
				x = CELL_SIZE * (ceil((horizontal_speed + x) / CELL_SIZE) - 1);
			}
			else
			{
				x = CELL_SIZE * (1 + floor((horizontal_speed + x) / CELL_SIZE));
			}

			horizontal_speed = 0;
		}
		else
		{
			x += horizontal_speed;
		}

		// Blokowanie gracza przed wyjściem poza lewą krawędź ekranu mapy
		x = std::max(0.0f, x);
		x = std::min(static_cast<float>(CELL_SIZE * i_map_width - CELL_SIZE), x);

		// Zarządzanie klatkami animacji ruchu
		if (0 != horizontal_speed)
		{
			if (1 == big)
			{
				big_walk_animation.update();
			}
			else
			{
				small_walk_animation.update();
			}
		}
	}
	else
	{
		// Ruch w powietrzu po śmierci postaci (wypadanie z planszy)
		y += vertical_speed;
	}

	// Śmierć po wpadnięciu do dziury (spadek poniżej mapy)
	if (SCREEN_HEIGHT <= y)
	{
		die(1);
	}
}

void Student::draw(sf::RenderWindow& i_window, const unsigned i_view_x)
{
	// Wybór odpowiedniej tekstury w zależności od stanu Studenta
	if (0 == dead)
	{
		if (0 == on_ground)
		{
			// Tekstura skoku
			if (1 == big)
			{
				texture.loadFromFile(crouching ? "Resources/bigstudentjump3.png" : "Resources/bigstudentjump1.png");
			}
			else
			{
				texture.loadFromFile("Resources/jump1.png");
			}
			sprite.setTexture(texture);
		}
		else if (0 == horizontal_speed)
		{
			// Tekstura stania w miejscu
			if (1 == big)
			{
				texture.loadFromFile(crouching ? "Resources/bigstudentjump3.png" : "Resources/juststudent.png"); // Zamiast dedykowanego big_idle
			}
			else
			{
				texture.loadFromFile("Resources/juststudent.png");
			}
			sprite.setTexture(texture);
		}
	}

	// Rysowanie animacji lub statycznego sprite'a
	if (0 == dead && 0 != horizontal_speed && 1 == on_ground)
	{
		if (1 == big)
		{
			big_walk_animation.set_flipped(flipped);
			big_walk_animation.set_position(round(x) - i_view_x, round(y));
			big_walk_animation.draw(i_window);
		}
		else
		{
			small_walk_animation.set_flipped(flipped);
			small_walk_animation.set_position(round(x) - i_view_x, round(y));
			small_walk_animation.draw(i_window);
		}
	}
	else
	{
		// Odwrócenie sprite'a w zależności od kierunku patrzenia
		if (1 == flipped)
		{
			sprite.setScale(-1, 1);
			sprite.setPosition(round(x) - i_view_x + CELL_SIZE, round(y));
		}
		else
		{
			sprite.setScale(1, 1);
			sprite.setPosition(round(x) - i_view_x, round(y));
		}

		i_window.draw(sprite);
	}
}

// Obsługa kolejki wrogów do spawnowania wywołanych przez MailBlock
bool Student::has_queued_enemy() const { return !enemy_spawn_queue.empty(); }
std::pair<unsigned char, sf::Vector2f> Student::pop_queued_enemy()
{
	auto front = enemy_spawn_queue.front();
	enemy_spawn_queue.erase(enemy_spawn_queue.begin());
	return {front.type, sf::Vector2f(front.x, front.y)};
}