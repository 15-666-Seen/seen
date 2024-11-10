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

    furnituresMap[furniture_type] = furniture;
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

    itemsMap[item_type] = item;
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
  for (auto &[_, furniture] : furnituresMap) {

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
  for (auto &[_, item] : itemsMap) {
    if (!item->interactable(player_transform, camera)) {
      continue;
    }
    gameplayUI->setInteractionText(item->interactText());
    if (interact_pressed) {
      inventory.addItem(item->type);
      item->interact(elapsed);

      if (item->type == FILE1) {
        closeDoor(BEDROOM_DOOR);
      }
    }
    return true;
  }
  return false;
}

void InteractableManager::setFurniturePhaseAvailability(
    FurnitureType furniture_type, bool allow) {
  furnituresMap[furniture_type]->phase_allow_interact = allow;
}
void InteractableManager::setItemPhaseAvailability(ItemType item_type,
                                                   bool allow) {
  itemsMap[item_type]->phase_allow_interact = allow;
}

int InteractableManager::interactStatusCheck(FurnitureType furniture_type) {
  return furnituresMap[furniture_type]->getInteractStatus();
}

int InteractableManager::interactStatusCheck(ItemType item_type) {
  return itemsMap[item_type]->interact_status;
}

void InteractableManager::closeDoor(FurnitureType furniture_type) {
  Door *door = dynamic_cast<Door *>(furnituresMap[furniture_type]);
  door->state = Door::DoorState::CLOSED;
  door->interact_status = 0; // set interaction text
  door->drawable->transform->rotation =
      glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  door->animation_time = 0.0f;
}
