#include "rmpch.h"
#include "Physics.h"

CollisionCallback Dispatch[Shape::eCount][Shape::eCount] =
{
	{
		CircletoCircle, CircletoPolygon
	},
	{
		PolygontoCircle, PolygontoPolygon
	},
};

void CircletoCircle(Manifold* m, Body* a, Body* b)
{
	Circle* A = reinterpret_cast<Circle*>(a->shape);
	Circle* B = reinterpret_cast<Circle*>(b->shape);

	glm::vec2 normal = b->position - a->position;

	float dist_sqr = (normal.x * normal.x) + (normal.y * normal.y);
	float radius = A->radius + B->radius;

	if (dist_sqr >= radius * radius)
	{
		m->contact_count = 0;
		return;
	}

	float distance = glm::sqrt(dist_sqr);

	m->contact_count = 1;

	if (distance == 0.0f)
	{
		m->penetration = A->radius;
		m->normal = glm::vec2(1.0f, 0.0f);
		m->contacts[0] = a->position;
	}
	else
	{
		m->penetration = radius - distance;
		m->normal = normal / distance;
		m->contacts[0] = m->normal * A->radius + a->position;
	}
}

void CircletoPolygon(Manifold* m, Body* a, Body* b)
{
	Circle* A = reinterpret_cast<Circle*>(a->shape);
	PolygonShape* B = reinterpret_cast<PolygonShape*>(b->shape);

	m->contact_count = 0;

	glm::vec2 center = a->position;
	center = glm::transpose(B->u) * (center - b->position);

	float separation = -FLT_MAX;
	glm::uint32 faceNormal = 0;
	for (glm::uint32 i = 0; i < B->m_vertexCount; ++i)
	{
		float s = glm::dot(B->m_normals[i], center - B->m_vertices[i]);

		if (s > A->radius)
			return;

		if (s > separation)
		{
			separation = s;
			faceNormal = i;
		}
	}

	glm::vec2 v1 = B->m_vertices[faceNormal];
	glm::uint32 i2 = faceNormal + 1 < B->m_vertexCount ? faceNormal + 1 : 0;
	glm::vec2 v2 = B->m_vertices[i2];

	if (separation < EPSILON)
	{
		m->contact_count = 1;
		m->normal = -(B->u * B->m_normals[faceNormal]);
		m->contacts[0] = m->normal * A->radius + a->position;
		m->penetration = A->radius;
		return;
	}

	float dot1 = glm::dot(center - v1, v2 - v1);
	float dot2 = glm::dot(center - v2, v1 - v2);
	m->penetration = A->radius - separation;

	if (dot1 <= 0.0f)
	{
		if (glm::dot(center - v1, center - v1) > A->radius* A->radius)
			return;

		m->contact_count = 1;
		glm::vec2 n = v1 - center;
		n = B->u * n;
		n = glm::normalize(n);
		m->normal = n;
		v1 = B->u * v1 + b->position;
		m->contacts[0] = v1;
	}
	else if (dot2 <= 0.0f)
	{
		if (glm::dot(center - v2, center - v2) > A->radius* A->radius)
			return;

		m->contact_count = 1;
		glm::vec2 n = v2 - center;
		v2 = B->u * v2 + b->position;
		m->contacts[0] = v2;
		n = B->u * n;
		n = glm::normalize(n);
		m->normal = n;
	}
	else
	{
		glm::vec2 n = B->m_normals[faceNormal];
		if (glm::dot(center - v1, n) > A->radius)
			return;

		n = B->u * n;
		m->normal = -n;
		m->contacts[0] = m->normal * A->radius + a->position;
		m->contact_count = 1;
	}
}

void PolygontoCircle(Manifold* m, Body* a, Body* b)
{
	CircletoPolygon(m, b, a);
	m->normal = -m->normal;
}

float FindAxisLeastPenetration(glm::uint32* faceIndex, PolygonShape* A, PolygonShape* B)
{
	float bestDistance = -FLT_MAX;
	glm::uint32 bestIndex;

	for (glm::uint32 i = 0; i < A->m_vertexCount; ++i)
	{
		glm::vec2 n = A->m_normals[i];
		glm::vec2 nw = A->u * n;

		glm::mat2 buT = glm::transpose(B->u);
		n = buT * nw;

		glm::vec2 s = B->GetSupport(-n);

		glm::vec2 v = A->m_vertices[i];
		v = A->u * v + A->body->position;
		v -= B->body->position;
		v = buT * v;

		float d = glm::dot(n, s - v);

		if (d > bestDistance)
		{
			bestDistance = d;
			bestIndex = i;
		}
	}

	*faceIndex = bestIndex;
	return bestDistance;
}

void FindIncidentFace(glm::vec2* v, PolygonShape* RefPoly, PolygonShape* IncPoly, glm::uint32 referenceIndex)
{
	glm::vec2 referenceNormal = RefPoly->m_normals[referenceIndex];

	referenceNormal = RefPoly->u * referenceNormal;
	referenceNormal = glm::transpose(IncPoly->u) * referenceNormal;

	glm::int32 incidentFace = 0;
	float minDot = FLT_MAX;
	for (glm::uint32 i = 0; i < IncPoly->m_vertexCount; ++i)
	{
		float dot = glm::dot(referenceNormal, IncPoly->m_normals[i]);
		if (dot < minDot)
		{
			minDot = dot;
			incidentFace = i;
		}
	}

	v[0] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->body->position;
	incidentFace = incidentFace + 1 >= (glm::int32)IncPoly->m_vertexCount ? 0 : incidentFace + 1;
	v[1] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->body->position;
}

glm::int32 Clip(glm::vec2 n, float c, glm::vec2* face)
{
	glm::uint32 sp = 0;
	glm::vec2 out[2] = {
	  face[0],
	  face[1]
	};

	float d1 = glm::dot(n, face[0]) - c;
	float d2 = glm::dot(n, face[1]) - c;

	if (d1 <= 0.0f) out[sp++] = face[0];
	if (d2 <= 0.0f) out[sp++] = face[1];

	if (d1 * d2 < 0.0f)
	{
		float alpha = d1 / (d1 - d2);
		out[sp] = face[0] + alpha * (face[1] - face[0]);
		++sp;
	}

	face[0] = out[0];
	face[1] = out[1];

	assert(sp != 3);

	return sp;
}

void PolygontoPolygon(Manifold* m, Body* a, Body* b)
{
	PolygonShape* A = reinterpret_cast<PolygonShape*>(a->shape);
	PolygonShape* B = reinterpret_cast<PolygonShape*>(b->shape);
	m->contact_count = 0;

	glm::uint32 faceA;
	float penetrationA = FindAxisLeastPenetration(&faceA, A, B);
	if (penetrationA >= 0.0f)
		return;

	glm::uint32 faceB;
	float penetrationB = FindAxisLeastPenetration(&faceB, B, A);
	if (penetrationB >= 0.0f)
		return;

	glm::uint32 referenceIndex;
	bool flip;

	PolygonShape* RefPoly;
	PolygonShape* IncPoly;

	const float k_biasRelative = 0.95f;
	const float k_biasAbsolute = 0.01f;

	if (penetrationA >= penetrationB * k_biasRelative + penetrationA * k_biasAbsolute)
	{
		RefPoly = A;
		IncPoly = B;
		referenceIndex = faceA;
		flip = false;
	}

	else
	{
		RefPoly = B;
		IncPoly = A;
		referenceIndex = faceB;
		flip = true;
	}

	// World space incident face
	glm::vec2 incidentFace[2];
	FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);

	//        y
	//        ^  ->n       ^
	//      +---c ------posPlane--
	//  x < | i |\
    //      +---+ c-----negPlane--
	//             \       v
	//              r
	//
	//  r : reference face
	//  i : incident poly
	//  c : clipped point
	//  n : incident normal

	// Setup reference face vertices
	glm::vec2 v1 = RefPoly->m_vertices[referenceIndex];
	referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
	glm::vec2 v2 = RefPoly->m_vertices[referenceIndex];

	// Transform vertices to world space
	v1 = RefPoly->u * v1 + RefPoly->body->position;
	v2 = RefPoly->u * v2 + RefPoly->body->position;

	// Calculate reference face side normal in world space
	glm::vec2 sidePlaneNormal = (v2 - v1);
	sidePlaneNormal = glm::normalize(sidePlaneNormal);

	// Orthogonalize
	glm::vec2 refFaceNormal(sidePlaneNormal.y, -sidePlaneNormal.x);

	// ax + by = c
	// c is distance from origin
	float refC = glm::dot(refFaceNormal, v1);
	float negSide = -glm::dot(sidePlaneNormal, v1);
	float posSide = glm::dot(sidePlaneNormal, v2);

	// Clip incident face to reference face side planes
	if (Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
		return; // Due to floating point error, possible to not have required points

	if (Clip(sidePlaneNormal, posSide, incidentFace) < 2)
		return; // Due to floating point error, possible to not have required points

	  // Flip
	m->normal = flip ? -refFaceNormal : refFaceNormal;

	// Keep points behind reference face
	glm::uint32 cp = 0; // clipped points behind reference face
	float separation = glm::dot(refFaceNormal, incidentFace[0]) - refC;
	if (separation <= 0.0f)
	{
		m->contacts[cp] = incidentFace[0];
		m->penetration = -separation;
		++cp;
	}
	else
		m->penetration = 0;

	separation = glm::dot(refFaceNormal, incidentFace[1]) - refC;
	if (separation <= 0.0f)
	{
		m->contacts[cp] = incidentFace[1];

		m->penetration += -separation;
		++cp;

		// Average penetration
		m->penetration /= (float)cp;
	}

	m->contact_count = cp;
}