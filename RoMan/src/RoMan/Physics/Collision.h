#pragma once

#include "Shape.h"

struct Manifold;
struct Body;

typedef void (*CollisionCallback)(Manifold* m, Body* a, Body* b);

extern CollisionCallback Dispatch[Shape::eCount][Shape::eCount];

void CircletoCircle(Manifold* m, Body* a, Body* b);
void CircletoPolygon(Manifold* m, Body* a, Body* b);
void PolygontoCircle(Manifold* m, Body* a, Body* b);
void PolygontoPolygon(Manifold* m, Body* a, Body* b);