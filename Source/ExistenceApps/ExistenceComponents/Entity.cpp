//
// Copyright (c) 2008-2014 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Urho3D.h>

#include "../../../Urho3D/Graphics/AnimationController.h"
#include "../../../Urho3D/Core/Context.h"
#include "../../../Urho3D/IO/MemoryBuffer.h"
#include "../../../Urho3D/Physics/PhysicsEvents.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"
#include "../../../Urho3D/Physics/RigidBody.h"
#include "../../../Urho3D/Scene/Scene.h"
#include "../../../Urho3D/Scene/SceneEvents.h"
#include "../../../Urho3D/Graphics/StaticModel.h"
#include "../../../Urho3D/Graphics/AnimatedModel.h"


#include "Character.h"
#include "Entity.h"

#include <iostream>


using namespace std;
using namespace Urho3D;


/// create a entity
Entity::Entity(Context* context) :
    LogicComponent(context)

{

    /// Initalize information
    CharacterEntity.firstname="";
    CharacterEntity.middlename="";
    CharacterEntity.lastname="";

    CharacterEntity.health= 0;

    /// player moves for surface of planet to a ship so it relations change for the x,y,z for respective object
    CharacterEntity.reputation1= 0;
    CharacterEntity.reputation2= 0;
    CharacterEntity.reputation3= 0;
    CharacterEntity.reputation4= 0;
    CharacterEntity.reputation5= 0;

    /// Set player experience
    CharacterEntity.experience = 0;
    CharacterEntity.personalitytrait=0;

    /// Charcter specs
    CharacterEntity.alienrace=0;
    CharacterEntity.alienalliancealigned=0;
    CharacterEntity.gender=0;

    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

/// Set and get the character health
int Entity::SetHealth(int health)
{
    CharacterEntity.health = health;

    return 1;

}

int Entity::GetHealth(void)
{
    return CharacterEntity.health;
}


/// Don nothing now;
void Entity::Start(void)
{
    return;
}

int Entity::Clear(void)
{
    return 1;
}

/// Get player characteristics
playercharacteristics Entity::GetCharacteristics(void)
{
    /// Charcter specs
    playercharacteristics Temporarycharacteristics;

    Temporarycharacteristics.gender = CharacterEntity.gender;
    Temporarycharacteristics.personalitytrait=CharacterEntity.personalitytrait;

    return Temporarycharacteristics;
}


/// Get player alliance
playeralliance Entity::GetAlliance(void)
{
    /// Charcter specs
    playeralliance Temporaryalliance;

    Temporaryalliance.alienrace = CharacterEntity.alienrace;
    Temporaryalliance.alienalliancealigned=CharacterEntity.alienalliancealigned;

    return Temporaryalliance;
}

/// Set player characteristics
int Entity::SetCharacteristics(playercharacteristics TempCharacteristics)
{
    /// Set character charactistics
    CharacterEntity.gender= TempCharacteristics.gender;
    CharacterEntity.personalitytrait= TempCharacteristics.personalitytrait;

    return 1;
}


/// Set player alliance
int Entity::SetAlliance(playeralliance TempAlliance)
{
    /// Set charcter alliance
    CharacterEntity.alienrace=TempAlliance.alienrace;
    CharacterEntity.alienalliancealigned=TempAlliance.alienalliancealigned;

    return 1;
}

int Entity::SetEntityInfo(playerbasicinfo TempEntity)
{
    /// Set charcter alliance
    CharacterEntity.firstname=TempEntity.firstname;
    CharacterEntity.middlename=TempEntity.middlename;
    CharacterEntity.lastname=TempEntity.lastname;

    return 1;
}

/// Entity GetEntityInfo
playerbasicinfo Entity::GetEntityInfo(void)
{
    /// Charcter specs
    playerbasicinfo Temporarybasicinfo;

    /// Set charcter alliance
    Temporarybasicinfo.firstname = CharacterEntity.firstname;
    Temporarybasicinfo.middlename = CharacterEntity.middlename;
    Temporarybasicinfo.lastname = CharacterEntity.lastname;

    return Temporarybasicinfo;
}

/// Entity Register Object
void Entity::RegisterObject(Context* context)
{
    context->RegisterFactory<Entity>("Existence");


}

/// Entity Handle Node Collision
void Entity::HandleNodeCollision(StringHash eventType, VariantMap& eventData)
{
    using namespace NodeCollision;

    // Get the other colliding body, make sure it is moving (has nonzero mass)
    RigidBody* otherBody = (RigidBody*)eventData[P_OTHERBODY].GetPtr();
    Node* otherNode = (Node*)eventData[P_OTHERNODE].GetPtr();

    // If the other collision shape belongs to static geometry, perform world collision
    StaticModel * staticmodelreference= otherNode->GetComponent<StaticModel>();
    AnimatedModel * animatedmodelreference = otherNode->GetComponent<AnimatedModel>();

    /// if node has a static model
    if (staticmodelreference||animatedmodelreference)
    {
        WorldCollision(eventData);
    }

    return;
}


/// Entity FixedUpdate
void Entity::FixedUpdate(float timeStep)
{


}

/// World Collision Entity
void Entity::WorldCollision(VariantMap& eventData)
{
    /// world collsion
    /// Code for collision goes here but might move it to to each t

    // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
	using namespace NodeCollision;

	VectorBuffer contacts = eventData["Contacts"].GetBuffer();
	while (!contacts.IsEof())
	{
		Vector3 contactPosition = contacts.ReadVector3();
		Vector3 contactNormal = contacts.ReadVector3();
		float contactDistance = contacts.ReadFloat();
		float contactImpulse = contacts.ReadFloat();

		// If contact is below node center and mostly vertical, assume it's ground contact
		if (contactPosition.y_ < node_->GetPosition().y_)
		{
			float level = Abs(contactNormal.y_);
			if (level > 0.75)
				onGround = true;
			else
			{
				// If contact is somewhere inbetween vertical/horizontal, is sliding a slope
				if (level > 0.1)
					isSliding = true;
			}
		}
	}

	// Ground contact has priority over sliding contact
	if (onGround == true)
		isSliding = false;

    return;
}
