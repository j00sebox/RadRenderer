#include "model.hpp"
#include "pch.h"
#include "renderer.hpp"
#include "vector.hpp"

#include "../mathz/misc.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

int main()
{
  const int width = 1280, height = 720;
  const float near = 0.1f, far = 1000.f;

  Model model("../assets/objs/teapot.obj");
  Camera camera(width, height, near, far, 60.f);

  Renderer renderer(width, height, near, far);
  sf::RenderWindow window;
  sf::Sprite sprite;
  sf::Texture texture;
  sf::Clock clock;

  window.create(sf::VideoMode(width, height), "Rad Renderer", sf::Style::Default);
  texture.create(width, height);
  sprite.setTexture(texture);
  sf::Vector2i window_center(width / 2, height / 2);

  model.SetPosition(0.f, -3.f, 10.f);
  model.SetScale(1.2f);

  bool mouse_down = false;
  int prev_x, prev_y;
  float pitch = 0.f, yaw = 0.f;
  float forward = 0.f;
  float right = 0.f;
  float rotation_speed = 0.005f;
  float cam_movement = 0.f;
  float cam_x_movement = 0.f;

  while (window.isOpen())
  {
    float dx = 0.f;
    float dy = 0.f;
    float elapsed_time = clock.restart().asMilliseconds();

    // Handle forward movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
      forward = -1.f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
      forward = 1.f;
    }
    else
    {
      forward = 0.f;
    }

    // Handle side movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
      right = -1.f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
      right = 1.f;
    }
    else
    {
      right = 0.f;
    }

    // Events
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed ||
          (event.type == sf::Event::KeyPressed &&
           event.key.code == sf::Keyboard::Escape))
        window.close();

      if (event.type == sf::Event::MouseButtonPressed)
      {
        mouse_down = true;
        sf::Mouse::setPosition(window_center, window); // Lock cursor
      }

      if (event.type == sf::Event::MouseButtonReleased)
        mouse_down = false;
    }

    // Update camera
    if (mouse_down)
    {
      sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
      sf::Vector2i delta = mouse_pos - window_center;

      if (delta.x != 0 || delta.y != 0)
      {
        yaw += delta.x * rotation_speed * elapsed_time;
        pitch += delta.y * rotation_speed * elapsed_time;

        camera.Rotate(pitch, yaw);
      }

      sf::Mouse::setPosition(window_center, window); // Reset cursor to center

      cam_movement += forward * elapsed_time * 0.01f;
      cam_x_movement += right * elapsed_time * 0.01f;
      camera.Move((camera.GetForward() * cam_movement) + (camera.GetRight() * cam_x_movement));
    }

    renderer.Render(model, camera);

    texture.update(renderer.GetFrameBuffer());
    window.draw(sprite);
    window.display();
    window.clear();
  }
  return 0;
}
