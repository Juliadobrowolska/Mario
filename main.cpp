#include <SFML/Graphics.hpp>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"

int main()
{
    // Utworzenie okna gry z uwzględnieniem skalowania SCREEN_RESIZE
    sf::RenderWindow window(sf::VideoMode({static_cast<unsigned int>(SCREEN_WIDTH * SCREEN_RESIZE), static_cast<unsigned int>(SCREEN_HEIGHT * SCREEN_RESIZE)}), "Super Mario Game");
    window.setFramerateLimit(60);

    MapManager map_manager;
    
    // Inicjalizacja zasobów i ładowanie tekstur z folderu Resources
    if (!map_manager.initialize())
    {
        std::cout << "Critical error: Failed to load assets from Resources directory!" << std::endl;
        return -1;
    }

    Student student;
    // Ustawienie pozycji startowej obiektu Student w powietrzu
    student.set_position(100.0f, 100.0f);

    unsigned int camera_x = 0;

    // Główna pętla gry
    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Aktualizacja managera mapy na podstawie pozycji kamery
        map_manager.update(camera_x);
        
        // Aktualizacja fizyki oraz pozycji obiektu Student
        student.update(camera_x, map_manager);

        // Aktualizacja pozycji kamery w celu wyśrodkowania widoku na graczu
        if (student.get_x() > SCREEN_WIDTH / 2.0f)
        {
            camera_x = static_cast<unsigned int>(student.get_x() - SCREEN_WIDTH / 2.0f);
        }
        else
        {
            camera_x = 0;
        }

        // Czyszczenie bufora ekranu
        window.clear(sf::Color::Black);
        
        // Renderowanie kafelków mapy oraz obiektów zależnych od przesunięcia kamery
        map_manager.draw_map(false, false, camera_x, window);
        
        // Renderowanie obiektu Student
        student.draw(window);
        
        // Wyświetlenie wyrenderowanej klatki
        window.display();
    }

    return 0;
}