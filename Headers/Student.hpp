#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "Animation.hpp"

// Forward declaration klas, by uniknąć zapętleń nagłówków
class MapManager;
class EnergyDrink;
class Enemy;

class Student
{
public:
	// Struktura pomocnicza do spawnowania wrogów wyciągniętych z MailBlocka
	struct QueuedEnemy
	{
		unsigned char type;
		float x;
		float y;
	};

	Student();

	// Podstawowe metody fizyki i aktualizacji stanu
	void update(const unsigned short i_map_width, MapManager& i_map_manager, std::vector<EnergyDrink>& i_energy_drinks, std::vector<std::shared_ptr<Enemy>>& i_enemies);
	void draw(sf::RenderWindow& i_window, const unsigned i_view_x);
	
	void die(const bool i_instant_death);
	void reset_stats();
	void drink_energy();

	// Gettery i Settery
	float get_x() const;
	float get_vertical_speed() const;
	void set_vertical_speed(const float i_value);
	bool get_dead() const;
	bool is_sugar_overdose() const;
	sf::FloatRect get_hit_box() const;

	// Obsługa kolejki wrogów wyskakujących z MailBlocków
	bool has_queued_enemy() const;
	std::pair<unsigned char, sf::Vector2f> pop_queued_enemy();

private:
	// Flagi stanów bohatera
	bool crouching;
	bool dead;
	bool big;
	bool sugar_overdose;
	bool flipped;
	bool on_ground;

	// Statystyki rozgrywki
	unsigned char energy_drinks_drunk;
	unsigned short enemy_spawn_timer;

	// Pozycja i prędkość
	float x;
	float y;
	float horizontal_speed;
	float vertical_speed;

	// Obiekty SFML do rysowania i animacji
	sf::Sprite sprite;
	sf::Texture texture;
	Animation big_walk_animation;
	Animation small_walk_animation;

	// Kontener na wrogów oczekujących na natychmiastowy spawn na mapie
	std::vector<QueuedEnemy> enemy_spawn_queue;
};