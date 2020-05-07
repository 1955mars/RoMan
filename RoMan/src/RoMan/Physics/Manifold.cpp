#include "rmpch.h"
#include "Physics.h"

void Manifold::Solve(void)
{
	Dispatch[A->shape->GetType()][B->shape->GetType()](this, A, B);
}

void Manifold::Initialize(void)
{
	// Calculate average restitution
	e = glm::min(A->restitution, B->restitution);

	// Calculate static and dynamic friction
	sf = glm::sqrt(A->staticFriction * A->staticFriction);
	df = glm::sqrt(A->dynamicFriction * A->dynamicFriction);

	for (glm::uint32 i = 0; i < contact_count; ++i)
	{
		// Calculate radii from COM to contact
		glm::vec2 ra = contacts[i] - A->position;
		glm::vec2 rb = contacts[i] - B->position;

		glm::vec2 rv = B->velocity + glm::vec2(-B->angularVelocity * rb.y, B->angularVelocity * rb.x) -
					   A->velocity - glm::vec2(-A->angularVelocity * ra.y, A->angularVelocity * ra.x);

		float rvsqr = rv.x * rv.x + rv.y * rv.y;
		float gravsqr = (dt * gravity.x) * (dt * gravity.x) + (dt * gravity.y) * (dt * gravity.y);

		if (rvsqr < gravsqr + EPSILON)
			e = 0.0f;
	}
}

void Manifold::ApplyImpulse(void)
{
	// Don't bother if both objects have infinite mass
	if (glm::abs(A->inverseMass + B->inverseMass - 0.0f) <= EPSILON)
	{
		InfiniteMassCorrection();
		return;
	}

	for (glm::uint32 i = 0; i < contact_count; ++i)
	{
		// Calculate radii from COM to contact
		glm::vec2 ra = contacts[i] - A->position;
		glm::vec2 rb = contacts[i] - B->position;

		// Relative velocity
		glm::vec2 rv = B->velocity + glm::vec2(-B->angularVelocity * rb.y, B->angularVelocity * rb.x) -
					   A->velocity - glm::vec2(-A->angularVelocity * ra.y, A->angularVelocity * ra.x);

		// Relative velocity along the normal
		float contactVel = glm::dot(rv, normal);

		// Do not resolve if velocities are separating
		if (contactVel > 0)
			return;

		float raCrossN = glm::cross(glm::vec3(ra, 0.0f), glm::vec3(normal, 0.0f)).z;
		float rbCrossN = glm::cross(glm::vec3(rb, 0.0f), glm::vec3(normal, 0.0f)).z;
		float invMassSum = A->inverseMass + B->inverseMass + (raCrossN * raCrossN) * A->inverseInertia + (rbCrossN * rbCrossN) * B->inverseInertia;

		// Calculate impulse scalar
		float j = -(1.0f + e) * contactVel;
		j /= invMassSum;
		j /= (float)contact_count;

		// Apply impulse
		glm::vec2 impulse = normal * j;
		A->ApplyImpulse(-impulse, ra);
		B->ApplyImpulse(impulse, rb);

		// Friction impulse
		rv = B->velocity + glm::vec2(-B->angularVelocity * rb.y, B->angularVelocity * rb.x) -
			 A->velocity - glm::vec2(-A->angularVelocity * ra.y, A->angularVelocity * ra.x);

		glm::vec2 t = rv - (normal * glm::dot(rv, normal));
		t = glm::normalize(t);

		// j tangent magnitude
		float jt = -glm::dot(rv, t);
		jt /= invMassSum;
		jt /= (float)contact_count;

		// Don't apply tiny friction impulses
		if (glm::abs(jt - 0.0f) <= EPSILON)
			return;

		// Coulumb's law
		glm::vec2 tangentImpulse;
		if (glm::abs(jt) < j * sf)
			tangentImpulse = t * jt;
		else
			tangentImpulse = t * -j * df;

		// Apply friction impulse
		A->ApplyImpulse(-tangentImpulse, ra);
		B->ApplyImpulse(tangentImpulse, rb);
	}
}

void Manifold::PositionalCorrection(void)
{
	const float k_slop = 0.05f; // Penetration allowance
	const float percent = 0.4f; // Penetration percentage to correct
	glm::vec2 correction = (glm::max(penetration - k_slop, 0.0f) / (A->inverseMass + B->inverseMass)) * normal * percent;
	A->position -= correction * A->inverseMass;
	B->position += correction * B->inverseMass;
}

void Manifold::InfiniteMassCorrection(void)
{
	A->velocity = glm::vec2(0.0f);
	B->velocity = glm::vec2(0.0f);
}