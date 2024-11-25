#pragma once

#include <string>

#include "Scene.hpp"

struct Ghost {
  Ghost();

  Ghost(const std::string &name, Scene::Drawable *a_drawable, Scene::Transform *player_, Scene::Transform *camera_);

  std::string ghose_name;
  Scene::Drawable *drawable;
  Scene::Transform* player;
  Scene::Transform* camera
  
  glm::vec3 getPosition();

  void set_invisible();

  bool active = false;

  // parameter of the ghost
  float speed = 1.0f;	// same as player
  uint8_t stare_at_player = 0;	// 0: not staring, 1: staring in 2d, 2: staring in 3d

};
