#include "Coin.h"
#include "ChickenWings.h"

std::shared_ptr<Texture> Coin::colorTex = nullptr;
std::shared_ptr<Texture> Coin::normalTex = nullptr;

Coin::Coin(float xPos) {
	float yPos = ChickenWings::game.spline->sampleHight(xPos);

	if (colorTex == nullptr) colorTex = std::make_shared<Texture>("Coin1.png");
	if (normalTex == nullptr) normalTex = std::make_shared<Texture>("Coin1N.png");

	quad = ChickenWings::game.quadManager.CreateQuad(colorTex, normalTex);
	quad->position = glm::vec2(xPos, yPos + 1);
	quad->layer = 150;
	quad->scale = {0.8f, 0.8f};

	b2BodyDef bodyDef;
	bodyDef.position.Set(xPos, yPos + 0.5f);
	b2PolygonShape collider;
	collider.SetAsBox(0.5f * 0.8f, 0.5f * 0.8f);

	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = true;
	fixtureDef.shape = &collider;
	fixtureDef.density = 0;
	fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(tag);

	body = ChickenWings::game.world.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);
}

void Coin::Update()
{

}

Coin::~Coin() {
	delete tag;
	ChickenWings::game.world.DestroyBody(body);
}

void Coin::onCollision(b2Contact* contact)
{

}
