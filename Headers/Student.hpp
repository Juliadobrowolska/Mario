#pragma once
#include <vector>
#include <queue>
#include <utility>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Global.hpp"
#include "MapManager.hpp"

class Enemy;
class EnergyDrink;

class Student
{
public:
	Student();

	void reset_stats();
	void update(unsigned short map_width, MapManager& map_manager, std::vector<EnergyDrink>& energy_drinks, std::vector<std::shared_ptr<Enemy>>& enemies);
	void draw(sf::RenderWindow& window, unsigned int view_x);
	
	void collect_energy_drink();
	void die(bool time_out);

	// Getters
	float get_x() const;
	float get_y() const;
	bool get_dead() const;
	bool is_sugar_overdose() const;
	unsigned int get_energy_drinks() const;
	sf::FloatRect get_hitbox() const;

	// Spawning queue mechanics
	bool has_queued_enemy();
	std::pair<unsigned char, sf::Vector2f> pop_queued_enemy();

private:
	float x;
	float y;
	float vx;
	float vy;

	bool dead;
	bool sugar_overdose;
	bool is_big;

	unsigned int energy_drinks_collected;
	unsigned char jump_timer;

	// Animation controllers
	float animation_timer;
	unsigned char current_frame;

	sf::Sprite sprite;
	sf::Texture texture_small;
	sf::Texture texture_big;

	// Queue for enemies that might be dynamically spawned (e.g., from hitting blocks)
	std::queue<std::pair<unsigned char, sf::Vector2f>> enemy_queue;
};