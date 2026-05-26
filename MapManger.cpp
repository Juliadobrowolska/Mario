#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"

// Konstruktor klasy MapManager
MapManager::MapManager() :
    mail_anim_frame(0)
{
    // Inicjalizacja generatora liczb losowych
    srand(static_cast<unsigned int>(time(nullptr)));

    // Alokacja pionowej przestrzeni dla struktury world_map (sztywna wysokość 16 kafelków)
    world_map.resize(SEGMENT_HEIGHT);
}

// Inicjalizacja zasobów graficznych oraz ładowanie początkowej sekwencji mapy
bool MapManager::initialize()
{
    // Ładowanie pojedynczych tekstur kafelków bezpośrednio z folderu Resources
    if (!tex_floor1.loadFromFile("Resources/block1.png")) return false;       
    if (!tex_floor2.loadFromFile("Resources/block2.png")) return false;       
    if (!tex_floor3.loadFromFile("Resources/bookblock.png")) return false;    
    if (!tex_platform.loadFromFile("Resources/block1.png")) return false;     
    if (!tex_activated_mail.loadFromFile("Resources/mailblock4.png")) return false; 

    // Ładowanie 4 klatek animacji aktywnego bloku mailowego (od mailblock0 do mailblock3)
    tex_mail_frames.resize(4);
    for (int i = 0; i < 4; i++)
    {
        if (!tex_mail_frames[i].loadFromFile("Resources/mailblock" + std::to_string(i) + ".png"))
            return false;
    }

    // Ładowanie 7 klatek animacji punktów ECTS z uwzględnieniem pliku ECTS4.jpg
    tex_ects_frames.resize(7);
    for (int i = 0; i < 7; i++)
    {
        std::string extension = (i + 1 == 4) ? ".jpg" : ".png";
        if (!tex_ects_frames[i].loadFromFile("Resources/ECTS" + std::to_string(i + 1) + extension))
            return false;
    }

    // Wczytanie konfiguracji segmentów poziomów bezpośrednio z Resources
    stage1 = load_segment_from_image("Resources/level_1.png");
    stage2 = load_segment_from_image("Resources/level_2.png");
    stage3 = load_segment_from_image("Resources/level_3.png");

    // Weryfikacja poprawności załadowania struktur danych
    if (stage1.empty() || stage2.empty() || stage3.empty()) return false;

    // Przypisanie segmentu startowego do aktywnej mapy świata
    for (int y = 0; y < SEGMENT_HEIGHT; y++)
    {
        world_map[y] = stage1[y];
    }

    // Dołączenie kolejnego losowego segmentu na start rozgrywki
    append_random_segment();

    // Uruchomienie licznika czasu dla animacji bloków
    mail_anim_clock.restart();

    return true;
}
// Konwersja kolorów pikseli z pliku graficznego na kafelki logiczne
std::vector<std::vector<Cell>> MapManager::load_segment_from_image(const std::string& filename)
{
    std::vector<std::vector<Cell>> segment(SEGMENT_HEIGHT, std::vector<Cell>(SEGMENT_WIDTH, Cell::Empty));
    sf::Image img;

    if (!img.loadFromFile(filename))
    {
        return std::vector<std::vector<Cell>>();
    }

    // Iteracja po pikselach obrazu w celu zmapowania ich na typy komórek
    for (unsigned int y = 0; y < img.getSize().y && y < SEGMENT_HEIGHT; y++)
    {
        for (unsigned int x = 0; x < img.getSize().x && x < SEGMENT_WIDTH; x++)
        {
            sf::Color pixel = img.getPixel(sf::Vector2u(x, y));

            if (pixel == sf::Color::Black)           segment[y][x] = Cell::Floor1;
            else if (pixel == sf::Color::Red)        segment[y][x] = Cell::Floor2;
            else if (pixel == sf::Color::Yellow)     segment[y][x] = Cell::Floor3; // Żółty piksel przypisany do regału z książkami
            else if (pixel == sf::Color::Magenta)    segment[y][x] = Cell::MailBlock; // Fioletowy piksel przypisany do bloku maila
            else if (pixel == sf::Color::Blue)       segment[y][x] = Cell::Empty; // Rezerwacja pozycji pod generowanie przeciwników
            else                                     segment[y][x] = Cell::Empty;
        }
    }

    return segment;
}

// Dynamiczne dołączanie losowego segmentu na koniec aktywnej struktury świata
void MapManager::append_random_segment()
{
    int random_stage = rand() % 3;
    std::vector<std::vector<Cell>>* selected_stage = &stage1;

    if (random_stage == 1)      selected_stage = &stage2;
    else if (random_stage == 2) selected_stage = &stage3;

    // Przypisywanie nowych kafelków na koniec wektora world_map
    for (size_t y = 0; y < SEGMENT_HEIGHT; y++)
    {
        for (size_t x = 0; x < SEGMENT_WIDTH; x++)
        {
            Cell cell_to_add = (*selected_stage)[y][x];
            world_map[y].push_back(cell_to_add);
        }
    }
}

// Pobieranie aktualnej szerokości mapy świata w pikselach
unsigned short MapManager::get_map_width() const
{
    if (world_map.empty() || world_map[0].empty()) return 0;
    return world_map[0].size() * CELL_SIZE;
}

// Pobieranie typu komórki na określonych współrzędnych siatki kafelków
Cell MapManager::get_cell_at(int x, int y) const
{
    if (y < 0 || y >= SEGMENT_HEIGHT || x < 0 || x >= static_cast<int>(world_map[0].size()))
    {
        return Cell::Empty;
    }
    return world_map[y][x];
}

const sf::Texture& MapManager::get_tex_floor1() const
{
    return tex_floor1;
}

const sf::Texture& MapManager::get_tex_floor2() const
{
    return tex_floor2;
}

const sf::Texture& MapManager::get_tex_floor3() const
{
    return tex_floor3;
}

const sf::Texture& MapManager::get_tex_platform() const
{
    return tex_platform;
}

const sf::Texture& MapManager::get_tex_activated_mail() const
{
    return tex_activated_mail;
}

const sf::Texture& MapManager::get_current_mail_texture() const
{
    return tex_mail_frames[mail_anim_frame];
}

const sf::Texture& MapManager::get_ects_texture(unsigned int i_frame) const
{
    return tex_ects_frames[i_frame % tex_ects_frames.size()];
}

const std::vector<Object>& MapManager::get_question_block_coins() const
{
    return question_block_coins;
}

const std::vector<Object>& MapManager::get_brick_particles() const
{
    return brick_particles;
}

// Modyfikacja zawartości konkretnego kafelka mapy
void MapManager::set_map_cell(const unsigned short i_x, const unsigned short i_y, const Cell& i_cell)
{
    if (i_y < SEGMENT_HEIGHT && i_x < world_map[0].size())
    {
        world_map[i_y][i_x] = i_cell;
    }
}

// Generowanie cząsteczek zniszczonego bloku środowiskowego
void MapManager::add_brick_particles(const unsigned short i_x, const unsigned short i_y)
{
    brick_particles.push_back(Object(i_x, i_y, -0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
    brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y, 0.25f * BRICK_PARTICLE_SPEED, -1.5f * BRICK_PARTICLE_SPEED));
    brick_particles.push_back(Object(i_x, i_y + 0.5f * CELL_SIZE, -0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
    brick_particles.push_back(Object(i_x + 0.5f * CELL_SIZE, i_y + 0.5f * CELL_SIZE, 0.5f * BRICK_PARTICLE_SPEED, -BRICK_PARTICLE_SPEED));
}

// Dodawanie wyskakującego punktu ECTS po aktywacji bloku mailowego
void MapManager::add_question_block_coin(const unsigned short i_x, const unsigned short i_y)
{
    Object new_ects(i_x, i_y, 0, ECTS_JUMP_SPEED);
    new_ects.current_anim_frame = 0; 
    question_block_coins.push_back(new_ects);
}

// Aktualizacja fizyki cząsteczek, punktów ECTS oraz stanów klatek animacji
void MapManager::update(unsigned int view_x)
{
    // Aktualizacja klatek animacji kafelków mailowych na podstawie upływu czasu (zakres 0 do 3)
    if (mail_anim_clock.getElapsedTime().asSeconds() >= 0.15f)
    {
        mail_anim_frame = (mail_anim_frame + 1) % 4;
        mail_anim_clock.restart();
    }

    // Aktualizacja pozycji oraz klatek animacji wyskakujących punktów ECTS
    for (Object& ects_point : question_block_coins)
    {
        ects_point.vertical_speed += GRAVITY;
        ects_point.y += ects_point.vertical_speed;

        // Cykliczne przełączanie wewnętrznej klatki animacji punktu ECTS (zakres 0 do 6)
        ects_point.current_anim_frame = (ects_point.current_anim_frame + 1) % 7;
    }

    // Aktualizacja pozycji odłamków bloków
    for (Object& brick_particle : brick_particles)
    {
        brick_particle.vertical_speed += GRAVITY;
        brick_particle.x += brick_particle.horizontal_speed;
        brick_particle.y += brick_particle.vertical_speed;
    }

    // Usuwanie odłamków znajdujących się poniżej krawędzi ekranu
    brick_particles.erase(std::remove_if(brick_particles.begin(), brick_particles.end(), [](const Object& p) {
        return SCREEN_HEIGHT <= p.y;
    }), brick_particles.end());

    // Usuwanie punktów ECTS po rozpoczęciu opadania
    question_block_coins.erase(std::remove_if(question_block_coins.begin(), question_block_coins.end(), [](const Object& c) {
        return 0 <= c.vertical_speed;
    }), question_block_coins.end());

    // Weryfikacja potrzeby wygenerowania nowego segmentu mapy na podstawie pozycji kamery
    unsigned int current_map_pixel_width = world_map[0].size() * CELL_SIZE;
    if (view_x + SCREEN_WIDTH + (SEGMENT_WIDTH * CELL_SIZE) > current_map_pixel_width)
    {
        append_random_segment();
    }
}

// Sprawdzanie kolizji w pionowych rzędach struktury kafelków
std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, const sf::FloatRect& i_hitbox) const
{
    std::vector<unsigned char> output;

    for (short a = floor(i_hitbox.position.y / CELL_SIZE); a <= floor((ceil(i_hitbox.size.y + i_hitbox.position.y) - 1) / CELL_SIZE); a++)
    {
        output.push_back(0);

        for (short b = floor(i_hitbox.position.x / CELL_SIZE); b <= floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE); b++)
        {
            if (0 <= b && b < static_cast<int>(world_map[0].size()))
            {
                if (0 <= a && a < SEGMENT_HEIGHT)
                {
                    if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), world_map[a][b]))
                    {
                        output[a - floor(i_hitbox.position.y / CELL_SIZE)] += pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b);
                    }
                }
            }
            else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
            {
                output[a - floor(i_hitbox.position.y / CELL_SIZE)] += pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b);
            }
        }
    }

    return output;
}

// Sprawdzanie kolizji z jednoczesną rejestracją współrzędnych modyfikowanych klocków
std::vector<unsigned char> MapManager::map_collision(const std::vector<Cell>& i_check_cells, std::vector<sf::Vector2i>& i_collision_cells, const sf::FloatRect& i_hitbox) const
{
    std::vector<unsigned char> output;
    i_collision_cells.clear();

    for (short a = floor(i_hitbox.position.y / CELL_SIZE); a <= floor((ceil(i_hitbox.size.y + i_hitbox.position.y) - 1) / CELL_SIZE); a++)
    {
        output.push_back(0);

        for (short b = floor(i_hitbox.position.x / CELL_SIZE); b <= floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE); b++)
        {
            if (0 <= b && b < static_cast<int>(world_map[0].size()))
            {
                if (0 <= a && a < SEGMENT_HEIGHT)
                {
                    if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), world_map[a][b]))
                    {
                        i_collision_cells.push_back(sf::Vector2i(b, a));
                        output[a - floor(i_hitbox.position.y / CELL_SIZE)] += pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b);
                    }
                }
            }
            else if (i_check_cells.end() != std::find(i_check_cells.begin(), i_check_cells.end(), Cell::Wall))
            {
                output[a - floor(i_hitbox.position.y / CELL_SIZE)] += pow(2, floor((ceil(i_hitbox.position.x + i_hitbox.size.x) - 1) / CELL_SIZE) - b);
            }
        }
    }

    return output;
}