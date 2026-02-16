#include "model.hpp"
#include "pch.h"
#include "renderer.hpp"
#include "vector.hpp"
#include "file.hpp"
#include "embedded_font.hpp"

#include "../mathz/misc.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

int main()
{
  const int width = 1280, height = 720;
  const float near = 0.1f, far = 1000.f;

  Model model("../assets/objs/avocado/Avocado.gltf");
  Camera camera(width, height, near, far, 60.f);

  Renderer renderer(width, height, near, far);
  sf::RenderWindow window;
  
  sf::Texture texture(sf::Vector2u( width, height ));
  sf::Clock clock;

  sf::Font font;
  if(!font.openFromMemory(silkscreen_font, silkscreen_font_len))
    return 0;

  const unsigned int font_size = 18;

  sf::Text fps_text = sf::Text(font, "0", font_size);
  fps_text.setFillColor(sf::Color::White);

  sf::Text frametime_text = sf::Text(font, "0", font_size);
  frametime_text.setFillColor(sf::Color::White);
  frametime_text.setPosition({0.f, 20.f});

  sf::Text model_tri_text = sf::Text(font, "0", font_size);
  model_tri_text.setFillColor(sf::Color::White);
  model_tri_text.setPosition({0.f, 40.f});

  sf::Text rendered_tri_text = sf::Text(font, "0", font_size);
  rendered_tri_text.setFillColor(sf::Color::White);
  rendered_tri_text.setPosition({0.f, 60.f});

  sf::RectangleShape stats_background(sf::Vector2f(320.f, 85.f));
  stats_background.setFillColor(sf::Color(50, 50, 50, 180));

  window.create(sf::VideoMode(sf::Vector2u(width, height)), "Rad Renderer", sf::Style::Default);

  sf::Sprite sprite(texture);
  sf::Vector2i window_center(width / 2, height / 2);

  model.setPosition(0.f, 0.f, 1.f);
  model.setScale(3.f);

  bool mouse_down = false;
  float pitch = 0.f, yaw = 0.f;
  float forward = 0.f;
  float right = 0.f;
  float rotation_speed = 0.005f;
  float move_speed = 0.01f;

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
        window.setMouseCursorGrabbed(true);
        window.setMouseCursorVisible(false);
        sf::Mouse::setPosition(window_center, window);
      }
      else if (event.is<sf::Event::MouseButtonReleased>())
      {
        mouse_down = false;
        window.setMouseCursorGrabbed(false);
        window.setMouseCursorVisible(true);
      }
    }

    // Update camera
    if (mouse_down)
    {
      sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
      sf::Vector2i delta = mouse_pos - window_center;

      if (delta.x != 0 || delta.y != 0)
      {
        yaw += delta.x * rotation_speed * elapsed_time.asMilliseconds();
        pitch -= delta.y * rotation_speed * elapsed_time.asMilliseconds();

        // Clamp pitch
        if (pitch > 89.f) pitch = 89.f;
        if (pitch < -89.f) pitch = -89.f;

        camera.rotate(pitch, yaw);
      }

      sf::Mouse::setPosition(window_center, window); // Reset cursor to center

      float delta_forward = forward * elapsed_time.asMilliseconds() * move_speed;
      float delta_right = right * elapsed_time.asMilliseconds() * move_speed;

      if (delta_forward != 0.f || delta_right != 0.f)
      {
        mathz::Vec3 new_pos = camera.getPosition()
            + (camera.getForward() * delta_forward)
            + (camera.getRight() * delta_right);
        camera.move(new_pos);
      }
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
        frametime_text.setString("Frame: " + std::to_string(elapsed_time.asMilliseconds()) + " ms");
        model_tri_text.setString("Model triangles: " + std::to_string(model.size()));
        rendered_tri_text.setString("Rendered triangles: " + std::to_string(renderer.getStats().triangles_rendered));
    }

    texture.update(renderer.getFrameBuffer());
    window.draw(sprite);
    window.draw(stats_background);
    window.draw(fps_text);
    window.draw(frametime_text);
    window.draw(model_tri_text);
    window.draw(rendered_tri_text);
    window.display();
    window.clear();
  }
  return 0;
}
