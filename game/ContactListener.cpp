#include "ContactListener.h"
#include <iostream>

ContactListener::ContactListener(Minecart& minecart) : minecart(minecart) {
}

void ContactListener::BeginContact(b2Contact* contact) {
	minecart.isAirborn = false;
}