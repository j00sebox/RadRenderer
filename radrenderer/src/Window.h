#pragma once

#include "RadRenderer.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Window {
public:
	Window(unsigned int width, unsigned int height);
	~Window();

	void loop();

	static inline Window* get() { return m_instance; }
	inline unsigned int width() const { return m_width; }
	inline unsigned int height() const { return m_height; }
	
	static Window* m_instance;

private:
	RadRenderer m_renderer;
	sf::RenderWindow m_window;
	sf::Text m_fps;
	sf::Font m_font;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Clock m_clock;
	
	unsigned int m_width, m_height;
};