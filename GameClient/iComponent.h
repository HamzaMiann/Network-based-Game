#pragma once

class cGameObject;

class iComponent
{
public:
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void collide(cGameObject* collided_with) {}
};