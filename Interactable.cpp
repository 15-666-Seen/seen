#include "Interactable.hpp"
#include <glm/glm.hpp>

#include <string>
#include <vector>


/* INVENTORY */
Inventory::Inventory() {

}

bool Inventory::hasItem() {
	return false;
}

bool Inventory::addItem() {
	return false;
}

bool Inventory::removeItem() {
	return false;
}

/* ITEM */
Item::Item() {

}

bool Item::interact() {
	return false;
}

/* FURNITURE */
Furniture::Furniture() {

}

bool Furniture::interact() {
	return false;
}
