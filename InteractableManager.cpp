#include "InteractableManager.hpp"

#include "Interactable.hpp"
#include "sound_prep.hpp"
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
    if (furniture_type == BEDROOM_DOOR || furniture_type == DOOR1 
        || furniture_type == REDROOM_DOOR || furniture_type == FRONT_DOOR) {
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
        if (current_phase == 3) {
          if (inventory.hasItem(BEDROOM_KEY)) {
            Door *door = dynamic_cast<Door *>(furniture);
            door->state = Door::DoorState::OPENING;
            furniture->phase_allow_interact = false;
            return true;
          }
          interaction_notification = "Seems like I need a key to open this.";
          return true;
        }
        interaction_notification = "This door is locked from inside.";
      }

      if (furniture->type == REDROOM_DOOR) {
              if (current_phase >= 5 && inventory.hasItem(REDROOM_KEY)) {
                  // TODO: DEBUG this crashes the game
                  Door* door = dynamic_cast<Door*>(furniture);
                  door->state = Door::DoorState::OPENING;
                  furniture->interact_status = 1;
                  furniture->phase_allow_interact = false;
                  return true;
              }
              else if (current_phase == 4 && inventory.hasItem(REDROOM_KEY)) {
                  interaction_notification = "NO TIME TO OPEN DOORS!!";
                  return true;
              }
              interaction_notification = "Seems like I need a key to open this.";
              return true;
      }

      if (furniture->type == BLUEPRINT) {
          gameplayUI->insertDialogueText("Great. Because the one place I'd want to go in a creepy old house is a dark, hidden basement...");
          gameplayUI->insertDialogueText("The arrow is pointing to...a basement under the sofa?");
          gameplayUI->insertDialogueText("A blueprint of the house... maybe it'll help me find a way out.");
          furniture->interact_status = 1;
          furniture->phase_allow_interact = false;
          return true;
      }

      else if (furniture->type == DOOR1) {
        // TODO: currently directly open door1
        if (current_phase == 0) {
          if (inventory.hasItem(DEN_KEY)) {
            Door *door = dynamic_cast<Door *>(furniture);
            door->state = Door::DoorState::OPENING;
            furniture->phase_allow_interact = false;
            return true;
          }
          interaction_notification = "Seems like I need a key to open this.";
        }
      }

      else if (furniture->type == FRONT_DOOR) {
          // TODO: currently directly open door1
          if (furniture->interact_status == 0) {
              setFurniturePhaseVisibility(CHAIN_CUT1, false);
              Door* door = dynamic_cast<Door*>(furniture);
              door->state = Door::DoorState::OPENING;
              furniture->interact_status = 1;
              interaction_notification = "It Opened..";
              return true;      
          }
          else if (furniture->interact_status == 1) {
              furniture->interact_status = 2;
              furniture->phase_allow_interact = false;
          }
      }


      else if (furniture->type == DOORBLOCK) {
          if (furniture->interact_status == 0) {
              interaction_notification = "Tentacles?? Am I losing my mind?";
              furniture->interact_status = 1;
          }
          else {
              gameplayUI->insertDialogueText("Maybe it's easier to just... stay. No! Focus, I need to find another way out!");
              gameplayUI->insertDialogueText("Something's trying to keep me in here...");
              gameplayUI->insertDialogueText("It's like they're growing into the walls... I can't pull them off.");
              setFurniturePhaseAvailability(BOOKSHELF, true);
          }
          return true;
      }

      else if (furniture->type == CHAIN) {
          if (furniture->interact_status == 0) {
              Sound::play_3D(*chain_sample, 2.0f,
                  furniture->drawable->transform->position, 10.0f);
              gameplayUI->insertDialogueText("No. No way. I can't be seeing this right. What kind of place is this?");
              gameplayUI->insertDialogueText("I turned around for a second and this chain... vanished out of thin air?");
              gameplayUI->insertDialogueText("...");
              furniture->interact_status = 1;
          }
          else if (inventory.hasItem(CLIP_L) && inventory.hasItem(CLIP_R) && inventory.hasItem(CLIP_M)) {
              Sound::play_3D(*cut_chain_sample, 2.0f,
                  furniture->drawable->transform->position, 10.0f);

              // display cut chain
              setFurniturePhaseVisibility(CHAIN_CUT1, true);
              setFurniturePhaseAvailability(CHAIN_CUT1, false);
              setFurniturePhaseVisibility(CHAIN_CUT2, true);
              setFurniturePhaseAvailability(CHAIN_CUT2, false);
              setFurniturePhaseVisibility(CHAIN, false);
              setFurniturePhaseAvailability(CHAIN, false);

              setFurniturePhaseAvailability(FRONT_DOOR, true);

          }
          else if (inventory.hasItem(CLIP_L) || inventory.hasItem(CLIP_R) || inventory.hasItem(CLIP_M)) {
              interaction_notification = "I have some parts of a chain cutter now..maybe if I find all three parts I can cut my way out!";
          }
          else {
              interaction_notification = "Still locked. I can't untangle it.";
          }
          
         return true;
      }


      else if (furniture->type == TINY_SCULPTURE) {
         
        if (!inventory.hasItem(EYEBALL)) {
          interaction_notification = "Hmm... Seems something is missing here.";
        } else {
            // can no longer interact w sculpture
            setFurniturePhaseAvailability(TINY_SCULPTURE, false);

            // animation
            furnituresMap[TINY_SCULPTURE_TENTACLES]->interact_status = 1;
            furnituresMap[SCULPTURE_EYE_R]->drawable->visible = true;
            setFurniturePhaseVisibility(TINY_SCULPTURE_TENTACLES, true);
            scaleFurniture(TINY_SCULPTURE_TENTACLES, 0.001f);
        }
      }

      else if (furniture->type == BOOKSHELF) {
        furniture->drawable->transform->position.x -= 2.0f;
        furniture->interact_status = 1;
      }

      else if (furniture->type == BED) {
        if (furniture->interact_status == 0) {
          furniture->interact_status = 1;
          // modify player's view
          camera->transform->position =
              glm::vec3(-8.9071f, -4.72332f, 3.21787f);
          camera->yaw = -1.54317f;
          camera->pitch = 0.327246f;
          isHiding = true;
        } else {
          furniture->interact_status = 0;
          // here we modify player pos
          camera->transform->position = player_transform->position;
          camera->transform->position.z += PLAYER_HEIGHT;
          camera->yaw = -1.54317f;
          camera->pitch = 0.327246f;
          isHiding = false;
        }
      }

      else if (furniture->type == CLOSET2) {
        if (furniture->interact_status == 0) {
          furniture->interact_status = 1;
          // modify player's view
          camera->transform->position = glm::vec3(
              6.85f, 4.65f, player_transform->position.z + PLAYER_HEIGHT);
          camera->yaw = 0.0f;
          camera->pitch = 0.0f;
          isHiding = true;
        } else {
          furniture->interact_status = 0;
          camera->transform->position = player_transform->position;
          camera->transform->position.z += PLAYER_HEIGHT;
          camera->yaw = -3.14159f;
          camera->pitch = 0.327246f;
          isHiding = false;
        }
      }

      else if (furniture->type == SOFA) {
        // Push sofa aside
        if (current_phase == 10) {
          furniture->drawable->transform->position +=
              glm::vec3(0.0f, 1.5f, 0.f);
          furniture->phase_allow_interact = false;
          // we can now use the ladder! yay!
          setFurniturePhaseAvailability(LADDER, true);
          return true;
        }
        interaction_notification =
            "This is a sofa. The pillows have a nice color scheme.";
        return true;
      } else if (furniture->type == LADDER) {
        if (current_phase == 10) {
          // go down
          furniture->interact_status = 1;
          return true;
        } else if (current_phase == 11 && inventory.hasItem(CLIP_R)) {
          // can only use when clip r is found
          // go up
          furniture->interact_status = 2;
          furniture->phase_allow_interact = false;

          setFurniturePhaseAvailability(FRIDGE1, false);
          setFurniturePhaseAvailability(FRIDGE2, false);
          setFurniturePhaseAvailability(FRIDGE3, false);
          setFurniturePhaseAvailability(FRIDGE4, false);
          setFurniturePhaseAvailability(FRIDGE5, false);

          return true;
        }

        interaction_notification =
            "Not yet. This place wouldn't be marked on the blueprint if this is all.";
        return true;
      } else if (furniture->type == FRIDGE1) {
        // Push sofa aside
        if (current_phase == 11) {
          Sound::play_3D(*fridge_sample, 2.0f,
                furniture->drawable->transform->position, 10.0f);

          Sound::play_3D(*breath_trigger_sample, 2.0f,
              furniture->drawable->transform->position, 10.0f);
          furniture->drawable->transform->rotation +=
              glm::angleAxis(glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
          furniture->drawable->transform->scale = glm::vec3(1.f);

          furniture->phase_allow_interact = false;
          interaction_notification = "Woah!!";
          return true;
        }
        return true;
      }

      else if (furniture->type == FRIDGE2) {
        // Push sofa aside
        if (current_phase == 11) {
            Sound::play_3D(*fridge_sample, 2.0f,
                furniture->drawable->transform->position, 10.0f);
          furniture->drawable->transform->rotation +=
              glm::angleAxis(glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
          furniture->drawable->transform->scale = glm::vec3(1.f);

          furniture->phase_allow_interact = false;
          return true;
        }
        return true;
      }

      else if (furniture->type == FRIDGE3) {
        // Push sofa aside
        if (current_phase == 11) {
            Sound::play_3D(*fridge_sample, 2.0f,
                furniture->drawable->transform->position, 10.0f);
          furniture->drawable->transform->rotation =
              glm::angleAxis(glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
          furniture->drawable->transform->scale = glm::vec3(1.f);

          furniture->phase_allow_interact = false;
          return true;
        }
        return true;
      }

      else if (furniture->type == FRIDGE4) {
        if (current_phase == 11) {
            Sound::play_3D(*fridge_sample, 2.0f,
                furniture->drawable->transform->position, 10.0f);
          furniture->drawable->transform->rotation =
              glm::angleAxis(glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
          furniture->drawable->transform->scale = glm::vec3(1.f);
          furniture->phase_allow_interact = false;
          setItemPhaseAvailability(CLIP_R, true);
          return true;
        }
        return true;
      }

      else if (furniture->type == FRIDGE5) {
        // Push sofa aside
        if (current_phase == 11) {
            Sound::play_3D(*fridge_sample, 2.0f,
                furniture->drawable->transform->position, 10.0f);
          furniture->drawable->transform->rotation =
              glm::angleAxis(glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));
          furniture->drawable->transform->scale = glm::vec3(1.f);
         

          furniture->phase_allow_interact = false;
          return true;
        }
        return true;
      }
    }

    return true;
  }
  return false;
}

bool InteractableManager::updateItem(Scene::Transform *player_transform,
                                     Scene::Camera *camera,
                                     bool interact_pressed, float elapsed) {

    int total_pt = inventory.hasItem(CLIP_L) + inventory.hasItem(CLIP_M) + inventory.hasItem(CLIP_R);
    
    for (auto &[_, item] : itemsMap) {
    if (!item->interactable(player_transform, camera)) {
      continue;
    }
    gameplayUI->setInteractionText(item->interactText());
    if (interact_pressed) {
      inventory.addItem(item->type);
      if (item->type == CLIP_L) {
          interaction_notification =
              "Part of a chain cutter. (" + std::to_string((total_pt + 1)) + "/3)";
      }
      else if (item->type == CLIP_R) {
          interaction_notification =
              "Part of a chain cutter. (" + std::to_string((total_pt + 1)) + "/3)";
      }
      else if (item->type == CLIP_M) {
          interaction_notification =
              "Part of a chain cutter. (" + std::to_string((total_pt + 1)) + "/3)";
      }
      else if (item->type == FILE2) {
          gameplayUI->insertDialogueText("'Deny any claims about this house being alive'..what?");
          gameplayUI->insertDialogueText("'Deny knowing the missing clergies. Deny the smell, call it old building issues.' ");
          gameplayUI->insertDialogueText("'Countermeasure to the police'... What are they hiding here?");
      }
      else if (item->type == FILE1) {
          gameplayUI->insertDialogueText(" So, if I run into trouble... if I hide... they'll disappear.");
          gameplayUI->insertDialogueText("'If a member is seen, they must hide to sever the link.'");
          gameplayUI->insertDialogueText("'Those who see invite their gaze. Those who flee invite their wrath.'");
          gameplayUI->insertDialogueText("'To appease The Old Ones: one soul for every trespass. The link between seer and seen must remain unbroken.'");
          gameplayUI->insertDialogueText("'Sacrifice Record'... What is this?");
      }

      else if (item->type == BEDROOM_KEY) {
        interaction_notification = "A Green Key.";
      } else if (item->type == DEN_KEY) {
        interaction_notification = "A Blue Key.";
      } else if (item->type == REDROOM_KEY) {
        interaction_notification = "A Red Key.";
      } else if (item->type == EYEBALL) {
        interaction_notification = "A stinky slimy eyeball.";
      }
      item->interact(elapsed);
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
void InteractableManager::setFurniturePhaseVisibility(
    FurnitureType furniture_type, bool visible) {
  furnituresMap[furniture_type]->drawable->visible = visible;
}
void InteractableManager::setItemPhaseVisibility(ItemType item_type,
                                                 bool visible) {
  itemsMap[item_type]->drawable->visible = visible;
}

int InteractableManager::interactStatusCheck(FurnitureType furniture_type) {
  return furnituresMap[furniture_type]->getInteractStatus();
}

int InteractableManager::interactStatusCheck(ItemType item_type) {
  return itemsMap[item_type]->interact_status;
}

void InteractableManager::closeDoor(FurnitureType furniture_type) {
  Door *door = dynamic_cast<Door *>(furnituresMap[furniture_type]);
  door->interact_sound = Sound::play_3D(
      *door_close_sample, 2.0f, door->drawable->transform->position, 10.0f);
  door->state = Door::DoorState::CLOSED;
  door->interact_status = 0; // set interaction text
  door->drawable->transform->rotation =
      glm::angleAxis(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
  door->animation_time = 0.0f;
}

void InteractableManager::scaleFurniture(FurnitureType furniture_type, float scale) {
    for (auto& [_, furniture] : furnituresMap) {

        if (furniture->type == furniture_type) {
            furniture->drawable->transform->scale = glm::vec3(scale);
        }
    }
}