#pragma once

#include <string>
#include <iostream>
#include "Scene.hpp"

struct Ghost {
  Ghost();

  Ghost(const std::string &name, Scene::Drawable *a_drawable, Scene::Transform *player_, Scene::Transform *camera_);

  std::string ghose_name;
  Scene::Drawable *drawable;
  Scene::Transform* player;
  Scene::Transform* camera;
  
  glm::vec3 getPosition(float e);

  void set_invisible();
  glm::vec3 last_known = glm::vec3(0.0f, 0.0f, 0.0f);
  float distance(bool use_last_known = false);

  bool active = false;
  bool valid_last_known = false;	// If the last known position is valid

  bool close_to_player = false;	// If the ghost is close to player
  bool got_player = false;	// If the ghost got the player

  // parameter of the ghost
  float speed = 1.5f;	// same as player
  uint8_t stare_at_player = 0;	// 0: not staring, 1: staring in 2d, 2: staring in 3d
  float dis_thresh = 4.0f;	// distance threshold to player
  static constexpr float INTERACT_ANGLE = 2.2f;	// angle to interact with player
  float speed_ratio = 2.f;		// used my MEMORY state, speed ratio to player, not using speed
  uint8_t player_stride = (uint8_t)(10 / (1 + speed_ratio));
  uint8_t ghost_stride = (uint8_t)(speed_ratio * player_stride);
  float ghost_z_offset = 1.0f; 	// ghost z = player z - ghost_z_offset 
};
