#include <Urho3D/Urho3D.h>

#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include "../../../Urho3D/Graphics/StaticModel.h"
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Input/Controls.h>

#include "Entity.h"

#include "DroneEntity.h"
#include "Character.h"

#include <Urho3D/DebugNew.h>

using namespace Urho3D;

/// DroneEntity Constructor
DroneEntity::DroneEntity(Context* context)  :Entity(context)
    ,okToJump(0)
    ,inAirTime(0)
    ,onGroundTime(0)
    ,isSliding(0)
    ,onGround(0)
    ,dirChangeTime(0)
{
    /// Initialize values just in case to null or matching value
    okToJump=true;
    inAirTime=0;
    onGroundTime=0;
    isSliding=false;
    dirChangeTime=0;
    onGround=false;

    return;
}

/// DroneEntity destructor
DroneEntity::~DroneEntity()
{
    return;
}

/// DroneEntity register factory
void DroneEntity::RegisterObject(Context* context)
{
    /// Register component
    context->RegisterFactory<DroneEntity>("Existence");

    return;
}

/// DroneEntity attach collision event
void DroneEntity::Start()
{
    /// subscribe to node event collision
    SubscribeToEvent(node_, E_NODECOLLISION, HANDLER(DroneEntity, HandleNodeCollision));

    return;
}

/// DroneEntity fix update
void DroneEntity::FixedUpdate(float timeStep)
{
    RigidBody* body = node_->GetComponent<RigidBody>();

    /// AI control if controller exists
    if (controller != NULL)
    {
        controller->Control(this, node_, timeStep);
    }

    /// Movement ground/air
    Vector3 vel = body->GetLinearVelocity();

    /// Force the physics rotation
    Quaternion q;
    q = body->GetRotation();

    /// If onground
    if (onGround)
    {
        // If landed, play a particle effect at feet (use the AnimatedModel node)
        if (inAirTime > 0.5)
        {
            ///SpawnParticleEffect(node_->GetChildren()[0], node_->GetChildren()[0]->GetWorldPosition(), "Particle/SnowExplosion.xml", 1 );
        }

        inAirTime = 0;
        onGroundTime += timeStep;
    }
    else
    {
        onGroundTime = 0;
        inAirTime += timeStep;
    }

    /// If object is moving but not sliding
    if (inAirTime < 0.3f && !isSliding)
    {
        bool sideMove = false;

        /// Movement in four directions
        if (controls.IsDown(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT))
        {
            float animDir = 1.0f;
            Vector3 force(0, 0, 0);
            if (controls.IsDown(CTRL_FORWARD))
                force += q * Vector3(0, 0, 1);
            if (controls.IsDown(CTRL_BACK))
            {
                animDir = -1.0f;
                force += q * Vector3(0, 0, -1);
            }
            if (controls.IsDown(CTRL_LEFT))
            {
                sideMove = true;
                force += q * Vector3(-1, 0, 0);
            }
            if (controls.IsDown(CTRL_RIGHT))
            {
                sideMove = true;
                force += q * Vector3(1, 0, 0);
            }
            /// Normalize so that diagonal strafing isn't faster
            force.Normalize();
            force *= DroneEntityMoveForce;
            body->ApplyImpulse(force);
        }

        /// Overall damping to cap maximum speed
        body->ApplyImpulse(Vector3(-DroneEntityDampingForce * vel.x_, 0, -DroneEntityDampingForce * vel.z_));

        /// Jumping
        if (controls.IsDown(CTRL_JUMP))
        {
            if (okToJump && inAirTime < 0.1f)
            {
                /// Lift slightly off the ground for better animation
                body->SetPosition(body->GetPosition() + Vector3(0, 0.03, 0));
                body->ApplyImpulse(Vector3(0, DroneEntityJumpForce, 0));
                inAirTime = 1.0f;
                //animCtrl->PlayExclusive("Models/DroneEntitySnowWar/DroneEntity_JumpNoHeight.ani", LAYER_MOVE, false, 0.1);
                //animCtrl->SetTime("Models/DroneEntitySnowWar/DroneEntity_JumpNoHeight.ani", 0.0); // Always play from beginning
                okToJump = false;
            }
        }
        else okToJump = true;
    }
    else
    {
        /// Motion in the air
        /// Note: when sliding a steep slope, control (or damping) isn't allowed!
        if (inAirTime > 0.3f && !isSliding)
        {
            if (controls.IsDown(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT))
            {
                Vector3 force(0, 0, 0);
                if (controls.IsDown(CTRL_FORWARD))
                    {force += q * Vector3(0, 0, 1);}
                if (controls.IsDown(CTRL_BACK))
                    {force += q * Vector3(0, 0, -1);}
                if (controls.IsDown(CTRL_LEFT))
                    {force += q * Vector3(-1, 0, 0);}
                if (controls.IsDown(CTRL_RIGHT))
                    {force += q * Vector3(1, 0, 0);}

                /// Normalize so that diagonal strafing isn't faster
                force.Normalize();
                force *= DroneEntityAirMoveForce;
                body->ApplyImpulse(force);
            }
        }


    }

    /// Set previous controls to current controls
    prevControls = controls;

    return;
}

bool DroneEntity::GetSliding(void)
{
    return isSliding;
}

void DroneEntity::SetSliding(bool changeSliding)
{
    isSliding = changeSliding;

    return;
}
