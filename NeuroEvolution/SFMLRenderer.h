#pragma once

#include <SFML/Graphics.hpp>

class Game;

class SFMLRenderer
{
public:
	SFMLRenderer(int resX, int resY, const char* title);

	void Draw(const Game&);
	void Zoom(const float amount);
	void Hide();
	void Show();

	std::vector<sf::Event> GetEvents();

private:
	int m_x, m_y;
	sf::RenderWindow m_window;
	sf::View m_view;

};
