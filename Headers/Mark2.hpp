#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Enemy.hpp"
#include "Global.hpp"
#include "MapManager.hpp"
#include "Student.hpp"

class Mark2 : public Enemy
{
public:
	Mark2(float i_x, float i_y);

	void update(unsigned int i_view_x, const std::vector<std::shared_ptr<Enemy>>& i_enemies, const MapManager& i_map_manager, Student& i_student) override;
	void draw(unsigned int i_view_x, sf::RenderWindow& i_window) override;

	sf::FloatRect get_hitbox() const override;
	
	// Metoda wywoływana przez Profesora, gdy uderzy w Ocenę 2.0 walizką
	void crush_by_professor();

private:
	sf::Sprite sprite;
	sf::Texture texture;

	float animation_timer;
	unsigned char current_frame;

	bool crushed_by_prof; // Flaga sprawdzająca, który rodzaj śmierci nastąpił
	float death_timer;    // Czas wyświetlania zwłok przed zniknięciem
};