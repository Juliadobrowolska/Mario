#include <array>
#include <chrono>
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Headers/Animation.hpp"
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Goomba.hpp"
#include "Headers/Koopa.hpp"

int main()
{
	unsigned short level_finish = 0;

	std::chrono::microseconds lag(0);
	std::chrono::steady_clock::time_point previous_time;

	std::vector<std::shared_ptr<Enemy>> enemies;

	sf::Color background_color = sf::Color(0, 219, 255);
	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(SCREEN_RESIZE * SCREEN_WIDTH, SCREEN_RESIZE * SCREEN_HEIGHT), "Student Bros", sf::Style::Close);
	window.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y - 90));

	sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));

	MapManager map_manager;
	Student student;

	map_manager.init_level_system();
	level_finish = map_manager.get_map_sketch_width() - 2;
	student.set_position(CELL_SIZE * 2, CELL_SIZE * 2);

	previous_time = std::chrono::steady_clock::now();

	while (true == window.isOpen())
	{
		std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
		lag += delta_time;
		previous_time += delta_time;

		while (FRAME_DURATION <= lag)
		{
			unsigned view_x;
			lag -= FRAME_DURATION;

			while (true == window.pollEvent(event))
			{
				switch (event.type)
				{
					case sf::Event::Closed:
					{
						window.close();
						break;
					}
					case sf::Event::KeyPressed:
					{
						switch (event.key.code)
						{
							case sf::Keyboard::Enter:
							{
								enemies.clear();
								student.reset();
								map_manager.init_level_system();
								student.set_position(CELL_SIZE * 2, CELL_SIZE * 2);
								level_finish = map_manager.get_map_sketch_width() - 2;
								break;
							}
						}
						break;
					}
				}
			}

			if (CELL_SIZE * level_finish <= student.get_x())
			{
				enemies.clear();
				student.reset();

				map_manager.load_next_random_level();

				student.set_position(CELL_SIZE * 2, CELL_SIZE * 2);
				level_finish = map_manager.get_map_sketch_width() - 2;
			}

			view_x = std::clamp<int>(round(student.get_x()) - 0.5f * (SCREEN_WIDTH - CELL_SIZE), 0, map_manager.get_map_width() - SCREEN_WIDTH);

			map_manager.update();
			student.update(view_x, map_manager);

			for (unsigned short a = 0; a < enemies.size(); a++)
			{
				enemies[a]->update(view_x, enemies, map_manager, student);
			}

			for (unsigned short a = 0; a < enemies.size(); a++)
			{
				if (true == enemies[a]->get_dead(1))
				{
					enemies.erase(a + enemies.begin());
					a--;
				}
			}

			if (FRAME_DURATION > lag)
			{
				view.reset(sf::FloatRect(view_x, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
				window.setView(view);
				window.clear(background_color);

				map_manager.draw_map(true, view_x, window);

				student.draw_indexes(view_x, window);

				map_manager.draw_map(false, view_x, window);

				for (unsigned short a = 0; a < enemies.size(); a++)
				{
					enemies[a]->draw(view_x, window);
				}

				student.draw(window);

				window.display();
			}
		}
	}
}