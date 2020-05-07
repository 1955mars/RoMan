#pragma once

struct Body;

struct Manifold
{
	Manifold(Body* a, Body* b) : A(a), B(b) {}

	void Solve(void);
	void Initialize(void);
	void ApplyImpulse(void);
	void PositionalCorrection(void);
	void InfiniteMassCorrection(void);

	Body* A;
	Body* B;

	float penetration;
	glm::vec2 normal;
	glm::vec2 contacts[2];
	glm::uint32 contact_count;
	float e;  // Restitution
	float df; // Dynamic friction
	float sf; // Static friction
};