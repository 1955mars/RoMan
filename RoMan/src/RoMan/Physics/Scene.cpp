#include "rmpch.h"
#include "Physics.h"

void IntegrateForces(Body* b, float dt)
{
    if (b->inverseMass == 0.0f)
        return;

    b->velocity += (b->force * b->inverseMass + gravity) * (dt / 2.0f);
    b->angularVelocity += b->torque * b->inverseInertia * (dt / 2.0f);
}

void IntegrateVelocity(Body* b, float dt)
{
    if (b->inverseMass == 0.0f)
        return;

    b->position += b->velocity * dt;
    b->orient += b->angularVelocity * dt;
    b->SetOrient(b->orient);
    IntegrateForces(b, dt);
}

void Scene::Step(void)
{
    // Generate new collision info
    contacts.clear();
    for (glm::uint32 i = 0; i < bodies.size(); ++i)
    {
        Body* A = bodies[i];

        for (glm::uint32 j = i + 1; j < bodies.size(); ++j)
        {
            Body* B = bodies[j];
            if (A->inverseMass == 0 && B->inverseMass == 0)
                continue;
            Manifold m(A, B);
            m.Solve();
            if (m.contact_count)
                contacts.emplace_back(m);
        }
    }

    // Integrate forces
    for (glm::uint32 i = 0; i < bodies.size(); ++i)
        IntegrateForces(bodies[i], m_dt);

    // Initialize collision
    for (glm::uint32 i = 0; i < contacts.size(); ++i)
        contacts[i].Initialize();

    // Solve collisions
    for (glm::uint32 j = 0; j < m_iterations; ++j)
        for (glm::uint32 i = 0; i < contacts.size(); ++i)
            contacts[i].ApplyImpulse();

    // Integrate velocities
    for (glm::uint32 i = 0; i < bodies.size(); ++i)
        IntegrateVelocity(bodies[i], m_dt);

    // Correct positions
    for (glm::uint32 i = 0; i < contacts.size(); ++i)
        contacts[i].PositionalCorrection();

    // Clear all forces
    for (glm::uint32 i = 0; i < bodies.size(); ++i)
    {
        Body* b = bodies[i];
        b->force = glm::vec2(0.0f, 0.0f);
        b->torque = 0;
    }
}

void Scene::Render(void)
{
    for (glm::uint32 i = 0; i < bodies.size(); ++i)
    {
        Body* b = bodies[i];
        b->shape->Draw();
    }
}

Body* Scene::Add(Shape* shape, glm::uint32 x, glm::uint32 y)
{
    assert(shape);
    Body* b = new Body(shape, x, y);
    bodies.push_back(b);
    return b;
}