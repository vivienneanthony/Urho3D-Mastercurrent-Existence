#ifndef AICONTROLLER_H
#define AICONTROLLER_H


#include "Entity.h"
#include "DroneEntity.h"

// All Urho3D classes reside in namespace Urho3D
using namespace Urho3D;

const float initialAggression = 0.0020f;
const float initialPrediction = 30.0f;
const float initialAimSpeed = 10.0f;
const float deltaAggression = 0.000025f;
const float deltaPrediction = -0.15f;
const float deltaAimSpeed = 0.30f;
const float maxAggression = 0.01f;
const float maxPrediction = 20.0f;
const float maxAimSpeed = 40.0f;

class DroneEntity;

class AIController : public Entity
{
	OBJECT(AIController);
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
	AIController(Context* context);
	~AIController();
	/// Register object factory.
	static void RegisterObject(Context* context);

	/// Called when the component is added to a scene node. Other components may not yet exist.
	virtual void Start();

    void Control(DroneEntity* ownEntity, Node* ownNode, float timeStep);

	static void ResetAI();
	static void MakeAIHarder();

protected:

private:

	static float aiAggression;
	static float aiPrediction;
	static float aiAimSpeed;

};

#endif
