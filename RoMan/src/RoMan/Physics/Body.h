#pragma once

struct Shape;

struct Body
{
	Body(Shape* shape_, glm::uint32 x, glm::uint32 y);

	void ApplyForce(const glm::vec2& f)
	{
		force += f;
	}

	void ApplyImpulse(const glm::vec2& impulse, const glm::vec2& contactVector)
	{
		velocity += inverseMass * impulse;
		angularVelocity += inverseInertia * glm::cross(glm::vec3(contactVector, 0.0f), glm::vec3(impulse, 0.0f)).z;
	}

	void SetStatic(void)
	{
		inertia = 0.0f;
		inverseInertia = 0.0f;
		mass = 0.0f;
		inverseMass = 0.0f;
	}

	void SetOrient(float radians);

	glm::vec2 position;
	glm::vec2 velocity;

	float angularVelocity;
	float torque;
	float orient; // Orientation - in radians

	glm::vec2 force;

	float inertia;
	float inverseInertia;
	float mass;
	float inverseMass;

	float staticFriction;
	float dynamicFriction;
	float restitution;

	Shape* shape;

	float r, g, b;
};

inline float Random(float l, float h)
{
	float a = (float)rand();
	a /= RAND_MAX;
	a = (h - l) * a + l;
	return a;
}