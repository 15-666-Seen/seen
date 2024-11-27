#include "Ghost.hpp"

#include <deque>

Ghost::Ghost() {}

Ghost::Ghost(const std::string &name, Scene::Drawable *a_drawable, Scene::Transform* player_, Scene::Transform* camera_) {
  ghose_name = name;
  this->drawable = a_drawable;
  player = player_;
  camera = camera_;
  ghost_z_offset = player->position.z - drawable->transform->position.z;
}

glm::vec3 Ghost::getPosition(float e) {
	if (!active) return drawable->transform->position;

		static bool straight = true;
		static uint16_t size = 1280, index = 0, ghost_index = 0;
		//static std::vector<glm::vec3> path(size);
		static std::deque< glm::vec3 > path;

		static uint8_t player_count = 0;

		// if ghost is closed to player, and/or if the player is looking at the ghost
		if (Ghost::distance() < dis_thresh) {
			std::cout << "\rWatching YOU!!";
			close_to_player = true;
			glm::vec3 player_look_dir = camera->make_local_to_world()[2];
			glm::vec3 fur_to_player = drawable->transform->position - player->position;
			float look_angle = glm::acos(
				glm::dot(glm::normalize(player_look_dir), glm::normalize(fur_to_player)));
			if (camera != nullptr && look_angle >= INTERACT_ANGLE) {
				active = false;
				valid_last_known = false;
				std::cout << "GOT YOU" << std::endl;
				got_player = true;
			}
			return drawable->transform->position;
		}
		else {
			close_to_player = false;
		}

		//update player path
		path.push_back(player->position);
		if (straight) {			// Move towards player in straight line to last known position
			// set up target for ghost to go to
			if (!valid_last_known) {
				last_known = player->position;
				valid_last_known = true;
			}
			// always face player
			glm::vec3 direction = last_known - drawable->transform->position;
			float angle = atan2(direction.y, direction.x);
			drawable->transform->rotation = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.f));

			// if ghost is close enough to last known position, use player's path
			if (Ghost::distance(true) < dis_thresh) {
				straight = false;
			}
			drawable->transform->position += glm::vec3(speed * cos(angle), speed * sin(angle), 0.0f) * e;
		}
		else {					// Move towards player in player's path
			// always face player
			glm::vec3 direction = player->position - drawable->transform->position;
			float angle = atan2(direction.y, direction.x);
			drawable->transform->rotation = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.f));
			if (player_count < player_stride) {
				player_count++;
				path.pop_front();
				path.pop_front();
			}
			else {
				player_count = 0;
				ghost_index = (ghost_index + ghost_stride) % size;
				drawable->transform->position = path[0];
				path.pop_front();
				drawable->transform->position.z = player->position.z - ghost_z_offset;
			}
		}
		index = (index + 1) % size;
	return drawable->transform->position;
}

void Ghost::set_invisible() {
	drawable->visible = false;
}

float Ghost::distance(bool use_last_known) {
	if (use_last_known) {
		return glm::distance(last_known, drawable->transform->position);
	}
	return glm::distance(player->position, drawable->transform->position);
}
