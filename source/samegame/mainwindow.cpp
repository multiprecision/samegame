/*
 * Author: Samuel I. Gunadi
 */


#include "mainwindow.h"
#include "content.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <string>
#include <stack>

namespace SameGame
{

MainWindow::MainWindow()
{
	/* Configuration */

	try
	{
		// Try to read the configuration file.
		readConfiguration();
	}
	catch (...)
	{
		// If something went wrong while reading configuration file,
		// create or overwrite the file with the default.
		writeDefaultConfiguration();
	}
	// Make sure configuration is valid.
	checkConfiguration();
	// Initialize the random number generator.
	m_uniform_dist = boost::random::uniform_int_distribution<>(0, static_cast<int>(m_block_colors.size() - 1));
	/* Load block image and texture. */
	try
	{
		if (!m_block_image.loadFromMemory(block_image, 52242) || !m_block_texture.loadFromImage(m_block_image))
			throw std::runtime_error("Error occured while loading image.");
		/* Load font. */
		if (!m_font.loadFromMemory(main_font, 58464))
			throw std::runtime_error("Error occured while loading font.");
	}
	catch (...)
	{
		exit(1);
	}
	/* Populate blocks. */
	m_blocks.resize(m_row_size);
	for (uint16_t row = 0; row < m_row_size; row++)
	{
		m_blocks[row].resize(m_column_size);
	}
	/*  Setup blocks. */
	startNewGame();
	m_window = new sf::RenderWindow(
		sf::VideoMode(m_block_width * m_column_size, m_block_height * (m_row_size + 1)),
		"Same Game",
		sf::Style::Titlebar | sf::Style::Close
	);
	m_window->setIcon(BLOCK_IMAGE_WIDTH, BLOCK_IMAGE_HEIGHT, m_block_image.getPixelsPtr());
	m_window->setVerticalSyncEnabled(true);
	sf::Clock clock;
	uint32_t frame_num = 0;
	/* Start Main loop. */
	while (m_window->isOpen())
	{
		if (clock.getElapsedTime().asSeconds() >= 1.f)
		{
			m_fps = frame_num;
			frame_num = 0;
			clock.restart();
			m_window->setTitle("Same Game | " + std::to_string(m_fps) + " FPS");
		}
		/* Handle events. */
		handleEvents();
		/* Clear the window. */
		m_window->clear(sf::Color::Black);
		/* Start drawing. */
		render();
		/* Display the window. */
		m_window->display();
		frame_num++;
	}
}

MainWindow::~MainWindow()
{
	delete m_window;
}

void MainWindow::startNewGame()
{
	for (uint16_t row = 0; row < m_row_size; row++)
	{
		for (uint16_t column = 0; column < m_column_size; column++)
		{
			m_blocks[row][column] = static_cast<uint8_t>(m_uniform_dist(m_rng)) % m_block_colors.size();
		}
	}
	bool found;
	for (;;)
	{
		found = false;
		uint16_t last_same_block_column;
		uint16_t last_same_block_row;
		uint16_t horizontal_count = 0;
		uint16_t vertical_count = 0;
		for (uint16_t row = 0; row < m_row_size; row++)
		{
			last_same_block_column = 0;
			horizontal_count = 0;
			for (uint16_t column = 1; column <= m_column_size; column++)
			{
				if (column < m_column_size && m_blocks[row][column] == m_blocks[row][last_same_block_column])
				{
					horizontal_count++;
				}
				else
				{
					horizontal_count++;
					if (horizontal_count >= m_block_needed)
					{
						for (uint16_t count = 0; count < horizontal_count; count++)
							m_blocks[row][last_same_block_column + count] = static_cast<uint8_t>(m_uniform_dist(m_rng)) % m_block_colors.size();
						found = true;
					}
					last_same_block_column = column;
					horizontal_count = 0;
				}
			}
		}
		for (uint16_t column = 0; column < m_column_size; column++)
		{
			last_same_block_row = 0;
			vertical_count = 0;
			for (uint16_t row = 1; row <= m_row_size; row++)
			{
				if (row < m_row_size && m_blocks[row][column] == m_blocks[last_same_block_row][column])
				{
					vertical_count++;
				}
				else
				{
					vertical_count++;
					if (vertical_count >= m_block_needed)
					{
						for (uint16_t count = 0; count < vertical_count; count++)
							m_blocks[last_same_block_row + count][column] = static_cast<uint8_t>(m_uniform_dist(m_rng)) % m_block_colors.size();
						found = true;
					}
					last_same_block_row = row;
					vertical_count = 0;
				}
			}
		}
		if (!found) break;
	}
}

bool MainWindow::removeBlocks()
{
	uint32_t score;
	bool found;
	bool return_val = false;
	for (;;)
	{
		score = 0;
		found = false;
		uint16_t last_same_block_column;
		uint16_t last_same_block_row;
		uint16_t horizontal_count = 0;
		uint16_t vertical_count = 0;
		for (uint16_t row = 0; row < m_row_size; row++)
		{
			last_same_block_column = 0;
			horizontal_count = 0;
			for (uint16_t column = 1; column <= m_column_size; column++)
			{
				if (column < m_column_size && m_blocks[row][column] == m_blocks[row][last_same_block_column])
				{
					horizontal_count++;
				}
				else
				{
					horizontal_count++;
					if (horizontal_count >= m_block_needed)
					{
						for (uint16_t count = 0; count < horizontal_count; count++)
						{
							m_blocks[row][last_same_block_column + count] = UINT8_MAX;
							score++;
						}
						found = true;
					}
					if (found) break;
					last_same_block_column = column;
					horizontal_count = 0;
				}
			}
			if (found) break;
		}
		for (uint16_t column = 0; column < m_column_size; column++)
		{
			last_same_block_row = 0;
			vertical_count = 0;
			for (uint16_t row = 1; row <= m_row_size; row++)
			{
				if (row < m_row_size && m_blocks[row][column] == m_blocks[last_same_block_row][column])
				{
					vertical_count++;
				}
				else
				{
					vertical_count++;
					if (vertical_count >= m_block_needed)
					{
						for (uint16_t count = 0; count < vertical_count; count++)
						{
							m_blocks[last_same_block_row + count][column] = UINT8_MAX;
							score++;
						}
						found = true;
					}
					if (found) break;
					last_same_block_row = row;
					vertical_count = 0;
				}
			}
			if (found) break;
		}
		if (!found) break;
		m_score += score * (score - 1);
		AnimationState state;
		state.m_duration1 = static_cast<uint32_t>(m_fps * 1.5f); /* 1.5 second */
		state.m_duration2 = m_fps; /* 1 second */
		state.m_score = score * (score - 1);
		state.m_x = -1;
		state.m_blocks_state.first = m_blocks;
		bool blank;
		do
		{
			blank = false;
			for (uint16_t row = 0; row < m_row_size; row++)
			{
				for (uint16_t column = 0; column < m_column_size; column++)
				{
					if (m_blocks[row][column] == UINT8_MAX)
					{
						if (!return_val) return_val = true;
						for (uint16_t shift = row; shift < m_row_size - 1; shift++)
						{
							m_blocks[shift][column] = m_blocks[shift + 1][column];
						}
						m_blocks[m_row_size - 1][column] = static_cast<uint8_t>(m_uniform_dist(m_rng)) % m_block_colors.size();
						blank = true;
						if (state.m_x == -1)
						{
							state.m_x = static_cast<float>(column * m_block_width);
							state.m_y = static_cast<float>(m_block_height * (m_row_size - 1) - row * m_block_height);
						}
					}
				}
			}
		} while (blank);
		state.m_blocks_state.second = m_blocks;
		m_anm_state.push(state);
	}
	while (found);
	return return_val;
}

void MainWindow::render()
{
	for (uint16_t row = 0; row < m_row_size; row++)
	{
		for (uint16_t column = 0; column < m_column_size; column++)
		{
			if (m_anm_state.size() &&
				m_anm_state.front().m_duration1 > 0 &&
				m_anm_state.front().m_blocks_state.first[row][column] == UINT8_MAX)
				continue;
			if (m_selected_column == column && m_selected_row == row) continue;
			sf::Sprite sprite;
			sprite.setTexture(m_block_texture);
			sprite.setScale(
				static_cast<float>(m_block_width) / BLOCK_IMAGE_WIDTH,
				static_cast<float>(m_block_height) / BLOCK_IMAGE_HEIGHT
				);
			if (m_anm_state.empty())
				sprite.setColor(m_block_colors[m_blocks[row][column]]);
			else if (m_anm_state.front().m_duration1 > 0)
				sprite.setColor(m_block_colors[m_anm_state.front().m_blocks_state.first[row][column]]);
			else
				sprite.setColor(m_block_colors[m_anm_state.front().m_blocks_state.second[row][column]]);
			sprite.setPosition(
				static_cast<float>(column * m_block_width),
				static_cast<float>(m_block_height * (m_row_size - 1) - m_block_height * row)
				);
			m_window->draw(sprite);
		}
	}
	if (m_anm_state.empty())
		m_anim_playing = false;
	else
	{
		m_anim_playing = true;
		if (m_anm_state.front().m_duration1 > 0)
		{
			m_anm_state.front().m_duration1--;
			sf::Text score_gained_text;
			score_gained_text.setFont(m_font);
			score_gained_text.setColor(sf::Color::White);
			score_gained_text.setCharacterSize(m_block_height);
			score_gained_text.setString("+" + std::to_string(m_anm_state.front().m_score));
			score_gained_text.setPosition(m_anm_state.front().m_x, m_anm_state.front().m_y - m_block_height / 10);
			m_window->draw(score_gained_text);
		}
		else
		{
			if (m_anm_state.front().m_duration2 > 0)
				m_anm_state.front().m_duration2--;
			else m_anm_state.pop();
		}
	}
	if (m_selected_column < m_column_size && m_selected_row < m_row_size)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_block_texture);
		sprite.setColor(
			sf::Color(
				m_block_colors[m_blocks[m_selected_row][m_selected_column]].r,
				m_block_colors[m_blocks[m_selected_row][m_selected_column]].g,
				m_block_colors[m_blocks[m_selected_row][m_selected_column]].b,
				191
				)
			);
		sprite.setScale(
			static_cast<float>(m_block_width) / BLOCK_IMAGE_WIDTH,
			static_cast<float>(m_block_height) / BLOCK_IMAGE_HEIGHT
			);
		sprite.setPosition(
			static_cast<float>(sf::Mouse::getPosition(*m_window).x - m_block_width / 2),
			static_cast<float>(sf::Mouse::getPosition(*m_window).y - m_block_height / 2)
			);
		m_window->draw(sprite);
	}
	sf::Text score_text;
	score_text.setFont(m_font);
	score_text.setColor(sf::Color::White);
	score_text.setCharacterSize(m_block_height);
	score_text.setString("Score: " + std::to_string(m_score));
	score_text.setPosition(0, static_cast<float>(m_block_height * m_row_size - m_block_height / 10));
	m_window->draw(score_text);
}

void MainWindow::handleEvents()
{
	sf::Event event;
	while (m_window->pollEvent(event))
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (!m_anim_playing)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					m_press_x = event.mouseButton.x;
					m_press_y = event.mouseButton.y;
					if (m_press_x <= m_block_width * m_column_size && m_press_y <= m_block_height * m_row_size)
					{
						m_selected_column = static_cast<uint16_t>(m_press_x / m_block_width);
						m_selected_row = static_cast<uint16_t>((m_block_height * m_row_size - m_press_y) / m_block_height);
					}
				}
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			m_selected_column = UINT16_MAX;
			m_selected_row = UINT16_MAX;
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			if (!m_anim_playing)
			{
				if (m_selected_column != UINT16_MAX && m_selected_row != UINT16_MAX)
				{
					m_release_x = event.mouseMove.x;
					m_release_y = event.mouseMove.y;
					int32_t delta_x = m_press_x - m_release_x;
					int32_t delta_y = m_press_y - m_release_y;
					if (abs(delta_x) > m_block_height || abs(delta_y) > m_block_width)
					{
						Direction direction = Direction::None;
						/* Right */
						if (abs(delta_x) > abs(delta_y) && delta_x < 0)
						{
							direction = Direction::Right;
						}
						/* Up */
						else if (abs(delta_y) > abs(delta_x) && delta_y > 0)
						{
							direction = Direction::Up;
						}
						/* Left */
						else if (abs(delta_x) > abs(delta_y) && delta_x > 0)
						{
							direction = Direction::Left;
						}
						/* Down */
						else if (abs(delta_y) > abs(delta_x) && delta_y < 0)
						{
							direction = Direction::Down;
						}
						if (direction != Direction::None &&
							!(m_selected_row <= 0 && direction == Direction::Down) &&
							!(m_selected_column <= 0 && direction == Direction::Left) &&
							!(m_selected_row >= m_row_size && direction == Direction::Up) &&
							!(m_selected_column >= m_column_size && direction == Direction::Right))
						{
							uint16_t switched_column = m_selected_column;
							uint16_t switched_row = m_selected_row;
							switch (direction)
							{
							case Direction::Right:
								switched_column++;
								break;
							case Direction::Up:
								switched_row++;
								break;
							case Direction::Left:
								switched_column--;
								break;
							case Direction::Down:
								switched_row--;
							}
							if (switched_column < m_column_size && switched_row < m_row_size)
							{
								std::vector<std::vector<uint8_t>> temp_blocks(m_blocks);
								m_blocks[switched_row][switched_column] = temp_blocks[m_selected_row][m_selected_column];
								m_blocks[m_selected_row][m_selected_column] = temp_blocks[switched_row][switched_column];
								if (!removeBlocks()) m_blocks = temp_blocks;
							}
						}
						m_selected_column = UINT16_MAX;
						m_selected_row = UINT16_MAX;
					}
				}
			}
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				m_window->close();
			}
		}
		else if (event.type == sf::Event::Closed)
		{
			m_window->close();
		}
	}
}

void MainWindow::readConfiguration()
{
	boost::property_tree::ptree pt;
	read_xml("config.xml", pt, boost::property_tree::xml_parser::no_comments);
	m_column_size = pt.get<uint16_t>("samegame.column_size");
	m_row_size = pt.get<uint16_t>("samegame.row_size");
	m_block_width = pt.get<uint16_t>("samegame.block_width");
	m_block_height = pt.get<uint16_t>("samegame.block_height");
	m_block_needed = pt.get<uint8_t>("samegame.block_needed");
	for (boost::property_tree::ptree::value_type &value : pt.get_child("samegame.block_colors"))
	{
		m_block_colors.push_back(
			sf::Color(
			static_cast<uint8_t>(std::strtoul(value.second.data().substr(1, 2).c_str(), nullptr, 16)),
			static_cast<uint8_t>(std::strtoul(value.second.data().substr(3, 2).c_str(), nullptr, 16)),
			static_cast<uint8_t>(std::strtoul(value.second.data().substr(5, 2).c_str(), nullptr, 16))
			)
			);
	}
}

void MainWindow::writeDefaultConfiguration()
{
	boost::property_tree::ptree pt;
	pt.clear();
	m_block_colors.clear();
	pt.put("samegame.column_size", 10);
	pt.put("samegame.row_size", 10);
	pt.put("samegame.block_width", 32);
	pt.put("samegame.block_height", 32);
	pt.put("samegame.block_needed", 3);
	pt.add("samegame.block_colors.color", "#FF0000");
	pt.add("samegame.block_colors.color", "#FFFF00");
	pt.add("samegame.block_colors.color", "#00FF00");
	pt.add("samegame.block_colors.color", "#00FFFF");
	pt.add("samegame.block_colors.color", "#0000FF");
	try
	{
		boost::property_tree::xml_writer_settings<char> settings('\t', 1);
		write_xml("config.xml", pt, std::locale(), settings);
	}
	catch (...)
	{
	}
}

void MainWindow::checkConfiguration()
{
	if (m_column_size < 2 || m_column_size > 128) m_column_size = 10;
	if (m_row_size < 2 || m_row_size > 70) m_row_size = 10;
	if (m_block_width < 2 || m_block_width > 640) m_block_width = 32;
	if (m_block_height < 2 || m_block_height > 640) m_block_height = 32;
	if (m_block_colors.size() < 2)
	{
		m_block_colors.push_back(sf::Color(255, 0, 0));
		m_block_colors.push_back(sf::Color(255, 255, 0));
		m_block_colors.push_back(sf::Color(0, 255, 0));
		m_block_colors.push_back(sf::Color(0, 255, 255));
		m_block_colors.push_back(sf::Color(0, 0, 255));
	}
	if (m_block_needed < 2 || m_block_needed > 128) m_block_needed = 3;
}
} // SameGame