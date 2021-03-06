#ifndef CHARACTER_H
#define CHARACTER_H
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


//#include "Player.h"
#include "../../../Urho3D/Input/Controls.h"
#include "../../../Urho3D/Scene/LogicComponent.h"
#include "../Player.h"
#include "../../../Urho3D/Core/Context.h"

#include "../../../Urho3D/Resource/ResourceCache.h"
#include "../../../Urho3D/Scene/Node.h"

#include "../../../Urho3D/Scene/SceneEvents.h"
#include "../../../Urho3D/Physics/PhysicsEvents.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"


using namespace Urho3D;
using namespace std;

const int CTRL_FORWARD = 1;
const int CTRL_BACK = 2;
const int CTRL_LEFT = 4;
const int CTRL_RIGHT = 8;
const int CTRL_JUMP = 16;
const int CTRL_FIRE = 32;
const int CTRL_ALL = 63;

const float MOVE_FORCE = 0.8f;
const float INAIR_MOVE_FORCE = 0.02f;
const float BRAKE_FORCE = 0.2f;
const float JUMP_FORCE = 1.0f;
const float YAW_SENSITIVITY = 0.1f;
const float INAIR_THRESHOLD_TIME = 0.1f;

#define CHARACTERWALKING 10
#define CHARACTERIDLE 1
#define CHARACTERNONE 0

/// Character component, responsible for physical movement according to controls, as well as animation.
class Character : public LogicComponent
{
    OBJECT(Character)

public:
    /// Construct.
    Character(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    virtual void Start();
    /// Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);
    virtual void WorldCollision(VariantMap& eventData);
    virtual void ArtificialSpaceCollision(VariantMap& eventData);

    /// Set and get the character health
    int SetHealth(int health);
    int GetHealth(void);

    /// Set basic information
    int SetPlayerInfo(playerbasicinfo TempPlayer);
    playerbasicinfo GetPlayerInfo(void);

    playeralliance GetAlliance(void);
    int SetAlliance(playeralliance TempAlliance);

    int SetCharacteristics(playercharacteristics TempCharacteristics);
    playercharacteristics GetCharacteristics(void);

    playerlevels GetLevels(void);
    int SetLevels(playerlevels TempLevels);

        /// Create a object
    void MagicBox(void);

    /// Movement controls. Assigned by the main program each frame.
    Controls controls_;

private:
    /// Handle physics collision event.
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    /// Grounded flag for movement.
    bool onGround_;
    /// Jump flag.
    bool okToJump_;
    /// In air timer. Due to possible physics inaccuracy, character can be off ground for max. 1/10 second and still be allowed to move.
    float inAirTimer_;

    Player CharacterPlayer;

    int characterstate_;
    int previouscharacterstate_;


};
#endif
