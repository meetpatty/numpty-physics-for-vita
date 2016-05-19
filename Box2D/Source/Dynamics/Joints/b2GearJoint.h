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

#ifndef B2_GEAR_JOINT_H
#define B2_GEAR_JOINT_H

#include "b2Joint.h"

class b2RevoluteJoint;
class b2PrismaticJoint;

// A gear joint is used to connect two joints together. Either joint
// can be a revolute or prismatic joint. You specify a gear ratio
// to bind the motions together:
// coordinate1 + ratio * coordinate2 = constant
// The ratio can be negative or positive. If one joint is a revolute joint
// and the other joint is a prismatic joint, then the ratio will have units
// of length or units of 1/length.
//
// RESTRICITON: The revolute and prismatic joints must be attached to
// a fixed body (which must be body1 on those joints).

struct b2GearJointDef : public b2JointDef
{
	b2GearJointDef()
	{
		type = e_gearJoint;
		joint1 = NULL;
		joint2 = NULL;
		ratio = 1.0f;
	}

	b2Joint* joint1;
	b2Joint* joint2;
	float32 ratio;
};

class b2GearJoint : public b2Joint
{
public:
	b2Vec2 GetAnchor1() const;
	b2Vec2 GetAnchor2() const;

	b2Vec2 GetReactionForce(float32 invTimeStep) const;
	float32 GetReactionTorque(float32 invTimeStep) const;

	float32 GetRatio() const;

	//--------------- Internals Below -------------------

	b2GearJoint(const b2GearJointDef* data);

	void InitVelocityConstraints();
	void SolveVelocityConstraints(const b2TimeStep& step);
	bool SolvePositionConstraints();

	b2Body* m_ground1;
	b2Body* m_ground2;

	// One of these is NULL.
	b2RevoluteJoint* m_revolute1;
	b2PrismaticJoint* m_prismatic1;

	// One of these is NULL.
	b2RevoluteJoint* m_revolute2;
	b2PrismaticJoint* m_prismatic2;

	b2Vec2 m_groundAnchor1;
	b2Vec2 m_groundAnchor2;

	b2Vec2 m_localAnchor1;
	b2Vec2 m_localAnchor2;

	b2Jacobian m_J;

	float32 m_constant;
	float32 m_ratio;

	// Effective mass
	float32 m_mass;

	// Impulse for accumulation/warm starting.
	float32 m_impulse;
};

#endif
