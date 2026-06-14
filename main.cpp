#include <array>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/Animation.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/EnergyDrink.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Mark2.hpp"
#include "Headers/Professor.hpp"

// Definition of available game states
enum class GameState
{
	Menu,
	Playing,
	Pause,
	GameOver,
	TooMuchSugar
};

int main()
{
	sf::RenderWindow window(sf::VideoWindow(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2), "USOS: The Game", sf::Style::Close);
	window.setView(sf::View(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)));

	srand(static_cast<unsigned int>(time(0)));

	sf::Font font;
	if (!font.loadFromFile("Resources/arial.ttf"))
	{
		return -1;
	}

	sf::Text hud_text;
	hud_text.setFont(font);
	hud_text.setCharacterSize(12);
	hud_text.setFillColor(sf::Color::White);

	sf::Text menu_text;
	menu_text.setFont(font);
	menu_text.setCharacterSize(14);
	menu_text.setFillColor(sf::Color::White);

	// Separate text object for the main game title
	sf::Text title_text;
	title_text.setFont(font);
	title_text.setCharacterSize(20);
	title_text.setFillColor(sf::Color::Yellow);

	// Separate text objects for difficulty options to handle different colors
	sf::Text easy_text;
	easy_text.setFont(font);
	easy_text.setCharacterSize(14);

	sf::Text hard_text;
	hard_text.setFont(font);
	hard_text.setCharacterSize(14);

	GameState game_state = GameState::Menu;
	unsigned int ects_points = 0;
	unsigned short game_timer = 400;
	bool hard_mode = false;

	sf::Clock game_clock;
	float time_accumulator = 0.0f;

	Student student;
	MapManager map_manager;
	std::vector<EnergyDrink> energy_drinks;
	std::vector<std::shared_ptr<Enemy>> enemies;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (game_state == GameState::Menu)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
					{
						hard_mode = !hard_mode;
					}
					if (event.key.code == sf::Keyboard::Enter)
					{
						map_manager.load_level(1);
						student.reset_stats();
						energy_drinks.clear();
						enemies.clear();

						for (const auto& pos : map_manager.get_enemy_positions())
						{
							if (pos.type == 0)
							{
								enemies.push_back(std::make_shared<Mark2>(pos.x, pos.y));
							}
							else
							{
								enemies.push_back(std::make_shared<Professor>(pos.x, pos.y));
							}
						}

						ects_points = 0;
						game_timer = hard_mode ? 200 : 400;
						game_clock.restart();
						game_state = GameState::Playing;
					}
				}
			}
			// Handle enabling and disabling pause during gameplay
			else if (game_state == GameState::Playing)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape)
					{
						game_state = GameState::Pause;
					}
				}
			}
			else if (game_state == GameState::Pause)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape)
					{
						game_state = GameState::Playing;
						game_clock.restart(); // Restart the clock to prevent physics from jumping after unpausing
					}
				}
			}
			else if (game_state == GameState::GameOver || game_state == GameState::TooMuchSugar)
			{
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
				{
					game_state = GameState::Menu;
				}
			}
		}

		window.clear(sf::Color(92, 148, 252));

		if (game_state == GameState::Menu)
		{
			// Drawing the main game title at the top
			title_text.setString("SUPER STUDENT BROS.");
			title_text.setPosition(45, 15);
			window.draw(title_text);

			menu_text.setString("Time for a break \n\nTake control over the Student!\n\nSelect difficulty:");
			menu_text.setPosition(20, 50);
			window.draw(menu_text);

			// Configure Easy option (Green)
			easy_text.setString(hard_mode ? "  Easy (Calm semester)" : "> Easy (Calm semester)");
			easy_text.setFillColor(sf::Color::Green);
			easy_text.setPosition(40, 135);
			window.draw(easy_text);

			// Configure Hard option (Red)
			hard_text.setString(hard_mode ? "> Hard (Too much exams)" : "  Hard (Too much exams)");
			hard_text.setFillColor(sf::Color::Red);
			hard_text.setPosition(40, 160);
			window.draw(hard_text);

			menu_text.setString("Press ENTER to start the session...");
			menu_text.setPosition(30, 200);
			window.draw(menu_text);
		}
		// Logic for rendering the game world during active gameplay OR pause
		else if (game_state == GameState::Playing || game_state == GameState::Pause)
		{
			float dt = game_clock.restart().asSeconds();
			
			// Update physics and timers ONLY when the game is NOT paused
			if (game_state == GameState::Playing)
			{
				time_accumulator += dt;

				static float second_counter = 0.0f;
				second_counter += dt;
				if (second_counter >= 1.0f)
				{
					if (game_timer > 0) game_timer--;
					else student.die(true);
					second_counter = 0.0f;
				}

				while (time_accumulator >= 1.0f / 60.0f)
				{
					unsigned int view_x = std::max(0, static_cast<int>(student.get_x()) - SCREEN_WIDTH / 2);
					view_x = std::min(view_x, static_cast<unsigned int>(map_manager.get_map_width() * CELL_SIZE - SCREEN_WIDTH));

					student.update(map_manager.get_map_width(), map_manager, energy_drinks, enemies);

					while (student.has_queued_enemy())
					{
						auto queued = student.pop_queued_enemy();
						if (queued.first == 0)
						{
							enemies.push_back(std::make_shared<Mark2>(queued.second.x, queued.second.y));
						}
						else
						{
							enemies.push_back(std::make_shared<Professor>(queued.second.x, queued.second.y));
						}
					}

					for (auto& drink : energy_drinks)
					{
						drink.update(view_x, map_manager, student);
					}

					for (auto& enemy : enemies)
					{
						enemy->update(view_x, enemies, map_manager, student);
					}

					if (student.get_dead())
					{
						game_state = student.is_sugar_overdose() ? GameState::TooMuchSugar : GameState::GameOver;
					}

					time_accumulator -= 1.0f / 60.0f;
				}
			}

			// Rendering (always displays, so you see a frozen game screen on pause)
			unsigned int view_x = std::max(0, static_cast<int>(student.get_x()) - SCREEN_WIDTH / 2);
			view_x = std::min(view_x, static_cast<unsigned int>(map_manager.get_map_width() * CELL_SIZE - SCREEN_WIDTH));

			for (unsigned short x = view_x / CELL_SIZE; x < (view_x + SCREEN_WIDTH) / CELL_SIZE + 1; x++)
			{
				for (unsigned short y = 0; y < SCREEN_HEIGHT / CELL_SIZE; y++)
				{
					Cell cell = map_manager.get_cell(x, y);
					if (cell != Cell::Empty)
					{
						sf::Sprite block_sprite;
						sf::Texture block_texture;

						if (cell == Cell::Wall)
						{
							block_texture.loadFromFile(y >= SCREEN_HEIGHT / CELL_SIZE - 2 ? "Resources/block2.png" : "Resources/block1.png");
						}
						else if (cell == Cell::Brick)
						{
							block_texture.loadFromFile("Resources/block1.png");
						}
						else if (cell == Cell::Platform)
						{
							block_texture.loadFromFile("Resources/bookblock.png");
						}
						else if (cell == Cell::MailBlock)
						{
							block_texture.loadFromFile("Resources/mailblock3.png");
						}
						else if (cell == Cell::ActivatedMailBlock)
						{
							block_texture.loadFromFile("Resources/mailblock0.png");
						}

						block_sprite.setTexture(block_texture);
						block_sprite.setPosition(CELL_SIZE * x - view_x, CELL_SIZE * y);
						window.draw(block_sprite);
					}
				}
			}

			for (auto& drink : energy_drinks) drink.draw(window, view_x);
			for (auto& enemy : enemies) enemy->draw(view_x, window);
			student.draw(window, view_x);

			hud_text.setString("STUDENT   ECTS: " + std::to_string(ects_points) + "   TIME: " + std::to_string(game_timer));
			hud_text.setPosition(10, 5);
			window.draw(hud_text);

			// If pause is active, overlay an information message
			if (game_state == GameState::Pause)
			{
				menu_text.setString("DEAN'S OFFICE CLOSED\n       (PAUSE)\n\nPress P to resume...");
				menu_text.setPosition(60, 90);
				window.draw(menu_text);
			}
		}
		else if (game_state == GameState::GameOver)
		{
			menu_text.setString("SESSION FAILED!\n\nYou didn't get enough ECTS.\n\nPress ENTER to return to menu...");
			menu_text.setPosition(30, 80);
			window.draw(menu_text);
		}
		else if (game_state == GameState::TooMuchSugar)
		{
			window.clear(sf::Color(139, 0, 0));
			menu_text.setString("TOO MUCH SUGAR!\n\nStudent's heart couldn't handle the third energy drink.\nCaffeine overdose!\n\nPress ENTER to try again...");
			menu_text.setPosition(20, 60);
			window.draw(menu_text);
		}

		window.display();
	}

	return 0;
}