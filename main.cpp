#include <SFML/Graphics.hpp>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(SCREEN_WIDTH * SCREEN_RESIZE), static_cast<unsigned int>(SCREEN_HEIGHT * SCREEN_RESIZE)}), "Super Mario Game");
    window.setFramerateLimit(60);

    MapManager map_manager;
    if (!map_manager.initialize())
    {
        std::cout << "Critical error: Failed to load assets from Resources directory!" << std::endl;
        return -1;
    }

    Student student;
    // Wczytanie tekstur postaci studenta (run1, run2, juststudent itp.)
    if (!student.load_textures())
    {
        std::cout << "Critical error: Failed to load student textures!" << std::endl;
        return -1;
    }

    // Ustawienie pozycji startowej na kafelku nad podłożem
    student.set_position(100.0f, 100.0f);

    unsigned int camera_x = 0;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        map_manager.update(camera_x);
        student.update(camera_x, map_manager);

        if (student.get_x() > SCREEN_WIDTH / 2.0f)
        {
            camera_x = static_cast<unsigned int>(student.get_x() - SCREEN_WIDTH / 2.0f);
        }
        else
        {
            camera_x = 0;
        }

        unsigned int max_camera_x = map_manager.get_map_width() - SCREEN_WIDTH;
        if (camera_x > max_camera_x)
        {
            camera_x = max_camera_x;
        }

        window.clear(sf::Color::Black);
        
        map_manager.draw_map(false, false, camera_x, window);
        student.draw(window, camera_x);
        
        window.display();
    }

    return 0;
}