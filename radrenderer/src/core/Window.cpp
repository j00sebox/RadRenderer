#include "pch.h"

#include "Window.h"

#include "RendererSettings.h"

Window* Window::m_instance = nullptr;

Window::Window(unsigned int width, unsigned int height)
    : m_width(width), m_height(height), m_renderer(width, height)
{
    if (m_instance)
#ifdef PLATFORM_WINDOWS
        __debugbreak();
#elif PLATFORM_LINUX
        raise(SIGTRAP);
#endif

    m_instance = this;

    m_window.create(sf::VideoMode(width, height), "Rad Renderer", sf::Style::Default);

    m_texture.create(width, height);

    m_sprite.setTexture(m_texture);

    m_window.setVerticalSyncEnabled(true);

    if (!m_font.loadFromFile("res/fonts/arial_narrow_7.ttf"))
    {
#ifdef PLATFORM_WINDOWS
          __debugbreak();
#elif PLATFORM_LINUX
        raise(SIGTRAP);
#endif
    }

    m_fps = sf::Text("0", m_font, 25);
    m_fps.setFillColor(sf::Color::Red);

}

Window::~Window() {}

void Window::loop()
{
    bool mouse_down = false;
    int prev_x, prev_y;

    float forward = 0.f;

    while (m_window.isOpen())
    {
		float dx = 0.f;
		float dy = 0.f;

        // events
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                m_window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
            {
                forward = -1.f;
            }

            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::W)
            {
                forward = 0.f;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
            {
                forward = 1.f;
            }

            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::S)
            {
                forward = 0.f;
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                mouse_down = true;

				prev_x = event.mouseButton.x;
				prev_y = event.mouseButton.y;
            }

            if (event.type == sf::Event::MouseButtonReleased)
                mouse_down = false;

            if (event.type == sf::Event::MouseMoved)
            {
                if (mouse_down)
                {
                    int x = event.mouseMove.x;
                    int y = event.mouseMove.y;

					dx = (float)(x - prev_x);
					dy = (float)(y - prev_y);
                }
            }

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
			{
                m_renderer.reset_object();
			}
        }

        sf::Time elapsed = m_clock.restart();

        std::uint8_t* pixels = reinterpret_cast<std::uint8_t*>(m_renderer.update((float)elapsed.asMilliseconds(), forward, -dx, dy));

        float fps = 1.f / elapsed.asSeconds();

        m_fps.setString(std::to_string(fps) + " FPS");

        m_texture.update(pixels);

        m_window.draw(m_sprite);
        m_window.draw(m_fps);
        m_window.display();
        m_window.clear();
    }

}
