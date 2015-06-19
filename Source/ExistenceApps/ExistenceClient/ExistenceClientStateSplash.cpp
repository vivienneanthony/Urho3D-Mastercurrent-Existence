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
#include "../../../Urho3D/UI/Sprite.h"
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

/// State SplashConstructor
ExistenceClientStateSplash::ExistenceClientStateSplash(Context * context)
    :ExistenceClientStateSingleton(context)
    ,Existence(NULL)
{
    /// Debug
    cout << "Debug: State SplashConstructor" << endl;

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();
    /// Set aApplication
    Existence = gamestatehandlercomponent_->GetApplication();

    /// Debug
    cout << "Debug: State SplashConstructor Test Value " << Existence->testvalue << endl;

    splashcompleted=false;

    return;
}

/// State SplashDesconstructor
ExistenceClientStateSplash::~ExistenceClientStateSplash()
{
    /// Debug
    cout << "Debug: State SplashDeconstructor" << endl;

    return;
}

/// State SplashEnter
void ExistenceClientStateSplash::Enter()
{

    /// Debug
    cout << "Debug: State Splash Enter" << endl;

    /// Login function start when entered
    SplashScreen();

    return;

}

/// State SplashExit
void ExistenceClientStateSplash::Exit()
{
    /// Debug
    cout << "Debug: State Splash Exit" << endl;

    return;
}

void ExistenceClientStateSplash::SetParameter(String parameter_)
{
    /// Do Nothing
    return;
}

/// Splashscreen
void ExistenceClientStateSplash::SplashScreen(void)
{
    SplashSetupScreenViewport();
    SplashShowGameLogo();
    SplashStatInit();

    return;
}
/// State SplashOnUpdate
void ExistenceClientStateSplash::OnUpdate(StringHash eventType, VariantMap& eventData)
{
    /// Debug
    cout << "Debug: State Splash OnUpdate" << endl;
}

/// Setup the main viewport
void ExistenceClientStateSplash::SplashSetupScreenViewport(void)
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
    Existence -> scene_= new Scene(context_);
    Existence -> scene_-> CreateComponent<Octree>();
    Existence -> scene_-> CreateComponent<DebugRenderer>();

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    Existence -> cameraNode_ = Existence -> scene_->CreateChild("CameraSplashScreen");

    /// Set an initial position for the camera scene node above the plane
    Existence -> cameraNode_->SetPosition(Vector3(0.0,0.0,5.0));
    Existence -> cameraNode_->SetRotation(Quaternion(0.0,-180.0,0.0));
    Camera* cameraObject = Existence -> cameraNode_->CreateComponent<Camera>();
    cameraObject->SetOrthographic(1);
    cameraObject->SetZoom(3);

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    Existence -> viewport = new Viewport(context_, Existence -> scene_, Existence -> cameraNode_->GetComponent<Camera>());
    renderer->SetViewport(0, Existence -> viewport);

    return;
}

/// Add logo to the viewport
void ExistenceClientStateSplash::SplashShowGameLogo(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    BorderImage* splashUI = new BorderImage(context_);
    splashUI->SetName("Splash");

    Texture2D* texture = cache->GetResource<Texture2D>("Resources/Textures/gamelogo.png");
    splashUI->SetTexture(texture); // Set texture
    splashUI->SetSize(width,height);
    splashUI->SetAlignment(HA_CENTER, VA_CENTER);

    ui->GetRoot()->AddChild(splashUI);


    return;
}

void ExistenceClientStateSplash::SplashStatInit(void)
{
/// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Create variables (urho3d)
    String InputDataFile;

    InputDataFile.Append(filesystem->GetProgramDir().CString());
    InputDataFile.Append("Resources/Scenes/");
    InputDataFile.Append("Login1.xml");

    bool success;
    /// Check if the input data file exist
    if(filesystem->FileExists(InputDataFile))
    {
        /// Open file as a Urho3d Datafile
        dataFile = new File(context_, InputDataFile, FILE_READ);

        if (dataFile -> IsOpen())
        {

            /// Get File Extension
            String extension = GetExtension(InputDataFile);


            /// Determine file extension
            if (extension != ".xml")
            {
                ///success= Existence-> scene_ -> Load(dataFile);
                success = Existence-> scene_ -> LoadAsync(dataFile);
            }
            else
            {
                success= Existence-> scene_ ->LoadAsyncXML(dataFile);
            }
        }
        else
        {
            /// set is error
            success=false;
        }

    }

    /// on update
    SubscribeToEvent(E_UPDATE, HANDLER(ExistenceClientStateSplash, HandlerSplashUpdate)); // Keep visible until rendering of the scene

    return;
}


void ExistenceClientStateSplash::HandlerSplashUpdate(StringHash eventType, VariantMap& eventData)
{
    /// Get Needed SubSystems
    ResourceCache* cache_ = GetSubsystem<ResourceCache>();
    Renderer* renderer_ = GetSubsystem<Renderer>();
    Graphics* graphics_ = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    /// Check scene status
    if(Existence -> scene_-> GetAsyncProgress()==1&&SplashTimer.GetMSec(false)>3600&&splashcompleted==false)
    {
        /// change splash
        splashcompleted = true;

        /// Create a scene node for the camera, which we will move around
        /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
        Node * cameraNode_ = Existence -> scene_->GetChild("MainCamera", true);

        /// If camera exist change viewport
        if(cameraNode_)
        {
            /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
            /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
            /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
            Existence -> viewport = new Viewport(context_, Existence->scene_, cameraNode_->GetComponent<Camera>());
            renderer_->SetViewport(0, Existence->viewport);

            /// Get current rendering path
            Existence->effectRenderPath =Existence-> viewport->GetRenderPath() -> Clone();

            /// loadd resources
            Existence->effectRenderPath->Append(cache_->GetResource<XMLFile>("PostProcess/Bloom.xml"));
            Existence->effectRenderPath->Append(cache_->GetResource<XMLFile>("PostProcess/FXAA2.xml"));

            /// Make the bloom mixing parameter more pronounced
            Existence->effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.9f, 0.6f));
            Existence->effectRenderPath->SetEnabled("Bloom", false);
            Existence->effectRenderPath->SetEnabled("FXAA2", false);

            /// Pointer render path to new
            Existence->viewport->SetRenderPath(Existence->effectRenderPath);

            /// Turn on effects
            Existence->effectRenderPath->ToggleEnabled("Bloom");
            Existence->effectRenderPath->ToggleEnabled("FXAA2");
        }

        /// Change UI
        UIElement * uiRoot_ = ui_->GetRoot();

        BorderImage * Splash = (BorderImage * ) uiRoot_ -> GetChild("Splash", true);

        if(Splash!=NULL)
        {
            /// Remove splash
            Splash -> Remove();
        }


        /// Get rendering window size as floats
        float width = (float)graphics_->GetWidth();
        float height = (float)graphics_->GetHeight();

        /// Create LetterBox Sprite
        Sprite* LetterBoxSprite = new Sprite(context_);
        LetterBoxSprite->SetName("LetterBoxSprite");

        /// Get letter box image
        Texture2D* texture = cache_ ->GetResource<Texture2D>("Resources/Textures/LetterBox.png");

        /// Set letter box properties
        LetterBoxSprite->SetTexture(texture); // Set texture
        LetterBoxSprite->SetSize(width,height);
        LetterBoxSprite->SetAlignment(HA_CENTER, VA_CENTER);

        /// Create letter box image to UIElement
        UIElement * LetterBoxUIElement = new UIElement(context_);
        LetterBoxUIElement->AddChild(LetterBoxSprite);

        /// Add letter box UIElement to ui
        uiRoot_->AddChild(LetterBoxUIElement);

        /// Set style of UIElements
        LetterBoxUIElement->SetOpacity(.8);

        LetterBoxSprite->SetStyleAuto();
        LetterBoxUIElement->SetStyleAuto();

        /// Load fonts
        Font * Mionta = cache_ ->GetResource<Font>("Resources/Fonts/mionta.ttf");
        Font * Neuton = cache_ ->GetResource<Font>("Resources/Fonts/Neuton-SC-Light.ttf");

        /// Create logo text
        Text * LogoText = new Text(context_);
        LogoText -> SetTextAlignment(HA_CENTER);
        LogoText -> SetFont(Mionta,32);
        LogoText -> SetText("EXISTENCE");

        /// Create LetterBox UI Element
        UIElement * LogoTextUIElement = new UIElement(context_);

        LogoTextUIElement->AddChild(LogoText);

        /// Add to UI
        uiRoot_->AddChild(LogoTextUIElement);

        ///TitleText->SetStyleAuto();
        LogoTextUIElement->SetStyleAuto();

        /// Move text to a position
        LogoTextUIElement->SetPosition((width/2)-288,200);

        /// Create Title text
        Text * TitleText = new Text(context_);
        TitleText -> SetTextAlignment(HA_CENTER);
        TitleText -> SetFont(Neuton,24);
        TitleText -> SetText("ALPHA AGE");
        TitleText -> SetRowSpacing(3.0);


        /// Create LetterBox UI Element
        UIElement * TitleTextUIElement = new UIElement(context_);

        TitleTextUIElement->AddChild(TitleText);

        /// Add to UI
        uiRoot_->AddChild(TitleTextUIElement);

        ///TitleText->SetStyleAuto();
        TitleTextUIElement->SetStyleAuto();

        /// Move text to a position
        TitleTextUIElement->SetPosition((width/2)-108,240);


        ///TitleText->SetStyleAuto();
        TitleTextUIElement->SetStyleAuto();

        UnsubscribeFromAllEvents();


        /// Create a event
        VariantMap gamestatechange;
        gamestatechange[GameState::P_CMD] = GAME_STATE_LOGIN;

        cout << "Debug: Attempt to send a state change" << endl;

        this->SendEvent(G_STATES_CHANGE,gamestatechange);
    }
    return ;

}
