#include "Interactable.hpp"
#include <glm/glm.hpp>

#include <string>
#include <vector>

/* INVENTORY */
Inventory::Inventory() {}

bool Inventory::hasItem(ItemType item_type) {
  return items.find(item_type) != items.end();
}

bool Inventory::addItem(ItemType item_type) {
  if (items.find(item_type) != items.end()) {
    assert(false && "Item already in inventory");
    return false;
  }
  items.insert(item_type);
  return true;
}

bool Inventory::removeItem(ItemType item_type) {
  if (items.find(item_type) == items.end()) {
    return false;
  }
  items.erase(item_type);
  return true;
}

/* ITEM */
Item::Item() {}

bool Item::interact() { return can_interact; }

bool Item::interactable(Scene::Transform *player_transform) {
  // should be visible and phase_allow_interact
  if (!visible || !phase_allow_interact) {
    return false;
  }

  // check if player is close enough to interact with item
  glm::vec3 player_pos = player_transform->position;
  glm::vec3 item_pos = transform->position;
  float distance = glm::distance(player_pos, item_pos);

  can_interact = distance < ITEM_INTERACT_DISTANCE;
  return can_interact;
}

/* FURNITURE */
Furniture::Furniture() {}

bool Furniture::interact() { return can_interact; }

bool Furniture::interactable(Scene::Transform *player_transform) {
  // should be phase_allow_interact
  if (!phase_allow_interact) {
    return false;
  }

  // check if player is close enough to interact with furniture
  glm::vec3 player_pos = player_transform->position;
  glm::vec3 furniture_pos = transform->position;
  float distance = glm::distance(player_pos, furniture_pos);

  can_interact = distance < FURNITURE_INTERACT_DISTANCE;
  return can_interact;
}
