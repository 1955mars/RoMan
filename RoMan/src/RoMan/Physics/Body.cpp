#include "rmpch.h"
#include "Physics.h"

Body::Body(Shape* shape_, glm::uint32 x, glm::uint32 y) : shape(shape_->Clone())
{
	shape->body = this;
	position = glm::vec2((float)x, (float)y);
	velocity = glm::vec2(0.01f, 0.01f);
	angularVelocity = 0.01f;
	torque = 0.0f;
	orient = Random(-PI, PI);
	force = glm::vec2(0.0f, 0.0f);
	staticFriction = 0.5f;
	dynamicFriction = 0.3f;
	restitution = 0.2f;
	shape->Initialize();
	r = Random(0.2f, 1.0f);
	g = Random(0.2f, 1.0f);
	b = Random(0.2f, 1.0f);
}

void Body::SetOrient(float radians)
{
	orient = radians;
	shape->SetOrient(radians);
}