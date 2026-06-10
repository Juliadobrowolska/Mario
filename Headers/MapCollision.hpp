#pragma once

#include <vector>
#include "Global.hpp"
#include "MapManager.hpp"

unsigned char map_collision(float i_x, float i_y, const std::vector<Cell>& i_check_cells, const MapManager& i_map_manager);