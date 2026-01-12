#include "model.hpp"
#include "pch.h"
#include "renderer.hpp"
#include "vector.hpp"
#include "file.hpp"

#include "../mathz/misc.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <signal.h>

int main()
{
  const int width = 1280, height = 720;
  const float _near = 0.1f, _far = 1000.f;

  Model model("../assets/objs/avocado/Avocado.gltf");
  Camera camera(width, height, _near, _far, 60.f);

  Renderer renderer(width, height, _near, _far);
  sf::RenderWindow window;
  
  sf::Texture texture(sf::Vector2u( width, height ));
  sf::Clock clock;

  sf::Font font;
  if (!font.openFromFile("../assets/fonts/arial_narrow_7.ttf"))
#ifdef _MSC_VER
      __debugbreak();
#else
      raise(SIGTRAP);
#endif

  sf::Text fps_text = sf::Text(font, "0", 25);
  fps_text.setFillColor(sf::Color::Red);

  window.create(sf::VideoMode(sf::Vector2u(width, height)), "Rad Renderer", sf::Style::Default);

  sf::Sprite sprite(texture);
  sf::Vector2i window_center(width / 2, height / 2);

  model.setPosition(0.f, 0.f, 1.f);
  model.setScale(3.f);

  bool mouse_down = false;
  int prev_x, prev_y;
  float pitch = 0.f, yaw = 0.f;
  float forward = 0.f;
  float right = 0.f;
  float rotation_speed = 0.005f;
  float cam_movement = 0.f;
  float cam_x_movement = 0.f;

  // FPS timing stuff
  float fps_timer = 0.0f;
  int frame_count = 0;
  float fps_update_interval = 1.0f; // Seconds
  float current_fps = 0.0f;

  while (window.isOpen())
  {
    float dx = 0.f;
    float dy = 0.f;
    sf::Time elapsed_time = clock.restart();

    // Handle forward movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
      forward = -1.f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
      forward = 1.f;
    }
    else
    {
      forward = 0.f;
    }

    // Handle side movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
      right = -1.f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
      right = 1.f;
    }
    else
    {
      right = 0.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
    {
        model = Model(OpenFileDialog().c_str());
    }

    // Events
    while (auto event_optional = window.pollEvent())
    {
        const sf::Event& event = *event_optional;

      if (event.is<sf::Event::Closed>())
        window.close();
      else if (auto keyEv = event.getIf<sf::Event::KeyPressed>()) 
      {
          if (keyEv->scancode == sf::Keyboard::Scancode::Escape) 
              window.close();
      }
      else if (event.is<sf::Event::MouseButtonPressed>())
      {
        mouse_down = true;
        sf::Mouse::setPosition(window_center, window); // Lock cursor
      }
      else if (event.is<sf::Event::MouseButtonReleased>())
        mouse_down = false;
    }

    // Update camera
    if (mouse_down)
    {
      sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
      sf::Vector2i delta = mouse_pos - window_center;

      if (delta.x != 0 || delta.y != 0)
      {
        yaw += delta.x * rotation_speed * elapsed_time.asMilliseconds();
        pitch += delta.y * rotation_speed * elapsed_time.asMilliseconds();

        camera.rotate(pitch, yaw);
      }

      sf::Mouse::setPosition(window_center, window); // Reset cursor to center

      cam_movement += forward * elapsed_time.asMilliseconds() * 0.01f;
      cam_x_movement += right * elapsed_time.asMilliseconds() * 0.01f;
      camera.move((camera.getForward() * cam_movement) + (camera.getRight() * cam_x_movement));
    }

    renderer.render(model, camera);

    fps_timer += elapsed_time.asSeconds();
    ++frame_count;

    if (fps_timer >= fps_update_interval) 
    {
        current_fps = frame_count / fps_timer;
        fps_timer = 0.0f;
        frame_count = 0;

        fps_text.setString("FPS: " + std::to_string(static_cast<int>(current_fps)));
    }

    texture.update(renderer.getFrameBuffer());
    window.draw(sprite);
    window.draw(fps_text);
    window.display();
    window.clear();
  }
  return 0;
}
