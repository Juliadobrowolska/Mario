#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Enemy.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Student.hpp"

class Professor : public Enemy
{
public:
	Professor(float i_x, float i_y);

	void update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student) override;
	void draw(unsigned int i_view_x, sf::RenderWindow& i_window) override;

	sf::FloatRect get_hitbox() const override;

private:
	sf::Sprite sprite;
	sf::Texture texture;

	float animation_timer;
	unsigned char current_frame;
	
	bool is_hiding; // Status, czy profesor chowa się/siedzi w walizce
	float state_timer;
};