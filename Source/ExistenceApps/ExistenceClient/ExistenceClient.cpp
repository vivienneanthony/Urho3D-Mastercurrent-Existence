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
#include "../../../Urho3D/UI/BorderImage.h"
#include "../../../Urho3D/Graphics/GraphicsEvents.h"
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
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"

#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;

#include "misc.inl"

// Expands to this example's entry-point
DEFINE_APPLICATION_MAIN(ExistenceClient)

ExistenceClient::ExistenceClient(Context* context) :
    ExistenceApp(context), uiRoot_(GetSubsystem<UI>()->GetRoot())
{
    /// Register
    Character::RegisterObject(context);
    GameObject::RegisterObject(context);
    EnvironmentBuild::RegisterObject(context);
    ProceduralTerrain::RegisterObject(context);
    Manager::RegisterNewSubsystem(context);
    EnvironmentBuild::RegisterNewSubsystem(context);
    GameStateHandlerComponent::RegisterNewSubsystem(context);
    GameStateHandlerComponent::RegisterGameStates(context);

    cout << "Debig: Existence App Existence " << &applicationPtr << endl;

    /// Register states
    cout << "Debug: Existence Client Base Class Constructor context" << &context << " context_"<< &context_ << endl;

    return;
}

ExistenceClient::~ExistenceClient()
{
    cout << "Debug: Existence Client Base Class Deconstructor" << endl;

    return;
}

void ExistenceClient::Init(Context * context)
{
    return;
}

/// Main program execution code
void ExistenceClient::Start()
{
    /// Initialize
    Init(context_);

    /// Execute base class startup
    ExistenceApp::Start();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_ = GetSubsystem<EnvironmentBuild>();
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Set aApplication
    gamestatehandlercomponent_->SetApplication(applicationPtr);

    /// Get ui
    UI* ui = GetSubsystem<UI>();

    /// create variables (urho3d)
    String additionresourcePath;

    additionresourcePath.Append(filesystem->GetProgramDir().CString());
    additionresourcePath.Append("Resources/");

    /// add resource path to last
    cache -> AddResourceDir(additionresourcePath);

    /// Set the loaded style as default style
    uiRoot_->SetDefaultStyle(style);

    CreateCursor();

    ui->GetCursor()->SetVisible(true);

    /// Set game active status
    accountexist=false;

    /// Setup Screen and Viewport
    ///SetupScreenViewport();
    ///AddLogoViewport();
    ///SetupScreenUI();
    //SplashSetupScreenViewport();
    //SplashShowGameLogo();
    //SplashTimer.Reset();
    //SplashStatInit();


    /// Initialize Console
    InitializeConsole();

    GetSubsystem<Input>()->Update();

    /// Enable OS cursor
       /// Configure rudimentary state handler
    gamestatehandlercomponent_ ->SetUIState(UI_NONE);

    GetSubsystem<Input>()->SetMouseVisible(true);

    /// load account
    LoadAccount();

    /// Create test value
    testvalue=911;

    /// Debug output context and testvalue
    cout << "Debug: ExistenceClient Class Test Value " << testvalue << " context_ " << &context_ << endl;

    /// Finally subscribe to the update event. Note that by subscribing events at this point we have already missed some events
    /// like the ScreenMode event sent by the Graphics subsystem when opening the application window. To catch those as well we
    /// could subscribe in the constructor instead.
    SubscribeToEvents();

    /// Randomize timer
    srand (time(NULL));

    cout << "Debig: Existence App Existence " << applicationPtr ->GetTestString()<< endl;

    /// Create test value
    testvalue=121;

    cout << "Debig: Existence App Existence " << applicationPtr ->GetTestString()<< endl;

    gamestatehandlercomponent_->Start();

    return;
}


/// Subscribe to all events
void ExistenceClient::SubscribeToEvents()
{
    /// Subscribe () function for console input
    SubscribeToEvent(E_CONSOLECOMMAND, HANDLER(ExistenceClient, HandleConsoleCommand));

    /// Subscribe () function for processing update events
    SubscribeToEvent(E_UPDATE, HANDLER(ExistenceClient, HandleUpdate));

    /// Add to Event (Key Input)
    SubscribeToEvent(E_KEYDOWN, HANDLER(ExistenceClient, HandlerFunctionKeyDown));

}

/// Setup the main viewport
void ExistenceClient::SetupScreenViewport()
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();


    renderer -> SetTextureQuality (QUALITY_HIGH);
    renderer ->SetMaterialQuality (QUALITY_HIGH);
    renderer ->SetShadowQuality (SHADOWQUALITY_HIGH_24BIT);

    /// create a new scene
    scene_= new Scene(context_);
    scene_-> CreateComponent<Octree>();
    scene_-> CreateComponent<DebugRenderer>();

    Node* existencelogobackgroundNode = scene_->CreateChild("Plane");
    existencelogobackgroundNode ->SetScale(Vector2(width*.012,height*.012));
    existencelogobackgroundNode ->SetPosition(Vector3(0.0,0.0,-2.93435));
    existencelogobackgroundNode ->SetRotation(Quaternion(0.0,0.0,0.0));
    StaticModel* existencelogobackgroundObject = existencelogobackgroundNode->CreateComponent<StaticModel>();
    existencelogobackgroundObject->SetModel(cache->GetResource<Model>("Resources/Models/existencelogobackground.mdl"));
    existencelogobackgroundObject->SetMaterial(cache->GetResource<Material>("Resources/Materials/existencelogobackground.xml"));

    /// Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    /// light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    /// The light will use default settings (white light, no shadows)
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    //lightNode->SetDirection(Vector3(0.0,0.9,2)); /// The direction vector does not need to be normalized
    lightNode->SetRotation(Quaternion(0.0,160.0,0.0));
    Light* lightObject = lightNode->CreateComponent<Light>();
    lightObject->SetLightType(LIGHT_DIRECTIONAL);
    lightObject->SetBrightness(.7);
    lightObject->SetSpecularIntensity(0);

    /// Add a component
    Node* lightNode2 = scene_->CreateChild("DirectionalLight");
    //lightNode2->SetDirection(Vector3(0.0,0.9,0.0)); /// The direction vector does not need to be normalized
    lightNode2->SetRotation(Quaternion(0.0,200.0,0.0));
    Light* lightObject2 = lightNode2->CreateComponent<Light>();
    lightObject2->SetLightType(LIGHT_DIRECTIONAL);
    lightObject2->SetBrightness(.4);
    lightObject2->SetSpecularIntensity(0);

    Node* lightNode3 = scene_->CreateChild("DirectionalLight");
    //lightNode3->SetDirection(Vector3(0.0,12,0.0)); /// The direction vector does not need to be normalized
    //lightNode3->SetRotation(Quaternion(0.0,180.0,0.0));
    lightNode3->LookAt(Vector3(0.0f,0.0f,0.0f));
    Light* lightObject3 = lightNode3->CreateComponent<Light>();
    lightObject3->SetLightType(LIGHT_DIRECTIONAL);
    lightObject3->SetBrightness(.1);
    lightObject3->SetSpecularIntensity(1);

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0,0.0,5.0));
    cameraNode_->SetRotation(Quaternion(0.0,-180.0,0.0));
    Camera* cameraObject = cameraNode_->CreateComponent<Camera>();
    cameraObject->SetOrthographic(1);
    cameraObject->SetZoom(3);

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}

/// Add logo to the viewport
void ExistenceClient::AddLogoViewport(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Create logo object
    Node* existencelogoNode = scene_->CreateChild("ExistenceLogo");

    /// Setup logo object properities
    existencelogoNode ->SetPosition(Vector3(0.0,2.0,2.0));
    existencelogoNode ->SetRotation(Quaternion(0.0, 0.0,0.0));
    existencelogoNode ->SetName("ExistenceLogo");

    StaticModel* existencelogoObject = existencelogoNode->CreateComponent<StaticModel>();
    existencelogoObject->SetModel(cache->GetResource<Model>("Resources/Models/existencelogo.mdl"));
    existencelogoObject->SetMaterial(cache->GetResource<Material>("Resources/Materials/existencelogo.xml"));

    return;
}


/// Handle updates
void ExistenceClient::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    using namespace Update;

    /// Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    return;
}

/// Print to output buffe console
void ExistenceClient::Print(const String& output)
{
    /// Logging appears both in the engine console and stdout
    LOGRAW(output + "\n");
}

/// Create a cursor
int  ExistenceClient::CreateCursor(void)
{

    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    /// control the camera, and when visible, it will point the raycast target
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    SharedPtr<Cursor> cursor(new Cursor(context_));

    ui->SetCursor(cursor);
    cursor->SetStyle("Cursor",style);

    /// Set starting position of the cursor at the rendering window center
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);

    return 1;

}

/// Setup the screen UI
void ExistenceClient::SetupScreenUI(void)
{
    /// Create a new scene UI
    scenePlayerUI_= new Scene(context_);
    scenePlayerUI_-> CreateComponent<Octree>();
    scenePlayerUI_-> CreateComponent<DebugRenderer>();

    /// Add a lightdelightNode
    Node* lightNode = scenePlayerUI_->CreateChild("uidirectionallight");
    lightNode->SetDirection(Vector3(0.0f, .8f, 3.20f)); /// The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    /// Create a Zone component for ambient lighting & fog control
    Node* zoneNode = scenePlayerUI_->CreateChild("uizone");
    Zone* zone = zoneNode->CreateComponent<Zone>();

    Vector3 boundingBoxMin(-20.0f,-20.0f,-20.0f);
    Vector3 boundingBoxMax(20.0f,20.0f,20.0f);

    /// change bounding box to something more accurate
    zone->SetBoundingBox(BoundingBox(boundingBoxMin,boundingBoxMax));
    zone->SetFogColor(Color(.08f, .08f, .08f));
    zone->SetFogStart(10.0f);
    zone->SetFogEnd(20.0f);
    zone->SetHeightFog (false);

    /// Add Camera
    Node * cameraNodePlayerUI_ = scenePlayerUI_->CreateChild("uicamera");
    cameraNodePlayerUI_->CreateComponent<Camera>();

    /// Set an initial position for the camera scene node above the plane
    cameraNodePlayerUI_->SetPosition(Vector3(0.0f, 0.8f, 3.0f));

    Node * emptyNode= scenePlayerUI_->CreateChild("uiempty");
    emptyNode->SetPosition(Vector3(0.0f,0.73f,0.0f));

    /// Create character node;
    Node * characterNode= scenePlayerUI_->CreateChild("Character");
    characterNode->SetPosition(Vector3(0.0f,0.0f,0.0f));

    cameraNodePlayerUI_ -> LookAt(Vector3(emptyNode->GetPosition()));
    lightNode -> Rotate(Quaternion(.398377,0.854323,0.141073,-0.302532));
}


/// Temporary Network
bool ExistenceClient::IsClientConnected(void)
{
    return ServerConnection;
}

/// Temporary online
bool ExistenceClient::ClientConnect(void)
{
    /// Set Connection
    ServerConnection = false;

    return false;
}
/// Temporary online
bool ExistenceClient::SetServerSettings(void)
{
    /// Set Connection
    ServerConnection = false;

    return false;
}


/// Get subsystems
Renderer * ExistenceClient::GetRenderSubsystems(void) const
{
    /// Define Resouces
    Renderer* CurrentRendererSubsystem = GetSubsystem<Renderer>();

    return CurrentRendererSubsystem;
}

/// Get subsystems
Graphics * ExistenceClient::GetGraphicsSubsystems(void) const
{
    /// Define Resouces
    Graphics* CurrentGraphics = GetSubsystem<Graphics>();

    return CurrentGraphics;
}

/// Get subsystems
UI * ExistenceClient::GetUISubsystems(void) const
{
    /// Define Resouces
    UI * CurrentUISubsystem = GetSubsystem<UI>();

    return CurrentUISubsystem;
}

/// Get subsystems
ResourceCache * ExistenceClient::GetResourceCacheSubsystems(void) const
{
    /// Define Resouces
    ResourceCache * CurrentResourceCacheSubsystem = GetSubsystem<ResourceCache>();

    return CurrentResourceCacheSubsystem;
}

Window * ExistenceClient::GetSharedWindow(void) const
{
    return window_;
}

void ExistenceClient::Exit()
{
    engine_->Exit();
}


/// Setup the main viewport
void ExistenceClient::SplashSetupScreenViewport(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    renderer -> SetTextureQuality (QUALITY_HIGH);
    renderer ->SetMaterialQuality (QUALITY_HIGH);
    renderer ->SetShadowQuality (SHADOWQUALITY_HIGH_24BIT);

    /// create a new scene
    scene_= new Scene(context_);
    scene_-> CreateComponent<Octree>();
    scene_-> CreateComponent<DebugRenderer>();

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("CameraSplashScreen");

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0,0.0,5.0));
    cameraNode_->SetRotation(Quaternion(0.0,-180.0,0.0));
    Camera* cameraObject = cameraNode_->CreateComponent<Camera>();
    cameraObject->SetOrthographic(1);
    cameraObject->SetZoom(3);

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}

/// Add logo to the viewport
void ExistenceClient::SplashShowGameLogo(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    BorderImage* splashUI = new BorderImage(context_);
    splashUI->SetName("Splash");

    Texture2D* texture = cache->GetResource<Texture2D>("Resources/Textures/gamelogo.png");
    splashUI->SetTexture(texture); // Set texture
    splashUI->SetSize(texture->GetWidth(), texture->GetHeight());
    splashUI->SetAlignment(HA_CENTER, VA_CENTER);

    ui->GetRoot()->AddChild(splashUI);


    return;
}

void ExistenceClient::SplashStatInit(void)
{

    SubscribeToEvent(E_UPDATE, HANDLER(ExistenceClient, HandlerSplashUpdate)); // Keep visible until rendering of the scene

    /// start frame
    GetSubsystem<Engine>()->RunFrame(); // Render Splash immediately

    return;
}


void ExistenceClient::HandlerSplashUpdate(StringHash eventType, VariantMap& eventData)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Check scene status
    if((scene_-> GetAsyncProgress()==true)&&(SplashTimer.GetMSec(false)>=600000000))
    {

        /// Create a scene node for the camera, which we will move around
        /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
        Node * cameraNode_ = scene_->GetChild("CameraLogin");

        /// If camera exist change viewport
        if(cameraNode_)
        {
            /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
            /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
            /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
            SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
            renderer->SetViewport(0, viewport);
        }
    }

    return ;
}
