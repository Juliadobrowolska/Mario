#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Global.hpp"

class MapManager
{
private:
    static const int SEGMENT_WIDTH = 40;
    static const int SEGMENT_HEIGHT = 16;

    // Struktury do obsługi cząsteczek i monet w locie
    std::vector<Object> brick_particles;
    std::vector<Object> question_block_coins;

    // osobne tekstury dla klocków 80x80
    sf::Texture tex_floor1;
    sf::Texture tex_floor2;
    sf::Texture tex_floor3;
    sf::Texture tex_platform;
    sf::Texture tex_activated_mail;

    // Kontener na 4 klatki animacji maila i zegar SFML
    std::vector<sf::Texture> tex_mail_frames;
    unsigned int mail_anim_frame;
    sf::Clock mail_anim_clock;

    // Kontener na 7 klatek animacji punktów ECTS
    std::vector<sf::Texture> tex_ects_frames;

    // Trzy segmenty wczytane z  rysunków w Paintcie
    std::vector<std::vector<Cell>> stage1;
    std::vector<std::vector<Cell>> stage2;
    std::vector<std::vector<Cell>> stage3;

    //  dynamiczna, nieskończona mapa gry
    std::vector<std::vector<Cell>> world_map;

    // Funkcja zamieniająca kolory pikseli z plików PNG na klocki
    std::vector<std::vector<Cell>> load_segment_from_image(const std::string& filename);

public:
    MapManager();

    // Inicjalizacja: ładuje wszystkie obrazki (w tym klocki, maile i ECTS) i tworzy początek mapy
    bool initialize();

    // Dokleja losowy segment (level_2 lub level_3) na koniec świata
    void append_random_segment();

    // Funkcje pobierające wymiary
    unsigned short get_map_sketch_height() const { return SEGMENT_HEIGHT; }
    unsigned short get_map_sketch_width() const { return SEGMENT_WIDTH; }
    unsigned short get_map_width() const;

    // Dostęp do tekstur i obiektów do rysowania
    const sf::Texture& get_tex_floor1() const;
    const sf::Texture& get_tex_floor2() const;
    const sf::Texture& get_tex_floor3() const;
    const sf::Texture& get_tex_platform() const;
    const sf::Texture& get_tex_activated_mail() const;
    const sf::Texture& get_current_mail_texture() const;
    const sf::Texture& get_ects_texture(unsigned int i_frame) const;
    const std::vector<Object>& get_question_block_coins() const;
    const std::vector<Object>& get_brick_particles() const;

    // Funkcje do obsługi cząsteczek i ECTS
    void add_brick_particles(const unsigned short i_x, const unsigned short i_y);
    void add_question_block_coin(const unsigned short i_x, const unsigned short i_y);
    
    // Główna funkcja rysująca
    void draw_map(const bool i_draw_background, const bool i_underground, const unsigned i_view_x, sf::RenderWindow& i_window);
    
    // Funkcje modyfikujące kafelki w locie
    void set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell);
    
    // Aktualizacja animacji i sprawdzanie, czy trzeba dokleić mapę
    void update(unsigned int view_x);

    // Dwie  funkcje kolizji potrzebne dla Studenta
    std::vector<unsigned char> map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const;
    std::vector<unsigned char> map_collision(const std::vector<Cell>& i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const;

    // Pobieranie konkretnego kafelka z mapy
    Cell get_cell_at(int x, int y) const;
};