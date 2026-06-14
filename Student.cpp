#include "Headers/Student.hpp"
#include "Headers/Global.hpp"

Student::Student() : x(0.0f), y(0.0f), vx(0.0f), vy(0.0f), dead(false), 
                     sugar_overdose(false), is_big(false), energy_drinks_collected(0), 
                     jump_timer(0), animation_timer(0.0f), current_frame(0) 
{
    tex_small.loadFromFile("Resources/juststudent.png");
    tex_big.loadFromFile("Resources/happybigstudent.png");
    tex_gameover.loadFromFile("Resources/gameover.png");
    
    tex_run[0].loadFromFile("Resources/run1.png");
    tex_run[1].loadFromFile("Resources/run2.png");
    tex_run[2].loadFromFile("Resources/run3.png");
    
    tex_jump[0].loadFromFile("Resources/jump1.png");
    tex_jump[1].loadFromFile("Resources/jump2.png");
    tex_jump[2].loadFromFile("Resources/jump3.png");
    
    tex_left[0].loadFromFile("Resources/left1.png");
    tex_left[1].loadFromFile("Resources/left2.png");

    sprite.setTexture(tex_small);
}

void Student::reset_stats()
{
    x = CELL_SIZE * 2.0f;
    y = CELL_SIZE * 5.0f;
    vx = 0.0f; vy = 0.0f;
    dead = false;
    is_big = false;
    energy_drinks_collected = 0;
    sprite.setTexture(tex_small);
    sprite.setPosition(sf::Vector2f{x, y});
}

void Student::update(unsigned short map_width, MapManager& map_manager, std::vector<EnergyDrink>& energy_drinks, const std::vector<std::shared_ptr<Enemy>>& enemies)
{
    if (dead) return;

    bool move_left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool move_right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
    bool jump_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);

    vx = (move_left ? -STUDENT_WALK_SPEED : (move_right ? STUDENT_WALK_SPEED : 0.0f));
    vy += GRAVITY;
    if (vy > MAX_VERTICAL_SPEED) vy = MAX_VERTICAL_SPEED;

    sf::FloatRect ground_check = get_hitbox();
    ground_check.position.y += 1.0f;
    bool on_ground = !map_manager.map_collision({Cell::Wall, Cell::Brick, Cell::Platform, Cell::MailBlock}, ground_check).empty();

    if (on_ground) {
        vy = (jump_pressed ? -STUDENT_JUMP_SPEED : 0.0f);
        if (jump_pressed) jump_timer = 10;
    } else if (jump_pressed && jump_timer > 0) {
        vy = -STUDENT_JUMP_SPEED;
        jump_timer--;
    } else {
        jump_timer = 0;
    }

    x += vx;
    if (x < 0) x = 0;
    if (x > (map_width * CELL_SIZE) - CELL_SIZE) x = (map_width * CELL_SIZE) - CELL_SIZE;

    y += vy;
    if (y > SCREEN_HEIGHT) die(false);

    animation_timer += 1.0f / 60.0f;
    if (animation_timer >= 0.15f) {
        current_frame = (current_frame + 1) % 3;
        animation_timer = 0.0f;
    }

    if (vy != 0.0f) sprite.setTexture(tex_jump[current_frame % 3]);
    else if (vx > 0.0f) { sprite.setTexture(tex_run[current_frame % 3]); sprite.setScale(1.0f, 1.0f); }
    else if (vx < 0.0f) { sprite.setTexture(tex_left[current_frame % 2]); }
    else sprite.setTexture(is_big ? tex_big : tex_small);

    sprite.setPosition(x, y);
}

void Student::die(bool time_out) {
    dead = true;
    sprite.setTexture(tex_gameover);
}

sf::FloatRect Student::get_hitbox() const {
    return sf::FloatRect(sf::Vector2f{x + 2.0f, y + 2.0f}, sf::Vector2f{CELL_SIZE - 4.0f, CELL_SIZE - 4.0f});
}

void Student::draw(sf::RenderWindow& window, unsigned int view_x) {
    sprite.setPosition(sf::Vector2f{x - view_x, y});
    window.draw(sprite);
}

float Student::get_x() const { return x; }
float Student::get_y() const { return y; }
bool Student::get_dead() const { return dead; }
bool Student::is_sugar_overdose() const { return sugar_overdose; }
unsigned int Student::get_energy_drinks() const { return energy_drinks_collected; }