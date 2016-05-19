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

#ifndef B2_PRISMATIC_JOINT_H
#define B2_PRISMATIC_JOINT_H

#include "b2Joint.h"

struct b2PrismaticJointDef : public b2JointDef
{
	b2PrismaticJointDef()
	{
		type = e_prismaticJoint;
		anchorPoint.Set(0.0f, 0.0f);
		axis.Set(1.0f, 0.0f);
		lowerTranslation = 0.0f;
		upperTranslation = 0.0f;
		motorForce = 0.0f;
		motorSpeed = 0.0f;
		enableLimit = false;
		enableMotor = false;
	}

	b2Vec2 anchorPoint;
	b2Vec2 axis;
	float32 lowerTranslation;
	float32 upperTranslation;
	float32 motorForce;
	float32 motorSpeed;
	bool enableLimit;
	bool enableMotor;
};

class b2PrismaticJoint : public b2Joint
{
public:
	b2Vec2 GetAnchor1() const;
	b2Vec2 GetAnchor2() const;

	b2Vec2 GetReactionForce(float32 invTimeStep) const;
	float32 GetReactionTorque(float32 invTimeStep) const;

	float32 GetJointTranslation() const;
	float32 GetJointSpeed() const;
	float32 GetMotorForce(float32 invTimeStep) const;

	void SetMotorSpeed(float32 speed);
	void SetMotorForce(float32 force);

	//--------------- Internals Below -------------------

	b2PrismaticJoint(const b2PrismaticJointDef* def);

	void InitVelocityConstraints();
	void SolveVelocityConstraints(const b2TimeStep& step);
	bool SolvePositionConstraints();

	b2Vec2 m_localAnchor1;
	b2Vec2 m_localAnchor2;
	b2Vec2 m_localXAxis1;
	b2Vec2 m_localYAxis1;
	float32 m_initialAngle;

	b2Jacobian m_linearJacobian;
	float32 m_linearMass;				// effective mass for point-to-line constraint.
	float32 m_linearImpulse;
	
	float32 m_angularMass;			// effective mass for angular constraint.
	float32 m_angularImpulse;

	b2Jacobian m_motorJacobian;
	float32 m_motorMass;			// effective mass for motor/limit translational constraint.
	float32 m_motorImpulse;
	float32 m_limitImpulse;
	float32 m_limitPositionImpulse;

	float32 m_lowerTranslation;
	float32 m_upperTranslation;
	float32 m_maxMotorForce;
	float32 m_motorSpeed;
	
	bool m_enableLimit;
	bool m_enableMotor;
	b2LimitState m_limitState;
};

#endif
