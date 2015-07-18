#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Scene/Serializable.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Graphics/ParticleEffect.h>

#include "AIController.h"
#include "DroneEntity.h"

#include <Urho3D/DebugNew.h>

float AIController::aiAimSpeed= initialAimSpeed;
float AIController::aiPrediction = initialPrediction;
float AIController::aiAggression = initialAggression;

void AIController::ResetAI()
{
    aiAggression = initialAggression;
    aiPrediction = initialPrediction;
    aiAimSpeed = initialAimSpeed;
}

void AIController::MakeAIHarder()
{
    aiAggression += deltaAggression;
    if (aiAggression > maxAggression)
        aiAggression = maxAggression;

    aiPrediction += deltaPrediction;
    if (aiPrediction < maxPrediction)
        aiPrediction = maxPrediction;

    aiAimSpeed += deltaAimSpeed;
    if (aiAimSpeed > maxAimSpeed)
        aiAimSpeed = maxAimSpeed;
}

AIController::AIController(Context* context) : Entity(context)
{
}


AIController::~AIController()
{
}

void AIController::Start()
{

    SubscribeToEvent(node_, E_NODECOLLISION, HANDLER(AIController, HandleNodeCollision));
}

void AIController::RegisterObject(Context* context)
{
    context->RegisterFactory<AIController>();
}


void AIController::Control(DroneEntity* ownEntity, Node* ownNode, float timeStep)
{

    /// No target so wander
    ownEntity->controls.Set(CTRL_ALL, false);
    ownEntity->controls.Set(CTRL_FORWARD, true);
    ownEntity->dirChangeTime -= timeStep;

    if (ownEntity->dirChangeTime <= 0.0f)
    {
        ownEntity->dirChangeTime = 1.0f + Random(2.0f);
        ownEntity->controls.yaw_ += 0.1f * (Random(600.0f) - 300.0f);
    }
    if (ownEntity->GetSliding())
    {
        ownEntity->controls.yaw_ += 0.2f;

    }

    return;
}

