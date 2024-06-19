#pragma once
#include <box2d/box2d.h>
#include "Minecart.h"

class ContactListener : public b2ContactListener {
public:
	ContactListener(Minecart& minecart);

	virtual void BeginContact(b2Contact* contact) override;

private:
	Minecart& minecart;
};