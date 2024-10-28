#include "Interactable.hpp"
#include "Scene.hpp"
#include <glm/glm.hpp>

#include <iostream>

bool iteractCheck(Scene::Transform *player_transform, glm::vec3 object_pos,
                  Scene::Camera *camera, float distance_threshold) {
  glm::vec3 player_eye_pos =
      player_transform->position + glm::vec3(0.0f, 0.0f, PLAYER_HEIGHT);

  // check if player is close enough to interact with item
  float distance_xy =
      glm::distance(glm::vec2(player_eye_pos.x, player_eye_pos.y),
                    glm::vec2(object_pos.x, object_pos.y));

  // player's looking at the furniture
  glm::vec3 player_look_dir = camera->transform->make_local_to_world()[2];
  glm::vec3 fur_to_player = object_pos - player_eye_pos;
  // if there angle is small enough, it is interactable
  float angle = glm::acos(
      glm::dot(glm::normalize(player_look_dir), glm::normalize(fur_to_player)));

  // final result
  return distance_xy < distance_threshold && angle >= INTERACT_ANGLE;
}

/* ITEM */
Item::Item() {}

// TODO: different type of items should have different interact text
bool Item::interact(float elapsed) {
  if (type == BEDROOM_KEY) {
    std::cout << "You picked up a bedroom key!" << std::endl;
    visible = false;
    phase_allow_interact = false;
    return true;
  }
  return true;
}

bool Item::interactable(Scene::Transform *player_transform,
                        Scene::Camera *camera) {
  // should be visible and phase_allow_interact
  if (!visible || !phase_allow_interact) {
    return false;
  }

  return iteractCheck(player_transform, transform->position, camera,
                      ITEM_INTERACT_DISTANCE);
}

/* FURNITURE */
Furniture::Furniture() {}

bool Furniture::interact(float elapsed) { return true; }

bool Furniture::interactable(Scene::Transform *player_transform,
                             Scene::Camera *camera) {
  // should be phase_allow_interact
  if (!phase_allow_interact) {
    return false;
  }

  return iteractCheck(player_transform, getCenterPos(), camera,
                      FURNITURE_INTERACT_DISTANCE);
}

/* DOOR */
bool Door::interact(float elapsed) {
  animation_time += elapsed;
  std::cout << "======================" << std::endl;
  std::cout << animation_time << std::endl;
  if (animation_time > 0.5f) {
    interact_status = false;
  }
  // rotate the door along z axis
  transform->rotation =
      glm::angleAxis(glm::radians(90.0f) * (animation_time / 0.5f),
                     glm::vec3(0.0f, 0.0f, 1.0f));
  return true;
}