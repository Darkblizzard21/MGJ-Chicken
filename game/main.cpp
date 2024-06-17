#include "App.h"
#include "ChickenWings.h"

int main() {
	//std::shared_ptr<Quad> quad = ChickenWings::game.quadManager->CreateQuad();
	//quad->shader = ChickenWings::game.shader;
	//quad->position.x += 0.5f;
	//quad->rotation = 1.33f;
	ChickenWings::game.run();
}