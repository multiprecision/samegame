/*
 * Author: Samuel I. Gunadi
 */

#ifndef SAMEGAME_MAINWINDOW_H
#define SAMEGAME_MAINWINDOW_H

#include <vector>
#include <queue>
#include <cstdint>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <SFML/Graphics.hpp>

namespace SameGame
{

class MainWindow
{
public:
	MainWindow();
	~MainWindow();
	void readConfiguration();
	void writeDefaultConfiguration();
	void checkConfiguration();
	void startNewGame();
	bool removeBlocks();
	void render();
	void handleEvents();
private:
	enum class Direction : char
	{
		None,
		Right,
		Up,
		Left,
		Down
	};

	struct AnimationState
	{
		uint32_t m_duration1;
		uint32_t m_duration2;
		uint32_t m_score;
		std::pair<std::vector<std::vector<uint8_t>>, std::vector<std::vector<uint8_t>>> m_blocks_state;
		float m_x;
		float m_y;
	};
	/* 2-dimensional array containing block types */
	std::vector<std::vector<uint8_t>> m_blocks;
	std::vector<std::vector<sf::Sprite>> m_block_sprites;
	sf::Image m_block_image;
	sf::Texture m_block_texture;
	std::vector<sf::Color> m_block_colors;
	sf::Font m_font;
	sf::RenderWindow* m_window;
	std::queue<AnimationState> m_anm_state;
	uint32_t m_fps = 0;
	bool m_anim_playing = false;
	uint32_t m_score = 0;
	/* Random number generator */
	boost::random::mt19937 m_rng;
	boost::random::uniform_int_distribution<> m_uniform_dist;
	/* Mouse positions */
	int32_t m_press_x = -1;
	int32_t m_press_y = -1;
	int32_t m_release_x = -1;
	int32_t m_release_y = -1;
	uint16_t m_selected_row = UINT16_MAX;
	uint16_t m_selected_column = UINT16_MAX;
	/* Configuration */
	uint16_t m_column_size = 0;
	uint16_t m_row_size = 0;
	uint16_t m_block_width = 0;
	uint16_t m_block_height = 0;
	uint8_t m_block_needed = 0;
};

} // Namespace SameGame

#endif // SAMEGAME_MAINWINDOW_H