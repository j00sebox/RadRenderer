#include "Window.h"

Window* Window::m_instance = nullptr;

Window::Window()
{
    m_window.create(sf::VideoMode(264, 250), "My window", sf::Style::Default);

    m_texture.create(264, 250);

    m_sprite.setTexture(m_texture);

    m_frame_buffer.reset(new sf::Uint8[264 * 250 * 4]);

    for (int i = 0; i < 264 * 250 * 4; i += 4)
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