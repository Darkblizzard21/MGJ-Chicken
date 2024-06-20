#include "ContactListener.h"

ContactListener::ContactListener(Minecart& minecart) : minecart(minecart) {
}

void ContactListener::BeginContact(b2Contact* contact) {
	minecart.onCollision();
}