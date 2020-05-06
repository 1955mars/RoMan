#pragma once

struct Scene
{
    Scene(float dt, glm::uint32 iterations)
        : m_dt(dt)
        , m_iterations(iterations)
    {
    }

    void Step(void);
    void Render(void);
    Body* Add(Shape* shape, glm::uint32 x, glm::uint32 y);
    void Clear(void);

    float m_dt;
    glm::uint32 m_iterations;
    std::vector<Body*> bodies;
    std::vector<Manifold> contacts;
};