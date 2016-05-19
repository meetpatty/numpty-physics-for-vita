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

#ifndef CONTACT_H
#define CONTACT_H

#include "../../Common/b2Math.h"
#include "../../Collision/b2Collision.h"
#include "../../Collision/b2Shape.h"

class b2Body;
class b2Contact;
class b2World;
class b2BlockAllocator;

typedef b2Contact* b2ContactCreateFcn(b2Shape* shape1, b2Shape* shape2, b2BlockAllocator* allocator);
typedef void b2ContactDestroyFcn(b2Contact* contact, b2BlockAllocator* allocator);

struct b2ContactNode
{
	b2Body* other;
	b2Contact* contact;
	b2ContactNode* prev;
	b2ContactNode* next;
};

struct b2ContactRegister
{
	b2ContactCreateFcn* createFcn;
	b2ContactDestroyFcn* destroyFcn;
	bool primary;
};

class b2Contact
{
public:
	virtual b2Manifold* GetManifolds() = 0;
	int32 GetManifoldCount() const
	{
		return m_manifoldCount;
	}

	b2Contact* GetNext();

	b2Shape* GetShape1();

	b2Shape* GetShape2();

	//--------------- Internals Below -------------------

	// m_flags
	enum
	{
		e_islandFlag		= 0x0001,
		e_destroyFlag		= 0x0002,
	};

	static void AddType(b2ContactCreateFcn* createFcn, b2ContactDestroyFcn* destroyFcn,
						b2ShapeType type1, b2ShapeType type2);
	static void InitializeRegisters();
	static b2Contact* Create(b2Shape* shape1, b2Shape* shape2, b2BlockAllocator* allocator);
	static void Destroy(b2Contact* contact, b2BlockAllocator* allocator);

	b2Contact() : m_shape1(NULL), m_shape2(NULL) {}
	b2Contact(b2Shape* shape1, b2Shape* shape2);
	virtual ~b2Contact() {}

	float32 ComputeTOI();
	virtual void Evaluate() = 0;
	static b2ContactRegister s_registers[e_shapeTypeCount][e_shapeTypeCount];
	static bool s_initialized;

	uint32 m_flags;

	// World pool and list pointers.
	b2Contact* m_prev;
	b2Contact* m_next;

	// Nodes for connecting bodies.
	b2ContactNode m_node1;
	b2ContactNode m_node2;

	b2Shape* m_shape1;
	b2Shape* m_shape2;

	int32 m_manifoldCount;

	// Combined friction
	float32 m_friction;
	float32 m_restitution;

	float32 m_toi;
};

inline b2Contact* b2Contact::GetNext()
{
	return m_next;
}

inline b2Shape* b2Contact::GetShape1()
{
	return m_shape1;
}

inline b2Shape* b2Contact::GetShape2()
{
	return m_shape2;
}

#endif
