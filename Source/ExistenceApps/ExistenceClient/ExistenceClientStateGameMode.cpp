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

#include "../../../Urho3D/Core/CoreEvents.h"
#include "../../../Urho3D/Engine/Engine.h"
#include "../../../Urho3D/UI/Font.h"
#include "../../../Urho3D/Input/Input.h"
#include "../../../Urho3D/Core/ProcessUtils.h"
#include "../../../Urho3D/UI/Text.h"
#include "../../../Urho3D/UI/UI.h"
#include "../../../Urho3D/Scene/Scene.h"
#include "../../../Urho3D/Graphics/StaticModel.h"
#include "../../../Urho3D/Graphics/Octree.h"
#include "../../../Urho3D/Graphics/Model.h"
#include "../../../Urho3D/Graphics/Material.h"
#include "../../../Urho3D/Graphics/Camera.h"
#include "../../../Urho3D/Resource/ResourceCache.h"
#include "../../../Urho3D/Graphics/Renderer.h"
#include "../../../Urho3D/Graphics/Camera.h"
#include "../../../Urho3D/UI/Window.h"
#include "../../../Urho3D/UI/Button.h"
#include "../../../Urho3D/UI/LineEdit.h"
#include "../../../Urho3D/UI/UIElement.h"
#include "../../../Urho3D/Math/BoundingBox.h"
#include "../../../Urho3D/UI/UIEvents.h"
#include "../../../Urho3D/Graphics/DebugRenderer.h"
#include "../../../Urho3D/IO/File.h"
#include "../../../Urho3D/IO/FileSystem.h"
#include "../../../Urho3D/Resource/XMLFile.h"
#include "../../../Urho3D/Resource/XMLElement.h"
#include "../../../Urho3D/IO/Deserializer.h"
#include "../../../Urho3D/UI/Cursor.h"
#include "../../../Urho3D/IO/FileSystem.h"
#include "../../../Urho3D/UI/ListView.h"
#include "../../../Urho3D/Engine/Console.h"
#include "../../../Urho3D/Physics/RigidBody.h"
#include "../../../Urho3D/Physics/CollisionShape.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"
#include "../../../Urho3D/Graphics/Animation.h"
#include "../../../Urho3D/Graphics/AnimatedModel.h"
#include "../../../Urho3D/Graphics/AnimationController.h"
#include "Character.h"
#include "../../../Urho3D/Graphics/Terrain.h"
#include "../../../Urho3D/Engine/EngineEvents.h"
#include "../../../Urho3D/Graphics/Zone.h"
#include "../../../Urho3D/IO/Log.h"
#include "../../../Urho3D/Graphics/Skybox.h"
#include "../../../Urho3D/UI/Sprite.h"
#include "../../../Urho3D/Graphics/StaticModelGroup.h"
#include "../../../Urho3D/Graphics/BillboardSet.h"
#include "../../../Urho3D/Math/Random.h"
#include "../../../Urho3D/Graphics/RenderPath.h"
#include "../../../Urho3D/Math/Color.h"

#include "GameStateHandlerComponent.h"
#include "GameStateEvents.h"
#include "GameObject.h"
#include "EnvironmentBuild.h"
#include "Manager.h"
#include "../Account.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <locale>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <algorithm>

#include "../../../Urho3D/Procedural/Procedural.h"
#include "../../../Urho3D/Procedural/ProceduralTerrain.h"
#include "../../../Urho3D/Procedural/RandomNumberGenerator.h"

#include "ExistenceClient.h"
#include "ExistenceClientStateGameMode.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"


#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;


ExistenceClientStateGameMode::ExistenceClientStateGameMode(Context* context):
    ExistenceClientStateSingleton (context)
    ,Existence(baseclass)
{

    /// Debug
    cout << "Debug: State Game Mode Constructor" << endl;

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Set aApplication
    Existence = gamestatehandlercomponent_->GetApplication();

    /// Subscribe () function for processing update events
    SubscribeToEvent(E_UPDATE, HANDLER(ExistenceClientStateGameMode, HandleUpdate));

    ///  Handle Post Updates
    SubscribeToEvent(E_POSTUPDATE,HANDLER(ExistenceClientStateGameMode,HandlerPostUpdates));

    return;
}

ExistenceClientStateGameMode::~ExistenceClientStateGameMode()
{
    /// Debug
    cout << "Debug: State Game Mode Deconstructor" << endl;


    return;
}

void ExistenceClientStateGameMode::Enter(void)
{
    /// Debug
    cout << "Debug: State Game Mode Enter" << endl;

    /// on ENter
    GameMode();

    return;
}

void ExistenceClientStateGameMode::Exit(void)
{

    /// Debug
    cout << "Debug: State Game Mode Exit" << endl;

    return;
}

void ExistenceClientStateGameMode::SetParameter(String parameter_)
{
    /// Do Nothing
    return;
}


/// Game Mode code
void ExistenceClientStateGameMode::GameMode(void)
{
    /// Get all Revelant resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Temporary for use flag of states
    bool CurrentStateIsGameMode = true;

    /// Set Camera mode to first person
    gamestatehandlercomponent_->SetCameraMode(CAMERAMODE_FIRSTPERSON);

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    /// viewport -> SetCamera(cameraObject);
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)

    /// Gets the child position
    Node* headNode = Existence->scene_->GetChild("head", true);

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    Node * cameraNode_ = headNode ->CreateChild("CameraFirstPerson");

    Existence->cameraNode_ = cameraNode_;

    /// Set an initial position for the camera scene node above the plane
    Existence->cameraNode_->SetPosition(Vector3(0.0f,0.0f,0.15f));
    Existence->cameraNode_->SetRotation(Quaternion(0.0,0.0,0.0));

    /// Set first person camera Node
    Camera* cameraObject = Existence->cameraNode_->CreateComponent<Camera>();
    cameraObject->SetOrthographic(0);
    cameraObject->SetZoom(1);
    cameraObject->SetNearClip(0.0f);
    cameraObject->SetUseClipping(false);

    Existence->character_->controls_.pitch_ = cameraNode_->GetRotation().PitchAngle();
    Existence->character_->controls_.yaw_ = cameraNode_->GetRotation().YawAngle();

    /// Change viewport
    Existence->viewport =  new Viewport(context_, Existence->scene_, cameraObject);
    renderer->SetViewport(0, Existence->viewport);

    Existence->effectRenderPath = Existence->viewport->GetRenderPath() -> Clone();

    Existence->effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    Existence->effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));

    /// Make the bloom mixing parameter more pronounced
    Existence->effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.9f, 0.6f));
    Existence->effectRenderPath->SetEnabled("Bloom", false);
    Existence->effectRenderPath->SetEnabled("FXAA3", false);

    Existence -> viewport->SetRenderPath(Existence->effectRenderPath);

    /// Load UI
    LoadGameModeUI();

    return;
}

/// Blank function for now
void ExistenceClientStateGameMode::OnUpdate(StringHash eventType, VariantMap& eventData)
{
    return;
}

/// Handle updates
void ExistenceClientStateGameMode::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    using namespace Update;

    /// Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();
    Input* input = GetSubsystem<Input>();

    /// Move the camera, scale movement with time step
    OnMoveCamera(timeStep);

    /// controls movement
    if(gamestatehandlercomponent_->GetCameraMode()==CAMERAMODE_FIRSTPERSON)
    {
        /// Clear previous controls
        Existence-> character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP | CTRL_FIRE, false);

        /// check if UI element is active
        if(!GetSubsystem<UI>()->GetFocusElement())
        {
            /// Clear previous controls

            /// Update controls using keys
            UI* ui = GetSubsystem<UI>();

            Existence-> character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown('W'));
            Existence-> character_->controls_.Set(CTRL_BACK, input->GetKeyDown('S'));
            Existence-> character_->controls_.Set(CTRL_LEFT, input->GetKeyDown('A'));
            Existence-> character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown('D'));
            Existence-> character_->controls_.Set(CTRL_FIRE, input->GetKeyDown('Q'));

            Existence-> character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

            /// Get character information
            Node * characterNode = Existence->scene_->GetChild("Character",true);

            RigidBody* body = characterNode->GetComponent<RigidBody>();

            Node * firstpersonCameraNode = characterNode -> GetChild("CameraFirstPerson",true);

            float YAW_SENSITIVITY=.5;

            /// Updaw character yaw and povement
            Existence-> character_->controls_.yaw_ +=(float)input->GetMouseMoveX() * YAW_SENSITIVITY;
            Existence-> character_->controls_.pitch_ +=(float)input->GetMouseMoveY() * YAW_SENSITIVITY;

            Existence-> character_->controls_.yaw_ = Clamp(Existence-> character_->controls_.yaw_, -80.0f, 80.0f);
            Existence-> character_->controls_.pitch_ = Clamp(Existence-> character_->controls_.pitch_, -80.0f, 80.0f);

            /// Copy yaw and pitch
            float yaw_=Existence-> character_->controls_.yaw_;
            float pitch_=Existence-> character_->controls_.pitch_;

            firstpersonCameraNode->SetRotation(Quaternion(pitch_,yaw_, 0.0f));
        }
    }
    return;
}

/// On any camera movement
void ExistenceClientStateGameMode::OnMoveCamera(float timeStep)
{
    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();
    Input* input = GetSubsystem<Input>();

    /// Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement()||gamestatehandlercomponent_ ->GetConsoleState())
    {
        return;
    }

    /// if camera is in fly mode
    if(gamestatehandlercomponent_ ->GetCameraMode()==CAMERAMODE_FLY)
    {
        /// Movement speed as world units per second
        float MOVE_SPEED=5.0f;

        /// Mouse sensitivity as degrees per pixel
        const float MOUSE_SENSITIVITY = 0.2f;

        /// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
        IntVector2 mouseMove = input->GetMouseMove();
        Existence->yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        Existence->pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        Existence->pitch_ = Clamp(Existence->pitch_, -180.0f, 180.0f);

        /// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        Existence->cameraNode_->SetRotation(Quaternion(Existence->pitch_, Existence->yaw_, 0.0f));

        /// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
        /// Use the Translate() function (default local space) to move relative to the node's orientation.
        if (input->GetKeyDown('W'))
        {
            Existence->cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('S'))
        {
            Existence->cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('A'))
        {
            Existence->cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('D'))
        {
            Existence->cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
        }

        /// added controls for up and down movement
        if (input->GetKeyDown('Q'))
        {
            Existence->cameraNode_->Translate(Vector3::UP * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('E'))
        {
            Existence->cameraNode_->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
        }


        /// added controls for up and down movement
        if (input->GetKeyDown('1'))
        {

            MOVE_SPEED=5.0f;
        }

        if (input->GetKeyDown('2'))
        {
            MOVE_SPEED=15.0f;
        }

        if (input->GetKeyDown('3'))
        {
            MOVE_SPEED=10.0f;
        }

        if (input->GetKeyDown('4'))
        {
            MOVE_SPEED=20.0f;
        }
    }

    return;
}


void ExistenceClientStateGameMode::LoadGameModeUI(void)
{
    /// Get all Revelant resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();


    /// Load main UI area
    Existence->loadSceneUI();

    /// rest of UI
    Existence->loadHUDFile("Resources/UI/MainTopBarWindow.xml",0,0);
    Existence->loadHUDFile("Resources/UI/PlayerInfoWindow.xml",0,34);

    /// load hud
    Existence->loadUIXML(UIQUICKMENU,0,0,0);

    /// Get player info  name from temporary list and put it into the character object
    Text* PlayerNameText = (Text*)ui->GetRoot()->GetChild("PlayerNameText", true);

    /// Get player level and level text
    unsigned int level=floor(Existence->character_->GetLevels().level/10);

    string levelstring=ConvertUIntToString(level);

    string levelstext=levels[level];

    /// Set hud sting to level and character name
    string playername=Existence->character_->GetPlayerInfo().lastname+" "+Existence->character_->GetPlayerInfo().firstname+" ("+levelstring+") "+ levelstext;


    PlayerNameText -> SetText(playername.c_str());

    Existence->UpdatePlayerInfoBar();

    /// use UI cursor
    /// Enable OS cursor
    ui->GetCursor()->SetVisible(true);

    if(ui->GetCursor()->IsVisible())
    {
        Existence->Print ("Cursor Exist");

        Existence->Print(ui->GetCursor()->GetAppliedStyle());
    }
    else
    {
        Existence->Print ("Cursor Does Not Exist");
    }

}


/// Handle post updates
void ExistenceClientStateGameMode::HandlerPostUpdates(StringHash eventType, VariantMap& eventData)
{
    /// game state
    UI* ui = GetSubsystem<UI>();

    Sprite* healthBar = (Sprite*)ui->GetRoot()->GetChild("PlayerInfoHealthBarIndicate", true);

    if(healthBar!=NULL)
    {

        float scale=Existence->character_->GetHealth()/100;

        healthBar -> SetScale(scale,1.0f);
    }

    return;
}

