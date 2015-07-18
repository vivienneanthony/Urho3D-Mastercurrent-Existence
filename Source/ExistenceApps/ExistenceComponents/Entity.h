#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

/// more headers
#include <Urho3D/Urho3D.h>

#include "../../../Urho3D/Graphics/AnimationController.h"
#include "../../../Urho3D/Core/Context.h"
#include "../../../Urho3D/IO/MemoryBuffer.h"
#include "../../../Urho3D/Physics/PhysicsEvents.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"
#include "../../../Urho3D/Physics/RigidBody.h"
#include "../../../Urho3D/Scene/Scene.h"
#include "../../../Urho3D/Scene/SceneEvents.h"

#include "Character.h"


#include <iostream>
#include <string>

#include "../Player.h"

#define PLAYERFILE "character.xml"


/// Define player levels
using namespace  std;
using namespace Urho3D;

class Entity : public LogicComponent
{
    OBJECT(Entity)
public:
    Entity(Context *context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

     /// declaration
    virtual void Start();
    virtual void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    /// Called on physics update, fixed timestep.
	virtual void FixedUpdate(float timeStep);
	void WorldCollision(VariantMap& eventData);


    /// Clear everything
    int Clear(void);

    /// Set and get the character health
    int SetHealth(int health);
    int GetHealth(void);

    /// Set and get player characteristc and alliances (Primarily for temporary usage)
    playercharacteristics GetCharacteristics(void);
    playeralliance GetAlliance(void);
    int SetCharacteristics(playercharacteristics TempCharacteristics);
    int SetAlliance(playeralliance TempAlliance);

    /// Set basic information
    int SetEntityInfo(playerbasicinfo TempEntity);
    playerbasicinfo GetEntityInfo(void);

protected:
    playerobject CharacterEntity;

private:

};

#endif // ENTITY_H_INCLUDED
