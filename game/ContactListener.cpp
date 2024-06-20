#include "ContactListener.h"
#include <iostream>
#include "ChickenWings.h"

ContactListener::ContactListener(Minecart& minecart, std::vector<std::unique_ptr<Obstacle>>& obsticles, std::vector<std::unique_ptr<Coin>>& coins) :
	minecart(minecart),
	obsticles(obsticles),
	coins(coins)
{
}

void ContactListener::BeginContact(b2Contact* contact) {



	uintptr_t ptrA = contact->GetFixtureA()->GetUserData().pointer;
	if (ptrA != 0) {
		std::string tag = *reinterpret_cast<std::string*>(ptrA);
		if (tag == "Obstacle") {
			for (int i = 0; i < obsticles.size(); i++)
			{
				obsticles[i]->isMarkdForDeletion = true;
			}
			b2Vec2 forceVector = b2Vec2(0, 1);
			minecart.body->ApplyLinearImpulseToCenter(forceVector, true);
			minecart.body->ApplyAngularImpulse(-1, true);
			minecart.isAirborn = true;
		}
		if (tag == "Coin") {
			ChickenWings::game.ScoreCoin();
			for (int i = 0; i < coins.size(); i++)
			{
				coins[i]->isMarkdForDeletion = true;
			}
			return;
		}
	}

	uintptr_t ptrB = contact->GetFixtureB()->GetUserData().pointer;
	if (ptrB != 0) {
		std::string tag = *reinterpret_cast<std::string*>(ptrB);
		if (tag == "Obstacle") {
			for (int i = 0; i < obsticles.size(); i++)
			{
				obsticles[i]->isMarkdForDeletion = true;
			}
			b2Vec2 forceVector = b2Vec2(0, 3);
			minecart.body->ApplyLinearImpulseToCenter(forceVector, true);
			minecart.body->ApplyAngularImpulse(4, true);
			minecart.isAirborn = true;
		}
		if (tag == "Coin") {
			ChickenWings::game.ScoreCoin();
			for (int i = 0; i < coins.size(); i++)
			{
				coins[i]->isMarkdForDeletion = true;
			}
			return;
		}
	}

	minecart.onCollision(contact);

}