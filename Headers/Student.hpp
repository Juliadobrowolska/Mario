#pragma once

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include "Animation.hpp"
#include "Global.hpp"
#include "EnergyDrink.hpp" 
#include "MapManager.hpp"

class Student{
	private:
    bool crouching = false;
    bool dead=false;
    bool flipped = false;
    bool on_ground = false;
	
	bool can_jump = true;

    // Zmienna zapobiegająca bugowi z podwójnym zaliczeniem wrogów na raz
	float enemy_bounce_speed;
	float horizontal_speed;
	float vertical_speed;
	float x;
	float y;

	// Im dłużej trzymamy spację/strzałkę, tym wyżej skacze Student
	unsigned char jump_timer;
	// Stan studenta: 0 = zmęczony (mały), 1 = naładowany energetykiem
	unsigned char powerup_state;

	// Timery oparte na stałych z Global.hpp
	unsigned short death_timer;
	unsigned short turbo_student_timer;
	unsigned short immunity_timer; 
   
	// Lista energetyków aktywnych na mapie 
	std::vector<EnergyDrink> energy_drinks;
	std::array<Animation, 2> walk_animations;

	sf::Sprite sprite;
	sf::Texture texture;

	// Animacje poruszania się
	Animation big_walk_animation;
	Animation walk_animation;

public:
	Student();

	float get_vertical_speed() const;
	float get_x() const;
	sf::FloatRect get_hit_box() const;

	// Funkcje wywoływane przy "skreśleniu z listy studentów" (śmierci w grze)
	void die(const bool i_instant_death);
	
	void draw(sf::RenderWindow& i_window);
	void draw_energy_drinks(const unsigned i_view_x, sf::RenderWindow& i_window); // Rysowanie energetyków
	void reset();
	void set_position(const float i_x, const float i_y);
	void set_vertical_speed(const float i_value);
	
	// Główna funkcja aktualizująca fizykę i pozycję Studenta
	void update(const unsigned i_view_x, MapManager& i_map_manager);
};
