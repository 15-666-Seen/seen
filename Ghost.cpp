#include "Ghost.hpp"
#include "Scene.hpp"

#include <deque>

Ghost::Ghost() {}

Ghost::Ghost(const std::string &name, Scene::Drawable *a_drawable,
             Scene::Transform *player_, Scene::Transform *camera_) {
  ghose_name = name;
  this->drawable = a_drawable;
  player = player_;
  camera = camera_;
}

glm::vec3 Ghost::update(float e) {

  if (!active) {
    return drawable->transform->position;
  }

  float distance_can_chase = GhostSpeed * e;
  while (!path_points.empty()) {
    auto next = path_points.front();
    float distance = glm::distance(next, drawable->transform->position);
    glm::vec3 dir = glm::normalize(next - drawable->transform->position);

    // using path_points to move ghost
    if (distance < distance_can_chase) {
      drawable->transform->position = next;
      path_points.pop_front();
      distance_can_chase -= distance;
      continue;
    }
    drawable->transform->position += dir * distance_can_chase;
    break;
  }

  if (path_points.empty() ||
      glm::distance(player->position, drawable->transform->position) <
          dis_thresh) {
    got_player = true;
  }

  // make ghost face player
  glm::vec3 direction = player->position - drawable->transform->position;
  float angle = atan2(direction.y, direction.x);
  drawable->transform->rotation =
      glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.f));
  return drawable->transform->position;
}

void Ghost::set_invisible() { drawable->visible = false; }

float Ghost::distance(bool use_last_known) {
  glm::vec3 temp;

  if (use_last_known) {
    temp = last_known;
    temp.z -= ghost_z_offset; // transform to ghost's z
  } else {
    temp = player->position;
    temp.z -= ghost_z_offset; // transform to ghost's z
  }
  return glm::distance(temp, drawable->transform->position);
}