#include "model.hpp"
#include "pch.h"
#include "renderer.hpp"
#include "vector.hpp"

#include <SFML/Graphics.hpp>

int main()
{
  const int width = 1280, height = 720;
  const float near = 0.1f, far = 1000.f;

  Model model("../assets/objs/teapot.obj");
  Camera camera({width, height, near, far, 60.f});

  Renderer renderer(width, height, near, far);
  sf::RenderWindow window;
  sf::Sprite sprite;
  sf::Texture texture;
  sf::Clock clock;

  window.create(sf::VideoMode(width, height), "Rad Renderer", sf::Style::Default);
  texture.create(width, height);
  sprite.setTexture(texture);

  model.SetPosition(0.f, -3.f, 10.f);
  model.SetScale(1.2f);

  bool mouse_down = false;
  int prev_x, prev_y;
  float forward = 0.f;
  float right = 0.f;
  float rotation_speed = 0.001f;
  float cam_movement = 0.f;
  float cam_x_movement = 0.f;

  while (window.isOpen())
  {
    float dx = 0.f;
    float dy = 0.f;

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
    }

    float elapsed_time = clock.restart().asMilliseconds();

    // Update camera
    cam_movement += forward * elapsed_time * 0.01f;
    cam_x_movement += right * elapsed_time * 0.01f;
    camera.Move((camera.GetForward() * cam_movement) + (camera.GetRight() * cam_x_movement));

    if (dx != 0.f || dy != 0.f)
    {
      float pitch = dy * rotation_speed * elapsed_time;
      float yaw = dx * rotation_speed * elapsed_time;

      camera.Rotate(pitch, yaw);
    }

    renderer.Render(model, camera);

    texture.update(renderer.GetFrameBuffer());
    window.draw(sprite);
    window.display();
    window.clear();
  }
  return 0;
}
