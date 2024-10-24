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

bool Item::interact() { return false; }

/* FURNITURE */
Furniture::Furniture() {}

bool Furniture::interact() { return false; }
