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

#include "b2ContactManager.h"
#include "b2World.h"
#include "b2Body.h"

// This is a callback from the broadphase when two AABB proxies begin
// to overlap. We create a b2Contact to manage the narrow phase.
void* b2ContactManager::PairAdded(void* proxyUserData1, void* proxyUserData2)
{
	b2Shape* shape1 = (b2Shape*)proxyUserData1;
	b2Shape* shape2 = (b2Shape*)proxyUserData2;

	b2Body* body1 = shape1->m_body;
	b2Body* body2 = shape2->m_body;

	if (body1->IsStatic() && body2->IsStatic())
	{
		return &m_nullContact;
	}

	if (shape1->m_body == shape2->m_body)
	{
		return &m_nullContact;
	}

	if (body2->IsConnected(body1))
	{
		return &m_nullContact;
	}

	if (m_world->m_filter != NULL && m_world->m_filter->ShouldCollide(shape1, shape2) == false)
	{
		return &m_nullContact;
	}

	// Ensure that body2 is dynamic (body1 is static or dynamic).
	if (body2->m_invMass == 0.0f)
	{
		b2Swap(shape1, shape2);
		b2Swap(body1, body2);
	}

	// Call the factory.
	b2Contact* contact = b2Contact::Create(shape1, shape2, &m_world->m_blockAllocator);

	if (contact == NULL)
	{
		return &m_nullContact;
	}
	else
	{
		// Insert into the world.
		contact->m_prev = NULL;
		contact->m_next = m_world->m_contactList;
		if (m_world->m_contactList != NULL)
		{
			m_world->m_contactList->m_prev = contact;
		}
		m_world->m_contactList = contact;
		++m_world->m_contactCount;
	}

	return contact;
}

// This is a callback from the broadphase when two AABB proxies cease
// to overlap. We destroy the b2Contact.
void b2ContactManager::PairRemoved(void* proxyUserData1, void* proxyUserData2, void* pairUserData)
{
	NOT_USED(proxyUserData1);
	NOT_USED(proxyUserData2);

	if (pairUserData == NULL)
	{
		return;
	}

	b2Contact* c = (b2Contact*)pairUserData;
	if (c != &m_nullContact)
	{
		b2Assert(m_world->m_contactCount > 0);

		if (m_destroyImmediate == true)
		{
			DestroyContact(c);
			c = NULL;
		}
		else
		{
			c->m_flags |= b2Contact::e_destroyFlag;
		}
	}
}

void b2ContactManager::DestroyContact(b2Contact* c)
{
	b2Assert(m_world->m_contactCount > 0);

	// Remove from the world.
	if (c->m_prev)
	{
		c->m_prev->m_next = c->m_next;
	}

	if (c->m_next)
	{
		c->m_next->m_prev = c->m_prev;
	}

	if (c == m_world->m_contactList)
	{
		m_world->m_contactList = c->m_next;
	}

	// If there are contact points, then disconnect from the island graph.
	if (c->GetManifoldCount() > 0)
	{
		b2Body* body1 = c->m_shape1->m_body;
		b2Body* body2 = c->m_shape2->m_body;

		// Wake up touching bodies.
		body1->WakeUp();
		body2->WakeUp();

		// Remove from body 1
		if (c->m_node1.prev)
		{
			c->m_node1.prev->next = c->m_node1.next;
		}

		if (c->m_node1.next)
		{
			c->m_node1.next->prev = c->m_node1.prev;
		}

		if (&c->m_node1 == body1->m_contactList)
		{
			body1->m_contactList = c->m_node1.next;
		}

		c->m_node1.prev = NULL;
		c->m_node1.next = NULL;

		// Remove from body 2
		if (c->m_node2.prev)
		{
			c->m_node2.prev->next = c->m_node2.next;
		}

		if (c->m_node2.next)
		{
			c->m_node2.next->prev = c->m_node2.prev;
		}

		if (&c->m_node2 == body2->m_contactList)
		{
			body2->m_contactList = c->m_node2.next;
		}

		c->m_node2.prev = NULL;
		c->m_node2.next = NULL;
	}

	// Call the factory.
	b2Contact::Destroy(c, &m_world->m_blockAllocator);
	--m_world->m_contactCount;
}

// Destroy any contacts marked for deferred destruction.
void b2ContactManager::CleanContactList()
{
	b2Contact* c = m_world->m_contactList;
	while (c != NULL)
	{
		b2Contact* c0 = c;
		c = c->m_next;

		if (c0->m_flags & b2Contact::e_destroyFlag)
		{
			DestroyContact(c0);
			c0 = NULL;
		}
	}
}

// This is the top level collision call for the time step. Here
// all the narrow phase collision is processed for the world
// contact list.
void b2ContactManager::Collide(const b2TimeStep& step)
{
	if (step.dt > 0.0f && b2World::s_enablePositionCorrection)
	{
		for (b2Body* b = m_world->m_bodyList; b; b = b->m_next)
		{
			b->m_toi = 1.0f;

			if (b->IsSleeping())
			{
				b->m_flags |= b2Body::e_toiResolved;
			}
			else
			{
				b->m_flags &= ~b2Body::e_toiResolved;
			}
		}

		bool found = true;
		while (found)
		{
			found = false;
			float32 minTOI = 1.0f;
			b2Contact* toiContact = NULL;
			for (b2Contact* c = m_world->m_contactList; c; c = c->m_next)
			{
				if (c->m_shape1->m_body->IsSleeping() &&
					c->m_shape2->m_body->IsSleeping())
				{
					continue;
				}

				float32 toi = c->ComputeTOI();
				if (toi < minTOI)
				{
					minTOI = toi;
					toiContact = c;
					found = true;
				}
			}

			if (toiContact)
			{
				toiContact->m_shape1->m_body->m_flags |= b2Body::e_toiResolved;
				toiContact->m_shape2->m_body->m_flags |= b2Body::e_toiResolved;
			}
		}

		for (b2Body* b = m_world->m_bodyList; b; b = b->m_next)
		{
			if (b->IsSleeping() || b->IsFrozen())
			{
				continue;
			}

			float32 toi = b->m_toi;
			b->m_position = (1.0f - toi) * b->m_position0 + toi * b->m_position;
			b->m_rotation = (1.0f - toi) * b->m_rotation0 + toi * b->m_rotation;
			b->m_R.Set(b->m_rotation);
			b->QuickSyncShapes();
		}
	}

	for (b2Contact* c = m_world->m_contactList; c; c = c->m_next)
	{
		if (c->m_shape1->m_body->IsSleeping() &&
			c->m_shape2->m_body->IsSleeping())
		{
			continue;
		}

		int32 oldCount = c->GetManifoldCount();
		c->Evaluate();

		int32 newCount = c->GetManifoldCount();

		if (oldCount == 0 && newCount > 0)
		{
			b2Assert(c->GetManifolds()->pointCount > 0);

			// Connect to island graph.
			b2Body* body1 = c->m_shape1->m_body;
			b2Body* body2 = c->m_shape2->m_body;

			// Connect to body 1
			c->m_node1.contact = c;
			c->m_node1.other = body2;

			c->m_node1.prev = NULL;
			c->m_node1.next = body1->m_contactList;
			if (c->m_node1.next != NULL)
			{
				c->m_node1.next->prev = &c->m_node1;
			}
			body1->m_contactList = &c->m_node1;

			// Connect to body 2
			c->m_node2.contact = c;
			c->m_node2.other = body1;

			c->m_node2.prev = NULL;
			c->m_node2.next = body2->m_contactList;
			if (c->m_node2.next != NULL)
			{
				c->m_node2.next->prev = &c->m_node2;
			}
			body2->m_contactList = &c->m_node2;
		}
		else if (oldCount > 0 && newCount == 0)
		{
			// Disconnect from island graph.
			b2Body* body1 = c->m_shape1->m_body;
			b2Body* body2 = c->m_shape2->m_body;

			// Remove from body 1
			if (c->m_node1.prev)
			{
				c->m_node1.prev->next = c->m_node1.next;
			}

			if (c->m_node1.next)
			{
				c->m_node1.next->prev = c->m_node1.prev;
			}

			if (&c->m_node1 == body1->m_contactList)
			{
				body1->m_contactList = c->m_node1.next;
			}

			c->m_node1.prev = NULL;
			c->m_node1.next = NULL;

			// Remove from body 2
			if (c->m_node2.prev)
			{
				c->m_node2.prev->next = c->m_node2.next;
			}

			if (c->m_node2.next)
			{
				c->m_node2.next->prev = c->m_node2.prev;
			}

			if (&c->m_node2 == body2->m_contactList)
			{
				body2->m_contactList = c->m_node2.next;
			}

			c->m_node2.prev = NULL;
			c->m_node2.next = NULL;
		}
	}
}