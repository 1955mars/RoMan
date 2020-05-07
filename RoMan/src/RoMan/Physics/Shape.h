#pragma once

#include "glm/glm.hpp"
#include "rmpch.h"
#include "Physics.h"
#include "RoMan/Physics/Body.h"


#define MaxPolyVertexCount 64

const float PI = 3.141592741f;
const float EPSILON = 0.0001f; // To check against small values
const float dt = 1.0f / 60.0f; // Time increment
const float gravityScale = -5.0f; // Strength of gravity
const glm::vec2 gravity = glm::vec2(0.0f, 10.0f * gravityScale); // Gravity

struct Shape
{
	enum Type
	{
		eCircle,
		ePoly,
		eCount
	};

	Shape() {};
	virtual Shape* Clone(void) const = 0;
	virtual void Initialize(void) = 0;
	virtual void ComputeMass(float density) = 0;
	virtual void SetOrient(float radians) = 0;
	virtual void Draw(void) const = 0;
	virtual Type GetType(void) const = 0;

	Body* body;

	float radius; // For circle
	
	glm::mat2 u; // For polygon

	// Keep track of our indices/vertices so we can add them to the vertex array
	uint32_t indexCount; // Currently not used as dynamic index buffer is not implemented
	uint32_t vertexCount;

	float* vertices;
	uint32_t* indices;
};

struct Circle : public Shape
{
	Circle(float r)
	{
		radius = r;
	}

	Shape* Clone(void) const
	{
		return new Circle(radius);
	}

	void Initialize(void)
	{
		ComputeMass(1.0f);

		// Generate initial vertices
		const glm::uint32 segments = 20;
		float theta = body->orient;
		float increment = PI * 2.0f / (float)segments;

		indices = circleIndices;
		indexCount = 60;
		vertexCount = 3 * 21;

		vertices = new float[vertexCount];
		vertices[0] = vertices[1] = vertices[2] = 0.0f;

		for (glm::uint32 i = 0; i < segments; i++)
		{
			theta += increment;
			glm::vec2 point(cos(theta), sin(theta));

			point *= radius;

			vertices[i * 3 + 3] = point.x;
			vertices[i * 3 + 4] = point.y;
			vertices[i * 3 + 5] = 0.0f;
		}
	}

	void ComputeMass(float density)
	{
		body->mass = PI * radius * radius * density;
		body->inverseMass = (body->mass) ? 1.0f / body->mass : 0.0f;
		body->inertia = body->mass * radius * radius;
		body->inverseInertia = (body->inertia) ? 1.0f / body->inertia : 0.0f;
	}

	void SetOrient(float radians) {}

	void Draw(void) const 
	{
		// Update vertices with new position
		const glm::uint32 segments = 20;
		float theta = body->orient;
		float increment = PI * 2.0f / (float)segments;

		vertices[0] = vertices[1] = vertices[2] = 0.0f;

		for (glm::uint32 i = 0; i < segments; i++)
		{
			theta += increment;
			glm::vec2 point(cos(theta), sin(theta));

			point *= radius;

			vertices[i * 3 + 3] = point.x;
			vertices[i * 3 + 4] = point.y;
			vertices[i * 3 + 5] = 0.0f;
		}
	}

	Type GetType(void) const
	{
		return eCircle;
	}

	// Hard coded indices for RoMaN engine, currently not implemented
	uint32_t circleIndices[60] = { 0, 1, 2, 2, 3, 0,
								   0, 3, 4, 4, 5, 0,
								   0, 5, 6, 6, 7, 0,
								   0, 7, 8, 8, 9, 0,
								   0, 9, 10, 10, 11, 0,
								   0, 11, 12, 12, 13, 0,
								   0, 13, 14, 14, 15, 0,
								   0, 15, 16, 16, 17, 0,
								   0, 17, 18, 18, 19, 0,
								   0, 19, 20, 20, 1, 0 };
};

struct PolygonShape : public Shape
{
	void Initialize(void)
	{
		ComputeMass(1.0f);
		RM_INFO("Setting vertex count");
		vertexCount = 3 * 4;
		vertices = new float[vertexCount];
	}

	Shape* Clone(void) const
	{
		PolygonShape* poly = new PolygonShape();
		poly->u = u;
		for (glm::uint32 i = 0; i < m_vertexCount; ++i)
		{
			poly->m_vertices[i] = m_vertices[i];
			poly->m_normals[i] = m_normals[i];
		}
		poly->m_vertexCount = m_vertexCount;
		return poly;
	}

	void ComputeMass(float density)
	{
		// Centroid and moment of inertia
		glm::vec2 centroid = glm::vec2(0.0f, 0.0f);
		float area = 0.0f;
		float inertia = 0.0f;
		const float k_inv3 = 1.0f / 3.0f;

		for (glm::uint32 i1 = 0; i1 < m_vertexCount; ++i1)
		{
			// Third vertex of triangle is assumed to be (0, 0)
			glm::vec2 p1 = m_vertices[i1];
			glm::uint32 i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
			glm::vec2 p2 = m_vertices[i2];

			float D = glm::cross(glm::vec3(p1, 0.0f), glm::vec3(p2, 0.0f)).z;
			float triangleArea = 0.5f * D;

			area += triangleArea;

			// Use area to weight the centroid average, not just vertex position
			centroid += triangleArea * k_inv3 * (p1 + p2);

			float intx2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
			float inty2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;
			inertia += (0.25f * k_inv3 * D) * (intx2 + inty2);
		}

		centroid *= 1.0f / area;

		for (glm::uint32 i = 0; i < m_vertexCount; ++i)
			m_vertices[i] -= centroid;

		body->mass = density * area;
		body->inverseMass = (body->mass) ? 1.0f / body->mass : 0.0f;
		body->inertia = inertia * density;
		body->inverseInertia = body->inertia ? 1.0f / body->inertia : 0.0f;
	}

	void SetOrient(float radians)
	{
		float c = std::cos(radians);
		float s = std::sin(radians);

		u = glm::mat2(c, -s, s, c);
	}

	void Draw(void) const
	{
		for (int i = 0; i < vertexCount; i++)
		{
			glm::vec2 point = m_vertices[i] * u;

			vertices[i * 3] = point.x;
			vertices[i * 3 + 1] = point.y;
			vertices[i * 3 + 2] = 0.0f;
		}
	}

	void SetBox(float hw, float hh)
	{
		m_vertexCount = 4;
		m_vertices[0] = glm::vec2(-hw, -hh);
		m_vertices[1] = glm::vec2(hw, -hh);
		m_vertices[2] = glm::vec2(hw, hh);
		m_vertices[3] = glm::vec2(-hw, hh);
		m_normals[0] = glm::vec2(0.0f, -1.0f);
		m_normals[1] = glm::vec2(1.0f, 0.0f);
		m_normals[2] = glm::vec2(0.0f, 1.0f);
		m_normals[3] = glm::vec2(-1.0f, 0.0f);
	}

	void Set(glm::vec2* vertices, glm::uint32 count)
	{
		// Make sure there is at least 3 vertices
		assert(count > 2 && count <= MaxPolyVertexCount);
		count = glm::min((glm::int32)count, MaxPolyVertexCount);

		glm::int32 rightMost = 0;
		float highestXCoord = vertices[0].x;
		for (glm::uint32 i = 1; i < count; ++i)
		{
			float x = vertices[i].x;

			if (x > highestXCoord)
			{
				highestXCoord = x;
				rightMost = i;
			}
			else if (x == highestXCoord)
			{
				if (vertices[i].y < vertices[rightMost].y)
					rightMost = i;
			}
		}

		glm::int32 hull[MaxPolyVertexCount];
		glm::int32 outCount = 0;
		glm::int32 indexHull = rightMost;

		for (;;)
		{
			hull[outCount] = indexHull;

			glm::int32 nextHullIndex = 0;
			for (glm::int32 i = 1; i < (glm::int32)count; ++i)
			{
				if (nextHullIndex == indexHull)
				{
					nextHullIndex = i;
					continue;
				}

				glm::vec2 e1 = vertices[nextHullIndex] - vertices[hull[outCount]];
				glm::vec2 e2 = vertices[i] - vertices[hull[outCount]];
				float c = glm::cross(glm::vec3(e1, 0.0f), glm::vec3(e2, 0.0f)).z;
				if (c < 0.0f)
					nextHullIndex = i;

				float l1 = e1.x * e1.x + e1.y * e1.y;
				float l2 = e2.x * e2.x + e2.y * e2.y;

				if (c == 0.0f && l2 > l1)
					nextHullIndex = i;
			}

			++outCount;
			indexHull = nextHullIndex;

			if (nextHullIndex == rightMost)
			{
				m_vertexCount = outCount;
				break;
			}
		}

		// Copy vertices into shape's vertices
		for (glm::uint32 i = 0; i < m_vertexCount; ++i)
			m_vertices[i] = vertices[hull[i]];

		// Compute face normals
		for (glm::uint32 i1 = 0; i1 < m_vertexCount; ++i1)
		{
			glm::uint32 i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
			glm::vec2 face = m_vertices[i2] - m_vertices[i1];

			float lf = face.x * face.x + face.y * face.y;

			// Make sure edge isn't length 0
			assert(lf > EPSILON * EPSILON);

			// Calculate normal with 2D cross product between vector and scalar
			m_normals[i1] = glm::vec2(face.y, -face.x);
			m_normals[i1] = glm::normalize(m_normals[i1]);
		}
	}

	// The extreme point along a direction within a polygon
	glm::vec2 GetSupport(const glm::vec2& dir)
	{
		float bestProjection = -FLT_MAX;
		glm::vec2 bestVertex = glm::vec2();

		for (glm::uint32 i = 0; i < m_vertexCount; ++i)
		{
			glm::vec2 v = m_vertices[i];
			float projection = glm::dot(v, dir);

			if (projection > bestProjection)
			{
				bestVertex = v;
				bestProjection = projection;
			}
		}

		return bestVertex;
	}

	Type GetType(void) const
	{
		return ePoly;
	}

	glm::uint32 m_vertexCount;
	glm::vec2 m_vertices[MaxPolyVertexCount];
	glm::vec2 m_normals[MaxPolyVertexCount];
};

