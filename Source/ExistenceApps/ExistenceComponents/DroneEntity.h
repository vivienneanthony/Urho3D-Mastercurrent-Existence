#ifndef DroneEntity_H
#define DroneEntity_H
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



#include "../../../Urho3D/Input/Controls.h"
#include "../../../Urho3D/Scene/LogicComponent.h"
#include "../../../Urho3D/Core/Context.h"

#include "../../../Urho3D/Resource/ResourceCache.h"
#include "../../../Urho3D/Scene/Node.h"

#include "../../../Urho3D/Scene/SceneEvents.h"
#include "../../../Urho3D/Physics/PhysicsEvents.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"


#include "Entity.h"

#include "AIController.h"

using namespace Urho3D;
using namespace std;

const int LAYER_MOVE = 0;
const int LAYER_ATTACK = 1;


const float DroneEntityMoveForce = 25.0f;
const float DroneEntityAirMoveForce = 1.05f;
const float DroneEntityDampingForce = 5.0f;
const float DroneEntityJumpForce = 450.0f;
const Vector3 DroneEntityThrowVelocity(0.0f, 4.25f, 20.0f);
const Vector3 DroneEntityThrowPosition(0.0f, 0.2f, 1.0f);
const float DroneEntityThrowDelay = 0.1f;
const float DroneEntityCorpseDuration = 3.0f;
const int DroneEntityPoints = 250;

class AIController;

class DroneEntity : public Entity
{
    OBJECT(DroneEntity);
public:
    /// Constructor and Destructor
    DroneEntity( Context* context );
    ~DroneEntity();

    /// Register object factory.
    static void RegisterObject(Context* context);
    virtual void Start();
    virtual void FixedUpdate(float timeStep);
    virtual void OnNodeCollision(StringHash eventType, VariantMap& eventData);

    /// Movement controls. Assigned by the main program each frame.
    Controls controls;
    Controls prevControls;

    /// Temporary
    float dirChangeTime;

    /// Public functions
    bool GetSliding(void);
    void SetSliding(bool changeSliding);

protected:

private:

    /// added variables
    bool  okToJump;
    float inAirTime;
    float onGroundTime;
    bool onGround;
    bool isSliding;

    SharedPtr<AIController> controller;

};
#endif
