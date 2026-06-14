#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "Headers/Global.hpp"
#include "Headers/MapManager.hpp"
#include "Headers/Student.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/Mark2.hpp"
#include "Headers/Professor.hpp"
#include "Headers/EnergyDrink.hpp"

enum class GameState
{
	Menu,
	Playing,
	Pause,
	GameOver
};

int main()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// Tworzymy większe okno systemowe (960x720), aby gra nie była miniaturowa
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u{960, 720}), "Super Student Bros", sf::Style::Close);
	window.setFramerateLimit(60);

	// Tworzymy widok SFML o logicznej rozdzielczości retro (320x240)
	// Dzięki temu SFML sam idealnie i ostro rozciągnie grafikę na całe okno!
	sf::View game_view(sf::FloatRect(sf::Vector2f{0.0f, 0.0f}, sf::Vector2f{static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)}));
	window.setView(game_view);

	sf::Font font;
	if (!font.openFromFile("Resources/arial.ttf"))
	{
		return -1;
	}

	sf::Texture texture_block1, texture_block2, texture_bookblock, texture_mailblock3, texture_mailblock0;
	texture_block1.loadFromFile("Resources/block1.png");
	texture_block2.loadFromFile("Resources/block2.png");
	texture_bookblock.loadFromFile("Resources/bookblock.png");
	texture_mailblock3.loadFromFile("Resources/mailblock3.png");
	texture_mailblock0.loadFromFile("Resources/mailblock0.png");

	sf::Sprite sprite_block1(texture_block1);
	sf::Sprite sprite_block2(texture_block2);
	sf::Sprite sprite_bookblock(texture_bookblock);
	sf::Sprite sprite_mailblock3(texture_mailblock3);
	sf::Sprite sprite_mailblock0(texture_mailblock0);

	MapManager map_manager;
	Student student;
	std::vector<EnergyDrink> energy_drinks;
	std::vector<std::shared_ptr<Enemy>> enemies;

	GameState game_state = GameState::Menu;
	unsigned short current_level = 1;
	unsigned int ects_points = 0;
	float game_timer = 400.0f;

	sf::Clock delta_clock;
	float time_accumulator = 0.0f;
	const float TIME_STEP = 1.0f / 60.0f;

	// UI i Teksty dopasowane rozmiarowo do logicznego ekranu 320x240
	sf::Text menu_title(font, "SUPER STUDENT BROS", 18);
	menu_title.setFillColor(sf::Color::Yellow);
	menu_title.setPosition(sf::Vector2f{20.0f, 20.0f});

	sf::Text menu_options(font, "Nacisnij ENTER aby zaczac\n\nUzyj STRZALEK gora/dol by wybrac trudnosc:", 10);
	menu_options.setFillColor(sf::Color::White);
	menu_options.setPosition(sf::Vector2f{20.0f, 60.0f});

	sf::Text difficulty_text(font, "", 11);
	difficulty_text.setPosition(sf::Vector2f{20.0f, 110.0f});

	sf::Text hud_text(font, "", 9);
	hud_text.setFillColor(sf::Color::White);

	sf::Text pause_text(font, "PAUZA\n\nNacisnij P aby wznowic", 16);
	pause_text.setFillColor(sf::Color::White);
	pause_text.setPosition(sf::Vector2f{80.0f, 90.0f});

	sf::Text game_over_text(font, "GAME OVER\n\nNacisnij ENTER aby wrocic", 14);
	game_over_text.setFillColor(sf::Color::Red);
	game_over_text.setPosition(sf::Vector2f{60.0f, 100.0f});

	while (window.isOpen())
	{
		while (auto event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else if (event->is<sf::Event::KeyPressed>())
			{
				auto key_event = event->getIf<sf::Event::KeyPressed>();
				if (key_event)
				{
					auto key_code = key_event->code;
					if (game_state == GameState::Menu)
					{
						if (key_code == sf::Keyboard::Key::Up || key_code == sf::Keyboard::Key::Down)
						{
							hard_mode = !hard_mode;
						}

						if (key_code == sf::Keyboard::Key::Enter)
						{
							current_level = 1;
							if (map_manager.load_level(current_level))
							{
								student.reset_stats();
								energy_drinks.clear();
								enemies.clear();

								for (const auto& pos : map_manager.get_enemy_positions())
								{
									if (pos.type == 0)
										enemies.push_back(std::make_shared<Mark2>(pos.x, pos.y));
									else
										enemies.push_back(std::make_shared<Professor>(pos.x, pos.y));
								}

								ects_points = 0;
								game_timer = hard_mode ? 200.0f : 400.0f;
								delta_clock.restart();
								game_state = GameState::Playing;
							}
						}
					}
					else if (game_state == GameState::Playing && key_code == sf::Keyboard::Key::P)
					{
						game_state = GameState::Pause;
					}
					else if (game_state == GameState::Pause && key_code == sf::Keyboard::Key::P)
					{
						delta_clock.restart();
						game_state = GameState::Playing;
					}
					else if (game_state == GameState::GameOver && key_code == sf::Keyboard::Key::Enter)
					{
						game_state = GameState::Menu;
					}
				}
			}
		}

		// --- LOGIKA AKTUALIZACJI (Fizyka 60 FPS) ---
		if (game_state == GameState::Playing)
		{
			float dt = delta_clock.restart().asSeconds();
			time_accumulator += dt;

			game_timer -= dt;
			if (game_timer <= 0.0f)
			{
				game_timer = 0.0f;
				student.die(true);
			}

			if (student.get_dead())
			{
				game_state = GameState::GameOver;
			}

			while (time_accumulator >= TIME_STEP)
			{
				student.update(map_manager.get_map_width(), map_manager, energy_drinks, enemies);

				for (auto& drink : energy_drinks)
				{
					drink.update(0, map_manager, student);
				}

				for (auto& enemy : enemies)
				{
					enemy->update(0, enemies, map_manager, student);
				}

				ects_points = student.get_energy_drinks() * 10;

				if (student.get_x() >= (map_manager.get_map_width() - 2) * CELL_SIZE)
				{
					current_level = (std::rand() % 2) + 2;
					
					if (map_manager.load_level(current_level))
					{
						student.reset_stats();
						energy_drinks.clear();
						enemies.clear();

						for (const auto& pos : map_manager.get_enemy_positions())
						{
							if (pos.type == 0)
								enemies.push_back(std::make_shared<Mark2>(pos.x, pos.y));
							else
								enemies.push_back(std::make_shared<Professor>(pos.x, pos.y));
						}
						game_timer += hard_mode ? 100.0f : 200.0f;
					}
				}

				time_accumulator -= TIME_STEP;
			}
		}

		// --- OBLICZANIE KAMERY (PREWIJANIE) ---
		unsigned int view_x = 0;
		if (game_state == GameState::Playing || game_state == GameState::Pause)
		{
			if (student.get_x() > SCREEN_WIDTH / 2.0f)
			{
				view_x = static_cast<unsigned int>(student.get_x() - SCREEN_WIDTH / 2.0f);
				if (view_x > (map_manager.get_map_width() * CELL_SIZE) - SCREEN_WIDTH)
				{
					view_x = (map_manager.get_map_width() * CELL_SIZE) - SCREEN_WIDTH;
				}
			}
		}

		// --- RYSOWANIE EKRANU ---
		window.clear(sf::Color(100, 149, 237));

		if (game_state == GameState::Menu)
		{
			// Przed rysowaniem menu resetujemy widok kamery do pozycji wyjściowej (0,0)
			game_view.setCenter(sf::Vector2f{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
			window.setView(game_view);

			if (hard_mode)
			{
				difficulty_text.setString("TRYB: TRUDNY (Kampania Wrzesniowa)");
				difficulty_text.setFillColor(sf::Color::Red);
			}
			else
			{
				difficulty_text.setString("TRYB: NORMALNY (Pierwszy Termin)");
				difficulty_text.setFillColor(sf::Color::Green);
			}

			window.draw(menu_title);
			window.draw(menu_options);
			window.draw(difficulty_text);
		}
		else if (game_state == GameState::Playing || game_state == GameState::Pause)
		{
			// Ustawiamy widok kamery na środek ekranu z uwzględnieniem przewijania (view_x)
			game_view.setCenter(sf::Vector2f{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
			window.setView(game_view);

			short start_tile_x = static_cast<short>(view_x / CELL_SIZE);
			short end_tile_x = start_tile_x + (SCREEN_WIDTH / CELL_SIZE) + 2;

			for (short x = start_tile_x; x < end_tile_x; x++)
			{
				for (short y = 0; y < SCREEN_HEIGHT / CELL_SIZE; y++)
				{
					Cell cell = map_manager.get_cell(x, y);
					float draw_x = static_cast<float>(x * CELL_SIZE - view_x);
					float draw_y = static_cast<float>(y * CELL_SIZE);

					if (cell == Cell::Wall || cell == Cell::Brick)
					{
						if (y >= (SCREEN_HEIGHT / CELL_SIZE) - 2)
						{
							sprite_block2.setPosition(sf::Vector2f{static_cast<float>(draw_x), static_cast<float>(draw_y)});
							window.draw(sprite_block2);
						}
						else
						{
							sprite_block1.setPosition(sf::Vector2f{static_cast<float>(draw_x), static_cast<float>(draw_y)});
							window.draw(sprite_block1);
						}
					}
					else if (cell == Cell::Platform)
					{
						sprite_bookblock.setPosition(sf::Vector2f{static_cast<float>(draw_x), static_cast<float>(draw_y)});
						window.draw(sprite_bookblock);
					}
					else if (cell == Cell::MailBlock)
					{
						sprite_mailblock3.setPosition(sf::Vector2f{static_cast<float>(draw_x), static_cast<float>(draw_y)});
						window.draw(sprite_mailblock3);
					}
					else if (cell == Cell::ActivatedMailBlock)
					{
						sprite_mailblock0.setPosition(sf::Vector2f{static_cast<float>(draw_x), static_cast<float>(draw_y)});
						window.draw(sprite_mailblock0);
					}
				}
			}

			for (auto& drink : energy_drinks) drink.draw(window, view_x);
			for (auto& enemy : enemies) enemy->draw(view_x, window);
			student.draw(window, view_x);

			// HUD statyczny na górze ekranu
			std::string mode_label = hard_mode ? "HARD" : "NORMAL";
			hud_text.setString("ECTS: " + std::to_string(ects_points) + "  SEM: " + std::to_string(current_level) + "  TIME: " + std::to_string(static_cast<int>(game_timer)) + "  [" + mode_label + "]");
			hud_text.setPosition(sf::Vector2f{5.0f, 5.0f});
			window.draw(hud_text);

			if (game_state == GameState::Pause)
			{
				window.draw(pause_text);
			}
		}
		else if (game_state == GameState::GameOver)
		{
			game_view.setCenter(sf::Vector2f{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f});
			window.setView(game_view);
			window.draw(game_over_text);
		}

		window.display();
	}

	return 0;
}