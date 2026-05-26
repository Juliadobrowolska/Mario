#include <SFML/Graphics.hpp>
#include <iostream>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"

int main()
{
    // Tworzenie okna gry o odpowiednich wymiarach z uwzględnieniem skalowania SCREEN_RESIZE
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u{static_cast<unsigned>(SCREEN_WIDTH * SCREEN_RESIZE), static_cast<unsigned>(SCREEN_HEIGHT * SCREEN_RESIZE)}), "Super mario game");
    window.setFramerateLimit(60);

    MapManager map_manager;
    
    // Kluczowa inicjalizacja - ładujemy grafiki i segmenty map z poziomu Resources
    if (!map_manager.initialize())
    {
        std::cout << "Blad krytyczny: Nie udalo sie zaladowac grafik z folderu Resources!" << std::endl;
        return -1;
    }

    Student student;
    // Ustawiamy pozycję startową Studenta w powietrzu, żeby fizyka sama go opuściła na kafelki mapy
    student.set_position(100.0f, 100.0f);

    unsigned view_x = 0;

    // Główna pętla gry
    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // Aktualizacja managera mapy z przekazaniem aktualnej pozycji kamery
        map_manager.update(view_x);
        
        // Aktualizacja fizyki i ruchu studenta
        student.update(view_x, map_manager);

        // Czyszczenie ekranu (tło)
        window.clear(sf::Color::Black);
        
        // Rysowanie mapy - przekazujemy okno SFML i pozycję kamery
        map_manager.draw_map(false, false, view_x, window);
        
        // Rysowanie studenta
        student.draw(window);
        
        // Wyświetlenie wyrenderowanej klatki na ekranie
        window.display();
    }

    return 0;
}