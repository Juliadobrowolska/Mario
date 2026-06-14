#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "Global.hpp"

class MapManager
{
public:
    // Struktura przechowująca dane o wrogach wykrytych na mapie pikseli
    struct EnemyPosition
    {
        unsigned char type; // 0 = Ocena 2.0, 1 = Profesor
        float x;
        float y;
    };

    MapManager();

    // Główna funkcja ładująca grafikę poziomu
    void load_level(const unsigned char i_level);
    
    // Funkcja sprawdzająca kolizje obiektów z otoczeniem
    std::vector<unsigned char> map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hit_box) const;

    // Pobieranie pozycji wrogów do zespawnowania w main.cpp
    const std::vector<EnemyPosition>& get_enemy_positions() const;
    
    // Czyszczenie danych przed załadowaniem nowej mapy
    void clear_enemies();

    // Gettery i Settery dla komórek mapy
    Cell get_cell(const unsigned short i_x, const unsigned short i_y) const;
    void set_cell(const unsigned short i_x, const unsigned short i_y, const Cell i_cell);
    
    unsigned short get_map_width() const;

private:
    unsigned short map_width;
    
    // Dwuwymiarowy wektor reprezentujący siatkę klocków w grze
    std::vector<std::vector<Cell>> map_vector;
    
    // Lista pozycji wrogów wyciągnięta z rysunku poziomu
    std::vector<EnemyPosition> enemy_positions;
};