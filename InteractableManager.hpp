#pragma once

#include <unordered_set>
#include <vector>

#include "Interactable.hpp"
#include "Load.hpp"
#include "UI.hpp"

/* Player inventory */
struct Inventory {

  Inventory();

  std::unordered_set<ItemType> items; // all items
  int size = 5;                       // can initially hold 5 items
  int holding_item = 0; // the item the player currently hold s. 0 = nothing

  virtual bool hasItem(ItemType item_type);
  virtual bool addItem(ItemType item_type);
  virtual bool removeItem(ItemType item_type);
};

/* Manages the inventory, all items, all furniture*/
struct InteractableManager {

    int current_phase = 0;
  Inventory inventory = Inventory();

  GameplayUI *gameplayUI = nullptr;

  std::vector<Item *> items;
  std::vector<Furniture *> furnitures;

  void load(const Scene &scenes, GameplayUI *a_gameplayUI);

  // in each frame, we check interactable objects
  void update(Scene::Transform *player_transform, Scene::Camera *camera,
              bool interact_pressed, float elapsed);

  bool updateFurniture(Scene::Transform *player_transform,
                       Scene::Camera *camera, bool interact_pressed,
                       float elapsed);
  bool updateItem(Scene::Transform *player_transform, Scene::Camera *camera,
                  bool interact_pressed, float elapsed);

  void setFurniturePhaseAvailability(FurnitureType furniture_type, bool allow);
  void setItemPhaseAvailability(ItemType item_type, bool allow);

  // all interactable objects valid check
  bool interactValidCheck(FurnitureType furniture_type);

  bool interactStatusCheck(FurnitureType furniture_type);

  // furniture interaction in current phase, used to forward phase
  FurnitureType cur_furniture = NONE; // TODO: NOT used
  std::string interaction_notification = "";
};
