/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "b2Body.h"
#include "b2World.h"
#include "Joints/b2Joint.h"
#include "Contacts/b2Contact.h"
#include "../Collision/b2Shape.h"

b2Body::b2Body(const b2BodyDef* bd, b2World* world)
{
	m_flags = 0;
	if (bd->isFast)
	{
		m_flags |= e_fastFlag;
	}
	m_position = bd->position;
	m_rotation = bd->rotation;
	m_R.Set(m_rotation);
	m_position0 = m_position;
	m_rotation0 = m_rotation;
	m_world = world;

	m_linearDamping = b2Clamp(1.0f - bd->linearDamping, 0.0f, 1.0f);
	m_angularDamping = b2Clamp(1.0f - bd->angularDamping, 0.0f, 1.0f);

	m_force.Set(0.0f, 0.0f);
	m_torque = 0.0f;

	m_mass = 0.0f;

	b2MassData massDatas[b2_maxShapesPerBody];

	// Compute the shape mass properties, the bodies total mass and COM.
	m_shapeCount = 0;
	m_center.Set(0.0f, 0.0f);
	for (int32 i = 0; i < b2_maxShapesPerBody; ++i)
	{
		const b2ShapeDef* sd = bd->shapes[i];
		if (sd == NULL) break;
		b2MassData* massData = massDatas + i;
		sd->ComputeMass(massData);
		m_mass += massData->mass;
		m_center += massData->mass * (sd->localPosition + massData->center);
		++m_shapeCount;
	}

	// Compute center of mass, and shift the origin to the COM.
	if (m_mass > 0.0f)
	{
		m_center *= 1.0f / m_mass;
		m_position += b2Mul(m_R, m_center);
	}
	else
	{
		m_flags |= e_staticFlag;
	}

	// Compute the moment of inertia.
	m_I = 0.0f;
	for (int32 i = 0; i < m_shapeCount; ++i)
	{
		const b2ShapeDef* sd = bd->shapes[i];
		b2MassData* massData = massDatas + i;
		m_I += massData->I;
		b2Vec2 r = sd->localPosition + massData->center - m_center;
		m_I += massData->mass * b2Dot(r, r);
	}

	if (m_mass > 0.0f)
	{
		m_invMass = 1.0f / m_mass;
	}
	else
	{
		m_invMass = 0.0f;
	}

	if (m_I > 0.0f && bd->preventRotation == false)
	{
		m_invI = 1.0f / m_I;
	}
	else
	{
		m_I = 0.0f;
		m_invI = 0.0f;
	}

	// Compute the center of mass velocity.
	m_linearVelocity = bd->linearVelocity + b2Cross(bd->angularVelocity, m_center);
	m_angularVelocity = bd->angularVelocity;

	m_jointList = NULL;
	m_contactList = NULL;
	m_prev = NULL;
	m_next = NULL;

	// Create the shapes.
	m_shapeList = NULL;
	for (int32 i = 0; i < m_shapeCount; ++i)
	{
		const b2ShapeDef* sd = bd->shapes[i];
		b2Shape* shape = b2Shape::Create(sd, this, m_center);
		shape->m_next = m_shapeList;
		m_shapeList = shape;
	}

	m_sleepTime = 0.0f;
	if (bd->allowSleep)
	{
		m_flags |= e_allowSleepFlag;
	}
	if (bd->isSleeping)
	{
		m_flags |= e_sleepFlag;
	}

	if ((m_flags & e_sleepFlag)  || m_invMass == 0.0f)
	{
		m_linearVelocity.Set(0.0f, 0.0f);
		m_angularVelocity = 0.0f;
	}

	m_userData = bd->userData;
}

b2Body::~b2Body()
{
	b2Shape* s = m_shapeList;
	while (s)
	{
		b2Shape* s0 = s;
		s = s->m_next;

		b2Shape::Destroy(s0);
	}
}

void b2Body::SetOriginPosition(const b2Vec2& position, float rotation)
{
	if (IsFrozen())
	{
		return;
	}

	m_rotation = rotation;
	m_R.Set(m_rotation);
	m_position = position + b2Mul(m_R, m_center);

	m_position0 = m_position;
	m_rotation0 = m_rotation;

	for (b2Shape* s = m_shapeList; s; s = s->m_next)
	{
		s->Synchronize(m_position, m_R, m_position, m_R);
	}

	m_world->m_broadPhase->Commit();
}

void b2Body::SetCenterPosition(const b2Vec2& position, float rotation)
{
	if (IsFrozen())
	{
		return;
	}

	m_rotation = rotation;
	m_R.Set(m_rotation);
	m_position = position;

	m_position0 = m_position;
	m_rotation0 = m_rotation;

	for (b2Shape* s = m_shapeList; s; s = s->m_next)
	{
		s->Synchronize(m_position, m_R, m_position, m_R);
	}

	m_world->m_broadPhase->Commit();
}

void b2Body::SynchronizeShapes()
{
	b2Mat22 R0(m_rotation0);
	for (b2Shape* s = m_shapeList; s; s = s->m_next)
	{
		s->Synchronize(m_position0, R0, m_position, m_R);
	}
}

void b2Body::QuickSyncShapes()
{
	for (b2Shape* s = m_shapeList; s; s = s->m_next)
	{
		s->QuickSync(m_position, m_R);
	}
}

void b2Body::Freeze()
{
	m_flags |= e_frozenFlag;
	m_linearVelocity.SetZero();
	m_angularVelocity = 0.0f;
	for (b2Shape* s = m_shapeList; s; s = s->m_next)
	{
		s->DestroyProxy();
	}
}
