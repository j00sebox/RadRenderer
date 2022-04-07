#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class Window {
public:
	Window(unsigned int width, unsigned int height);
	~Window();

	void loop();

	static inline Window* get() { return m_instance; }
	
	static Window* m_instance;

private:
	sf::RenderWindow m_window;
	std::unique_ptr<sf::Uint8[]> m_frame_buffer;
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	
	unsigned int m_width, m_height;
};