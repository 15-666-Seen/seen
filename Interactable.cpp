#include "Interactable.hpp"
#include "Load.hpp"
#include <glm/glm.hpp>

#include <iostream>
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

/* INTERACTABLE MANAGER */
void InteractableManager::load(Load<Scene> scene) {
  for (const auto &[mesh_name, furniture_type] : MeshNameToFurnitureType) {
    auto it = scene->mesh_name_to_transform.find(mesh_name);
    if (it == scene->mesh_name_to_transform.end()) {
      std::cout << "Furniture mesh not found: " << mesh_name << std::endl;
      continue; // TODO: this should not be continue, should exit
    }

    // create new furniture
    Furniture *furniture = new Furniture();
    furniture->type = furniture_type;
    furniture->transform = it->second;
    // TODO: allowable need check
    furniture->phase_allow_interact = true;
    furniture->can_interact = true;
  }

  for (const auto &[mesh_name, item_type] : MeshNameToItemType) {
    auto it = scene->mesh_name_to_transform.find(mesh_name);
    if (it == scene->mesh_name_to_transform.end()) {
      std::cout << "Item mesh not found: " << mesh_name << std::endl;
      continue; // TODO: this should not be continue, should exit
    }

    // create new item
    Item *item = new Item();
    item->type = item_type;
    item->transform = it->second;

    // TODO: allowable need check
    item->visible = true;
    item->can_interact = true;
    item->phase_allow_interact = true;
  }
}
