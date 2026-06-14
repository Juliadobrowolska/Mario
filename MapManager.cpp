#include <array>
#include <cmath>
#include <algorithm>
#include <vector>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"

MapManager::MapManager() :
    map_width(0)
{
}

void MapManager::clear_enemies()
{
    enemy_positions.clear();
}

const std::vector<MapManager::EnemyPosition>& MapManager::get_enemy_positions() const
{
    return enemy_positions;
}

unsigned short MapManager::get_map_width() const
{
    return map_width;
}

Cell MapManager::get_cell(const unsigned short i_x, const unsigned short i_y) const
{
    if (i_x < map_vector.size() && i_y < map_vector[i_x].size())
    {
        return map_vector[i_x][i_y];
    }
    return Cell::Empty;
}

void MapManager::set_cell(const unsigned short i_x, const unsigned short i_y, const Cell i_cell)
{
    if (i_x < map_vector.size() && i_y < map_vector[i_x].size())
    {
        map_vector[i_x][i_y] = i_cell;
    }
}

// Ładowanie poziomu na podstawie numeru (np. i_level = 1 odpali level_1.png)
void MapManager::load_level(const unsigned char i_level)
{
    sf::Image map_image;
    
    // Ścieżka skrócona bezpośrednio do folderu Resources/
    if (!map_image.loadFromFile("Resources/level_" + std::to_string(i_level) + ".png"))
    {
        return; 
    }

    map_width = map_image.getSize().x;
    
    // Czyszczenie poprzednich danych mapy i wrogów
    map_vector.clear();
    enemy_positions.clear();
    map_vector.resize(map_width, std::vector<Cell>(SCREEN_HEIGHT / CELL_SIZE, Cell::Empty));

    // Iteracja po każdym pikselu obrazu poziomu
    for (unsigned short x = 0; x < map_image.getSize().x; x++)
    {
        for (unsigned short y = 0; y < map_image.getSize().y; y++)
        {
            sf::Color pixel_color = map_image.getPixel(x, y);

            // 1. CZARNY (0, 0, 0) -> Block1 (Stabilna podłoga)
            if (pixel_color == sf::Color(0, 0, 0))
            {
                map_vector[x][y] = Cell::Wall;
            }
            // 2. CZERWONY (237, 28, 36) -> Block1 wiszący (Cegła do niszczenia głową)
            else if (pixel_color == sf::Color(237, 28, 36))
            {
                map_vector[x][y] = Cell::Brick;
            }
            // 3. BORDOWY (136, 0, 21) -> Block2 (Dolna podłoga bez autouzupełniania dziur)
            else if (pixel_color == sf::Color(136, 0, 21))
            {
                map_vector[x][y] = Cell::Wall;
            }
            // 4. ŻÓŁTY (255, 242, 0) -> BookBlock (Platforma z książkami)
            else if (pixel_color == sf::Color(255, 242, 0))
            {
                map_vector[x][y] = Cell::Platform;
            }
            // 5. FIOLETOWY (163, 73, 164) -> MailBlock (Skrzynka pocztowa z lootem)
            else if (pixel_color == sf::Color(163, 73, 164))
            {
                map_vector[x][y] = Cell::MailBlock;
            }
            // 6. JASNONIEBIESKI (0, 162, 232) -> Punkt spawnu wrogów (50% Ocena 2.0 / 50% Profesor)
            else if (pixel_color == sf::Color(0, 162, 232))
            {
                map_vector[x][y] = Cell::Empty; // Sam kafel musi być pusty, żeby wróg mógł stać
                
                // Losowanie typu wroga: 0 = Ocena 2.0, 1 = Profesor
                unsigned char enemy_type = (rand() % 2 == 0) ? 0 : 1;
                enemy_positions.push_back({enemy_type, static_cast<float>(CELL_SIZE * x), static_cast<float>(CELL_SIZE * y)});
            }
        }
    }
}

// System wykrywania kolizji obiektów z kafelkami na mapie
std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hit_box) const
{
    std::vector<unsigned char> output(4, 0);

    // Przeliczanie pozycji z pikseli na indeksy kafelków w siatce
    short start_x = std::max(0.0f, floor(i_hit_box.left / CELL_SIZE));
    short end_x = std::min(static_cast<float>(map_width - 1), ceil((i_hit_box.left + i_hit_box.width) / CELL_SIZE));
    short start_y = std::max(0.0f, floor(i_hit_box.top / CELL_SIZE));
    short end_y = std::min(static_cast<float>(SCREEN_HEIGHT / CELL_SIZE - 1), ceil((i_hit_box.top + i_hit_box.height) / CELL_SIZE));

    for (short x = start_x; x < end_x; x++)
    {
        for (short y = start_y; y < end_y; y++)
        {
            // Sprawdzamy, czy dany kafelek blokuje ruch
            if (std::end(i_check_cells) != std::find(i_check_cells.begin(), i_check_cells.end(), map_vector[x][y]))
            {
                // Kolizja z lewej strony kafelka
                if (i_hit_box.left + i_hit_box.width - horizontal_speed > CELL_SIZE * x && i_hit_box.left - horizontal_speed < CELL_SIZE * (1 + x))
                {
                    // Kolizja od góry kafelka (stanięcie na ziemi)
                    if (i_hit_box.top + i_hit_box.height - vertical_speed <= CELL_SIZE * y)
                    {
                        output[0] = 1; 
                    }
                    // Kolizja od dołu kafelka (uderzenie głową)
                    else if (i_hit_box.top - vertical_speed >= CELL_SIZE * (1 + y))
                    {
                        output[1] = 1; 
                    }
                }

                // Kolizja z górną/dolną krawędzią kafelka
                if (i_hit_box.top + i_hit_box.height - vertical_speed > CELL_SIZE * y && i_hit_box.top - vertical_speed < CELL_SIZE * (1 + y))
                {
                    // Kolizja z prawej strony kafelka
                    if (i_hit_box.left + i_hit_box.width - horizontal_speed <= CELL_SIZE * x)
                    {
                        output[2] = 1; 
                    }
                    // Kolizja z lewej strony kafelka
                    else if (i_hit_box.left - horizontal_speed >= CELL_SIZE * (1 + x))
                    {
                        output[3] = 1; 
                    }
                }
            }
        }
    }

    return output;
}