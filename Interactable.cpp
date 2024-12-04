#include "Interactable.hpp"

#include <glm/glm.hpp>
#include <iostream>

#include "Scene.hpp"
#include "UI.hpp"
#include "sound_prep.hpp"

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
  // if there angle is small enough, it is interact-able
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
    visible = false;
    this->drawable->visible = false;
  } else if (type == DEN_KEY) {
    visible = false;
    this->drawable->visible = false;
  } else if (type == FILE1) {
    // interact_status = 1;
    return true;
  } else if (type == CLIP_L) {
    visible = false;
    this->drawable->visible = false;
    return true;
  } else if (type == CLIP_R) {
    visible = false;
    this->drawable->visible = false;
    return true;
  } else if (type == EYEBALL) {
    visible = false;
    this->drawable->visible = false;
    interact_sound = Sound::play_3D(*eyeball_sample, 2.0f,
                                    drawable->transform->position, 10.0f);
  } else if (type == REDROOM_KEY) {
    visible = false;
    this->drawable->visible = false;
    interact_sound = Sound::play_3D(*breath_trigger_sample, 2.0f,
                                    drawable->transform->position, 10.0f);
  }
  return true;
}

bool Item::interactable(Scene::Transform *player_transform,
                        Scene::Camera *camera) {
  // should be visible and phase_allow_interact
  if (!visible || !phase_allow_interact) {
    return false;
  }

  return iteractCheck(player_transform, drawable->transform->position, camera,
                      ITEM_INTERACT_DISTANCE);
}

/* FURNITURE */
Furniture::Furniture() {}

bool Furniture::interact(float elapsed) {
  if (type == BED) {
    interact_status = 1;
  } 
  else if (type == CLOSET2) {
      interact_status = 1;
  }
  else if (type == TINY_SCULPTURE_TENTACLES) {
      if (anim_time > 1.f) {
          interact_status = 2;
          return true;
      }
      anim_time += elapsed;
      drawable->transform->scale = glm::vec3(anim_time);
  }


  return true;
}

bool Furniture::interactable(Scene::Transform *player_transform,
                             Scene::Camera *camera) {
  // should be phase_allow_interact
  if (!phase_allow_interact) {
    return false;
  }

  if (type == BED) {
    if (interact_status == 0) {
      return iteractCheck(player_transform, getCenterPos(), camera,
                          FURNITURE_INTERACT_DISTANCE);
    } else {
      return true;
    }
  }

  else if (type == CLOSET2) {
    if (interact_status == 0) {
      return iteractCheck(player_transform, getCenterPos(), camera,
                          FURNITURE_INTERACT_DISTANCE);
    } else {
      return true;
    }
  }

  return iteractCheck(player_transform, getCenterPos(), camera,
                      FURNITURE_INTERACT_DISTANCE);
}

/* DOOR */
bool Door::interact(float elapsed) {
  if (animation_time == 0.0f) {
    interact_sound = Sound::play_3D(*door_open_sample, 2.0f,
                                    drawable->transform->position, 10.0f);
  }
  animation_time += elapsed;
  if (animation_time > 0.6f) {
    state = Door::DoorState::OPEN;
    interact_status = 1;
    return false;
  }
  // rotate the door along z axis
  drawable->transform->rotation =
      glm::angleAxis(glm::radians(-90.0f) * (animation_time / 0.6f),
                     glm::vec3(0.0f, 0.0f, 1.0f));
  return true;
}