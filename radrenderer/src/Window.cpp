#include "Window.h"

#include <iostream>

Window* Window::m_instance = nullptr;

Window::Window(unsigned int width, unsigned int height)
    : m_width(width), m_height(height), m_renderer(width, height, width*height)
{
    if (m_instance)
        __debugbreak();

    m_instance = this;

    m_window.create(sf::VideoMode(width, height), "Rad Renderer", sf::Style::Default);

    m_texture.create(width, height);

    m_sprite.setTexture(m_texture);
}

Window::~Window()
{
    
}

void Window::loop()
{
    while (m_window.isOpen())
    {
        // events
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        sf::Time elapsed = m_clock.restart();

        std::uint8_t* pixels = reinterpret_cast<std::uint8_t*>(m_renderer.update(elapsed.asMilliseconds()));

        m_texture.update(pixels);

        m_window.draw(m_sprite);
        m_window.display();
        m_window.clear();
    }

}