#include "Ghost.hpp"

Ghost::Ghost() {}

Ghost::Ghost(const std::string &name, Scene::Drawable *a_drawable) {
  ghose_name = name;
  this->drawable = a_drawable;
}
