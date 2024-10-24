

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <unordered_set>
#include <vector>

#include "Scene.hpp"

/* interactable object that can be picked up */
enum ItemType { BEDROOM_KEY };

/* Type of interactable object that cannot be picked up */
enum FurnitureType { BED, CLOSET, BEDROOM_DOOR, FRONT_DOOR };

/* A single furniture */
struct Furniture {

  Furniture();

  FurnitureType type;
  Scene::Transform *transform;

  bool visible;
  bool canInteract;

  virtual bool interact();
};

/* A single item */
struct Item {

  Item();

  ItemType type;
  Scene::Transform *transform;

  bool visible;
  bool canInteract;

  virtual bool interact();
};

/* Player inventory */
struct Inventory {

  Inventory();

  std::unordered_set<ItemType> items; // all items
  int size = 5;                       // can initially hold 5 items
  int current_item = 0; // the item the player currently hold s. 0 = nothing

  virtual bool hasItem(ItemType item_type);
  virtual bool addItem(ItemType item_type);
  virtual bool removeItem(ItemType item_type);
};

/* Manages the inventory, all items, all furniture*/
struct InteractableManager {

  Inventory inventory = Inventory();

  std::vector<Item *> items; // better name? This collides with inventory->items
  std::vector<Furniture *> furnitures;
};
