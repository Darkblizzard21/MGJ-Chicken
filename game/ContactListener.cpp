#include "ContactListener.h"
#include <iostream>

ContactListener::ContactListener(Minecart& minecart, std::vector<std::unique_ptr<Obstacle>>& obsticles) :
	minecart(minecart),
	obsticles(obsticles){
}

void ContactListener::BeginContact(b2Contact* contact) {
	minecart.onCollision(contact);

	uintptr_t ptrA = contact->GetFixtureA()->GetUserData().pointer;
	if (ptrA != 0) {
		std::string tag = *reinterpret_cast<std::string*>(ptrA);
		if (tag == "Obstacle") {
			std::cout << "Hit Obsacle" << std::endl;
			for (int i = 0; i < obsticles.size(); i++)
			{
				obsticles[i]->isMarkdForDeletion = true;
			}
			b2Vec2 forceVector = b2Vec2(0, 2);
			minecart.body->ApplyLinearImpulseToCenter(forceVector, true);
			minecart.body->ApplyAngularImpulse(-2, true);
		}

	}

	uintptr_t ptrB = contact->GetFixtureB()->GetUserData().pointer;
	if (ptrB != 0) {
		std::string tag = *reinterpret_cast<std::string*>(ptrB);
		if (tag == "Obstacle") {
			std::cout << "Hit Obsacle" << std::endl;
			for (int i = 0; i < obsticles.size(); i++)
			{
				obsticles[i]->isMarkdForDeletion = true;
			}
			b2Vec2 forceVector = b2Vec2(0, 4);
			minecart.body->ApplyLinearImpulseToCenter(forceVector, true);
			minecart.body->ApplyAngularImpulse(4, true);

		}

	}
}