#include "Ghost.hpp"

Ghost::Ghost() {}

Ghost::Ghost(const std::string &name, Scene::Drawable *a_drawable, Scene::Transform* player_, Scene::Transform* camera_) {
  ghose_name = name;
  this->drawable = a_drawable;
  player = player_;
  camera = camera_;
}

glm::vec3 Ghost::getPosition() {
	if (!active) return drawable->transform->position;


}

void Ghost::set_invisible() {
	drawable->visible = false;
}