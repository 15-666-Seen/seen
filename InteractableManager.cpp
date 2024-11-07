#include "InteractableManager.hpp"

#include "Interactable.hpp"
#include "util.hpp"

#include <iostream>

/* INVENTORY */
Inventory::Inventory() {}
bool Inventory::hasItem(ItemType item_type) {
  return items.find(item_type) != items.end();
}
bool Inventory::addItem(ItemType item_type) {
  if (items.find(item_type) != items.end()) {
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

// =============================================================================

/* INTERACTABLE MANAGER */
void InteractableManager::load(const Scene &scene, GameplayUI *a_gameplayUI) {
  this->gameplayUI = a_gameplayUI;

  for (const auto &[mesh_name, furniture_type] : MeshNameToFurnitureType) {
    auto it = scene.mesh_name_to_drawable.find(mesh_name);
    if (it == scene.mesh_name_to_drawable.end()) {
      wait_and_exit("Furniture mesh not found: " + mesh_name);
    }
    Furniture *furniture;
    // Special case to add offset for door
    if (furniture_type == BEDROOM_DOOR || furniture_type == DOOR1) {
      furniture = new Door();
    } else {
      furniture = new Furniture();
    }

    furniture->type = furniture_type;
    furniture->drawable = it->second;

    // TODO: allowable need check
    furniture->phase_allow_interact = false;
    furniture->can_interact = true;

    furnitures.push_back(furniture);
  }

  for (const auto &[mesh_name, item_type] : MeshNameToItemType) {
    auto it = scene.mesh_name_to_drawable.find(mesh_name);
    if (it == scene.mesh_name_to_drawable.end()) {
      wait_and_exit("Item mesh not found: " + mesh_name);
    }

    // create new item
    Item *item = new Item();
    item->type = item_type;
    item->drawable = it->second;

    // TODO: allowable need check
    item->visible = true;
    item->can_interact = true;
    item->phase_allow_interact = false;

    items.push_back(item);
  }
}

int InteractableManager::update(Scene::Transform *player_transform,
                                Scene::Camera *camera, bool interact_pressed,
                                float elapsed) {

  gameplayUI->setInteractionText(""); // Clear interaction text
  interaction_notification = "";      // clear interaction text

  bool used_in_current_frame =
      updateFurniture(player_transform, camera, interact_pressed, elapsed);
  if (!used_in_current_frame) {
    updateItem(player_transform, camera, interact_pressed, elapsed);
  }

  // some notification to show
  if (!interaction_notification.empty()) {
    gameplayUI->insertDialogueText(interaction_notification);
  }

  return -1;
}

bool InteractableManager::updateFurniture(Scene::Transform *player_transform,
                                          Scene::Camera *camera,
                                          bool interact_pressed,
                                          float elapsed) {
  for (auto &furniture : furnitures) {

    if (furniture->isInteracting()) {
      furniture->interact(elapsed);
    }

    if (!furniture->interactable(player_transform, camera)) {
      continue;
    }

    // as long as its interactable, set the interaction text
    gameplayUI->setInteractionText(furniture->interactText());

    if (interact_pressed) {
      cur_furniture = furniture->type;

      // TODO: set different notification
      if (furniture->type == BEDROOM_DOOR) {

        // phase 0 can directly open the door
        if (current_phase == 0) {
          Door *door = dynamic_cast<Door *>(furniture);
          door->state = Door::DoorState::OPENING;
          furniture->phase_allow_interact = false;
          return true;
        }

        else if (current_phase == 2) {
          Door *door = dynamic_cast<Door *>(furniture);
          if (!inventory.hasItem(BEDROOM_KEY)) {
            interaction_notification = "This door is locked.";
            return true;
          } else {
            door->state = Door::DoorState::OPENING;
            furniture->phase_allow_interact = false;
            return true;
          }
        }
      } else if (furniture->type == BED) {
        furniture->interact_status = 1;
        return true;
      } else if (furniture->type == DOOR1) {
        if (!inventory.hasItem(DOOR1_KEY)) {
          interaction_notification = "This door is locked.";
          return true;
        }
      }
    }

    return true;
  }
  return false;
}

bool InteractableManager::updateItem(Scene::Transform *player_transform,
                                     Scene::Camera *camera,
                                     bool interact_pressed, float elapsed) {
  for (auto &item : items) {
    if (!item->interactable(player_transform, camera)) {
      continue;
    }
    gameplayUI->setInteractionText(item->interactText());
    if (interact_pressed) {
      inventory.addItem(item->type);
      item->interact(elapsed);
    }
    return true;
  }
  return false;
}

void InteractableManager::setFurniturePhaseAvailability(
    FurnitureType furniture_type, bool allow) {
  for (auto &furniture : furnitures) {
    if (furniture->type == furniture_type) {
      furniture->phase_allow_interact = allow;
      return;
    }
  }
}
void InteractableManager::setItemPhaseAvailability(ItemType item_type,
                                                   bool allow) {
  for (auto &item : items) {
    if (item->type == item_type) {
      item->phase_allow_interact = allow;
      return;
    }
  }
}

int InteractableManager::interactStatusCheck(FurnitureType furniture_type) {
  for (auto &furniture : furnitures) {
    if (furniture->type == furniture_type)
      return furniture->interact_status;
  }
  wait_and_exit(
      "Interactable Manager cpp interactStatusCheck() furniture not found.");
  return 0;
}

int InteractableManager::interactStatusCheck(ItemType item_type) {
  for (auto &item : items) {
    if (item->type == item_type)
      return item->interact_status;
  }
  wait_and_exit(
      "Interactable Manager cpp interactStatusCheck() item not found.");
  return 0;
}

void InteractableManager::closeBedroomDoor() {
  for (auto &furniture : furnitures) {
    if (furniture->type == BEDROOM_DOOR) {
      Door *door = dynamic_cast<Door *>(furniture);
      door->state = Door::DoorState::CLOSED;
      door->animation_time = 0.0f;
      door->interact_status = 0;
      door->drawable->transform->rotation =
          glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      return;
    }
  }
}