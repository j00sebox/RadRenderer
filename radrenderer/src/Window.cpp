#include "Window.h"

Window* Window::m_instance = nullptr;

Window::Window(unsigned int width, unsigned int height)
    : m_width(width), m_height(height)
{
    if (m_instance)
        __debugbreak();

    m_instance = this;

    m_window.create(sf::VideoMode(width, height), "My window", sf::Style::Default);

    m_texture.create(width, height);

    m_sprite.setTexture(m_texture);

    m_frame_buffer.reset(new sf::Uint8[width * height * 4]);

    for (int i = 0; i < width * height * 4; i += 4)
    {
        m_frame_buffer[i] = 255;
        m_frame_buffer[i + 1] = 0;
        m_frame_buffer[i + 2] = 0;
        m_frame_buffer[i + 3] = 1;
    }
}

Window::~Window()
{

}

void Window::loop()
{
    while (m_window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                m_window.close();
        }

        m_texture.update(m_frame_buffer.get());

        m_window.draw(m_sprite);
        m_window.display();
    }

}