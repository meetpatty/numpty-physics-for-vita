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

#ifndef B2_BODY_H
#define B2_BODY_H

#include "../Common/b2Math.h"
#include "../Dynamics/Joints/b2Joint.h"
#include "../Collision/b2Shape.h"

#include <memory>
#include <cstring>

class b2Joint;
class b2Contact;
class b2World;
struct b2JointNode;
struct b2ContactNode;

struct b2BodyDef
{
	b2BodyDef()
	{
		userData = NULL;
		memset(shapes, 0, sizeof(shapes));
		position.Set(0.0f, 0.0f);
		rotation = 0.0f;
		linearVelocity.Set(0.0f, 0.0f);
		angularVelocity = 0.0f;
		linearDamping = 0.0f;
		angularDamping = 0.0f;
		allowSleep = true;
		isSleeping = false;
		preventRotation = false;
		isFast = false;
	}

	void* userData;
	b2ShapeDef* shapes[b2_maxShapesPerBody];
	b2Vec2 position;
	float32 rotation;
	b2Vec2 linearVelocity;
	float32 angularVelocity;
	float32 linearDamping;
	float32 angularDamping;
	bool allowSleep;
	bool isSleeping;
	bool preventRotation;
	bool isFast;

	void AddShape(b2ShapeDef* shape);
};

// A rigid body. Internal computation are done in terms
// of the center of mass position. The center of mass may
// be offset from the body's origin.
class b2Body
{
public:
	// Set the position of the body's origin and rotation (radians).
	// This breaks any contacts and wakes the other bodies.
	void SetOriginPosition(const b2Vec2& position, float32 rotation);

	// Get the position of the body's origin. The body's origin does not
	// necessarily coincide with the center of mass. It depends on how the
	// shapes are created.
	b2Vec2 GetOriginPosition() const;

	// Set the position of the body's center of mass and rotation (radians).
	// This breaks any contacts and wakes the other bodies.
	void SetCenterPosition(const b2Vec2& position, float32 rotation);

	// Get the position of the body's center of mass. The body's center of mass
	// does not necessarily coincide with the body's origin. It depends on how the
	// shapes are created.
	b2Vec2 GetCenterPosition() const;

	// Get the rotation in radians.
	float32 GetRotation() const;

	const b2Mat22& GetRotationMatrix() const;

	// Set/Get the linear velocity of the center of mass.
	void SetLinearVelocity(const b2Vec2& v);
	b2Vec2 GetLinearVelocity() const;

	// Set/Get the angular velocity.
	void SetAngularVelocity(float32 w);
	float32 GetAngularVelocity() const;

	// Apply a force at a world point. Additive.
	void ApplyForce(const b2Vec2& force, const b2Vec2& point);

	// Apply a torque. Additive.
	void ApplyTorque(float32 torque);

	// Apply an impulse at a point. This immediately modifies the velocity.
	void ApplyImpulse(const b2Vec2& impulse, const b2Vec2& point);

	float32 GetMass() const;

	float32 GetInertia() const;

	// Get the world coordinates of a point give the local coordinates
	// relative to the body's center of mass.
	b2Vec2 GetWorldPoint(const b2Vec2& localPoint);

	// Get the world coordinates of a vector given the local coordinates.
	b2Vec2 GetWorldVector(const b2Vec2& localVector);

	// Returns a local point relative to the center of mass given a world point.
	b2Vec2 GetLocalPoint(const b2Vec2& worldPoint);

	// Returns a local vector given a world vector.
	b2Vec2 GetLocalVector(const b2Vec2& worldVector);

	// Is this body static (immovable)?
	bool IsStatic() const;

	// Is this body frozen?
	bool IsFrozen() const;

	// Is this body sleeping (not simulating).
	bool IsSleeping() const;

	// You can disable sleeping on this particular body.
	void AllowSleeping(bool flag);

	// Wake up this body so it will begin simulating.
	void WakeUp();

	// Get the list of all shapes attached to this body.
	b2Shape* GetShapeList();

	// Get the list of all contacts attached to this body.
	b2ContactNode* GetContactList();

	// Get the list of all joints attached to this body.
	b2JointNode* GetJointList();

	// Get the next body in the world's body list.
	b2Body* GetNext();

	void* GetUserData();

	//--------------- Internals Below -------------------

	// m_flags
	enum
	{
		e_staticFlag		= 0x0001,
		e_frozenFlag		= 0x0002,
		e_islandFlag		= 0x0004,
		e_sleepFlag			= 0x0008,
		e_allowSleepFlag	= 0x0010,
		e_destroyFlag		= 0x0020,
		e_fastFlag			= 0x0040,
		e_toiResolved		= 0x0080,
	};

	b2Body(const b2BodyDef* bd, b2World* world);
	~b2Body();

	void SynchronizeShapes();
	void QuickSyncShapes();

	// This is used to prevent connected bodies from colliding.
	// It may lie, depending on the collideConnected flag.
	bool IsConnected(const b2Body* other) const;

	// This is called when the child shape has no proxy.
	void Freeze();

	uint32 m_flags;

	b2Vec2 m_position;	// center of mass position
	float32 m_rotation;
	b2Mat22 m_R;

	// Conservative advancement data.
	float32 m_toi;
	b2Vec2 m_position0;
	float32 m_rotation0;

	b2Vec2 m_linearVelocity;
	float32 m_angularVelocity;

	b2Vec2 m_force;
	float32 m_torque;

	b2Vec2 m_center;	// local vector from client origin to center of mass

	b2World* m_world;
	b2Body* m_prev;
	b2Body* m_next;

	b2Shape* m_shapeList;
	int32 m_shapeCount;

	b2JointNode* m_jointList;
	b2ContactNode* m_contactList;

	float32 m_mass, m_invMass;
	float32 m_I, m_invI;

	float32 m_linearDamping;
	float32 m_angularDamping;

	float32 m_sleepTime;

	void* m_userData;
};

inline void b2BodyDef::AddShape(b2ShapeDef* shape)
{
	for (int32 i = 0; i < b2_maxShapesPerBody; ++i)
	{
		if (shapes[i] == NULL)
		{
			shapes[i] = shape;
			break;
		}
	}
}

inline b2Vec2 b2Body::GetOriginPosition() const
{
	return m_position - b2Mul(m_R, m_center);
}

inline b2Vec2 b2Body::GetCenterPosition() const
{
	return m_position;
}

inline float32 b2Body::GetRotation() const
{
	return m_rotation;
}

inline const b2Mat22& b2Body::GetRotationMatrix() const
{
	return m_R;
}

inline void b2Body::SetLinearVelocity(const b2Vec2& v)
{
	m_linearVelocity = v;
}

inline b2Vec2 b2Body::GetLinearVelocity() const
{
	return m_linearVelocity;
}

inline void b2Body::SetAngularVelocity(float32 w)
{
	m_angularVelocity = w;
}

inline float32 b2Body::GetAngularVelocity() const
{
	return m_angularVelocity;
}

inline void b2Body::ApplyForce(const b2Vec2& force, const b2Vec2& point)
{
	if (IsSleeping() == false)
	{
		m_force += force;
		m_torque += b2Cross(point - m_position, force);
	}
}

inline void b2Body::ApplyTorque(float32 torque)
{
	if (IsSleeping() == false)
	{
		m_torque += torque;
	}
}

inline void b2Body::ApplyImpulse(const b2Vec2& impulse, const b2Vec2& point)
{
	if (IsSleeping() == false)
	{
		m_linearVelocity += m_invMass * impulse;
		m_angularVelocity += m_invI * b2Cross(point - m_position, impulse);
	}
}

inline float32 b2Body::GetMass() const
{
	return m_mass;
}

inline float32 b2Body::GetInertia() const
{
	return m_I;
}

inline b2Vec2 b2Body::GetWorldPoint(const b2Vec2& localPoint)
{
	return m_position + b2Mul(m_R, localPoint);
}

inline b2Vec2 b2Body::GetWorldVector(const b2Vec2& localVector)
{
	return b2Mul(m_R, localVector);
}

inline b2Vec2 b2Body::GetLocalPoint(const b2Vec2& worldPoint)
{
	return b2MulT(m_R, worldPoint - m_position);
}

inline b2Vec2 b2Body::GetLocalVector(const b2Vec2& worldVector)
{
	return b2MulT(m_R, worldVector);
}

inline bool b2Body::IsStatic() const
{
	return (m_flags & e_staticFlag) == e_staticFlag;
}

inline bool b2Body::IsFrozen() const
{
	return (m_flags & e_frozenFlag) == e_frozenFlag;
}

inline bool b2Body::IsSleeping() const
{
	return (m_flags & e_sleepFlag) == e_sleepFlag;
}

inline void b2Body::AllowSleeping(bool flag)
{
	if (flag)
	{
		m_flags |= e_allowSleepFlag;
	}
	else
	{
		m_flags &= ~e_allowSleepFlag;
		WakeUp();
	}
}

inline void b2Body::WakeUp()
{
	m_flags &= ~e_sleepFlag;
	m_sleepTime = 0.0f;
}

inline b2Shape* b2Body::GetShapeList()
{
	return m_shapeList;
}

inline b2ContactNode* b2Body::GetContactList()
{
	return m_contactList;
}

inline b2JointNode* b2Body::GetJointList()
{
	return m_jointList;
}

inline b2Body* b2Body::GetNext()
{
	return m_next;
}

inline void* b2Body::GetUserData()
{
	return m_userData;
}

inline bool b2Body::IsConnected(const b2Body* other) const
{
	for (b2JointNode* jn = m_jointList; jn; jn = jn->next)
	{
		if (jn->other == other)
			return jn->joint->m_collideConnected == false;
	}

	return false;
}

#endif
