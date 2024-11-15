#pragma once

#include <string>

#include "Scene.hpp"

struct Ghost {
  Ghost();

  Ghost(const std::string &name, Scene::Drawable *a_drawable);

  std::string ghose_name;
  Scene::Drawable *drawable;

  glm::vec3 getPosition() { return drawable->transform->position; }

  bool active = false;
};
