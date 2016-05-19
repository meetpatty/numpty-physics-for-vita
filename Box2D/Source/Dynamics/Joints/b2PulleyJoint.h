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

#ifndef B2_PULLEY_JOINT_H
#define B2_PULLEY_JOINT_H

#include "b2Joint.h"

// The pulley joint is connected to two bodies and two fixed ground points.
// The pulley supports a ratio such that:
// length1 + ratio * length2 = constant
// Yes, the force transmitted is scaled by the ratio.
// The pulley also enforces a maximum length limit on both sides. This is
// useful to prevent one side of the pulley hitting the top.

// We need a minimum pulley length to help prevent one side going to zero.
const float32 b2_minPulleyLength = b2_lengthUnitsPerMeter;

struct b2PulleyJointDef : public b2JointDef
{
	b2PulleyJointDef()
	{
		type = e_pulleyJoint;
		groundPoint1.Set(-1.0f, 1.0f);
		groundPoint2.Set(1.0f, 1.0f);
		anchorPoint1.Set(-1.0f, 0.0f);
		anchorPoint2.Set(1.0f, 0.0f);
		maxLength1 = 0.5f * b2_minPulleyLength;
		maxLength2 = 0.5f * b2_minPulleyLength;
		ratio = 1.0f;
		collideConnected = true;
	}

	b2Vec2 groundPoint1;
	b2Vec2 groundPoint2;
	b2Vec2 anchorPoint1;
	b2Vec2 anchorPoint2;
	float32 maxLength1;
	float32 maxLength2;
	float32 ratio;
};

class b2PulleyJoint : public b2Joint
{
public:
	b2Vec2 GetAnchor1() const;
	b2Vec2 GetAnchor2() const;

	b2Vec2 GetGroundPoint1() const;
	b2Vec2 GetGroundPoint2() const;

	b2Vec2 GetReactionForce(float32 invTimeStep) const;
	float32 GetReactionTorque(float32 invTimeStep) const;

	float32 GetLength1() const;
	float32 GetLength2() const;

	float32 GetRatio() const;

	//--------------- Internals Below -------------------

	b2PulleyJoint(const b2PulleyJointDef* data);

	void InitVelocityConstraints();
	void SolveVelocityConstraints(const b2TimeStep& step);
	bool SolvePositionConstraints();

	b2Body* m_ground;
	b2Vec2 m_groundAnchor1;
	b2Vec2 m_groundAnchor2;
	b2Vec2 m_localAnchor1;
	b2Vec2 m_localAnchor2;

	b2Vec2 m_u1;
	b2Vec2 m_u2;
	
	float32 m_constant;
	float32 m_ratio;
	
	float32 m_maxLength1;
	float32 m_maxLength2;

	// Effective masses
	float32 m_pulleyMass;
	float32 m_limitMass1;
	float32 m_limitMass2;

	// Impulses for accumulation/warm starting.
	float32 m_pulleyImpulse;
	float32 m_limitImpulse1;
	float32 m_limitImpulse2;

	// Position impulses for accumulation.
	float32 m_limitPositionImpulse1;
	float32 m_limitPositionImpulse2;

	b2LimitState m_limitState1;
	b2LimitState m_limitState2;
};

#endif
