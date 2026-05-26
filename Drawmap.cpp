#include <cmath>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "Headers/Global.hpp"
#include "Headers/DrawMap.hpp"
#include "Headers/MapManager.hpp"

void draw_map(unsigned i_view_x, sf::RenderWindow& i_window, const MapManager& i_map_manager)
{
	// OPTYMALIZACJA: Rysuje tylko to, co aktualnie widać na ekranie
	unsigned short map_start = floor(i_view_x / static_cast<float>(CELL_SIZE));
	unsigned short map_end = ceil((i_view_x + SCREEN_WIDTH) / static_cast<float>(CELL_SIZE));

	sf::Sprite cell_sprite(i_map_manager.get_tex_floor1());

	// kolumny (a) i pełną wysokość wierszy (b)
	for (unsigned short a = map_start; a < map_end; a++)
	{
		for (unsigned short b = 0; b < 16; b++) // 16 to sztywna wysokość  segmentu
		{
			// Zabezpieczenie przed wyjściem poza wygenerowany świat
			if (a >= i_map_manager.get_map_width() / CELL_SIZE) continue;

			// Pobieranie kafelek z managera mapy
			Cell current_cell = i_map_manager.get_cell_at(a, b);

			if (Cell::Empty == current_cell) continue;

			cell_sprite.setPosition(sf::Vector2f(CELL_SIZE * a, CELL_SIZE * b));

			// Przypisywanie odpowiednich tekstur z managera na podstawie typu kafelka
			if (Cell::Floor1 == current_cell)
			{
				cell_sprite.setTexture(i_map_manager.get_tex_floor1());
			}
			else if (Cell::Floor2 == current_cell)
			{
				cell_sprite.setTexture(i_map_manager.get_tex_floor2());
			}
			else if (Cell::Floor3 == current_cell)
			{
				cell_sprite.setTexture(i_map_manager.get_tex_floor3());
			}
			else if (Cell::Platform == current_cell)
			{
				cell_sprite.setTexture(i_map_manager.get_tex_platform());
			}
			else if (Cell::MailBlock == current_cell)
			{
				// Pobieranie aktualnej klatki mrugającego maila
				cell_sprite.setTexture(i_map_manager.get_current_mail_texture());
			}
			else if (Cell::ActivatedMailBlock == current_cell)
			{
				cell_sprite.setTexture(i_map_manager.get_tex_activated_mail());
			}

			i_window.draw(cell_sprite);
		}
	}

	// RYSOWANIE PUNKTÓW ECTS I CZĄSTECZEK (Pobieranie pętlami z managera)
	for (const Object& ects_point : i_map_manager.get_question_block_coins())
	{
		cell_sprite.setPosition(sf::Vector2f(ects_point.x, ects_point.y));
		cell_sprite.setTexture(i_map_manager.get_ects_texture(ects_point.current_anim_frame));
		i_window.draw(cell_sprite);
	}

	for (const Object& brick_particle : i_map_manager.get_brick_particles())
	{
		cell_sprite.setPosition(sf::Vector2f(brick_particle.x, brick_particle.y));
		cell_sprite.setTexture(i_map_manager.get_tex_floor1());
		cell_sprite.setTextureRect(sf::IntRect(sf::Vector2i{0, 0}, sf::Vector2i{CELL_SIZE / 4, CELL_SIZE / 4}));
		i_window.draw(cell_sprite);
	}
}