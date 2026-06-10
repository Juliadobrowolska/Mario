#pragma once
#include <SFML/Graphics.hpp>

class MapManager;
void draw_map(unsigned i_view_x, sf::RenderWindow& i_window, const MapManager& i_map_manager);
