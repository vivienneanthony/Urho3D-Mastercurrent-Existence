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
#include "../ExistenceComponents/Character.h"
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

#include "../ExistenceComponents/GameStateHandlerComponent.h"
#include "../ExistenceComponents/GameStateEvents.h"
#include "../ExistenceComponents/GameObject.h"
#include "../ExistenceComponents/EnvironmentBuild.h"
#include "../ExistenceComponents/Manager.h"
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
#include "ExistenceClientStateMainScreen.h"
#include "ExistenceClientStateProgress.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"


#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;

/// Constructor Destror
ExistenceClientStateMainScreen::ExistenceClientStateMainScreen(Urho3D::Context* context)
    :ExistenceClientStateSingleton (context)
    ,Existence(baseclass)
{

    /// Debug
    cout << "Debug: State Main Screen Constructor" << endl;

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Set Application
    Existence = gamestatehandlercomponent_->GetApplication();

    ///CHange State
    gamestatehandlercomponent_->SetUIState(UI_CHARACTERSELECTIONINTERFACE);

    /// Subscribe to
    SubscribeToEvent(P_LOAD_CHANGE,HANDLER(ExistenceClientStateMainScreen,ListenToLoad));

    return;
}

ExistenceClientStateMainScreen::~ExistenceClientStateMainScreen()
{
    /// Debug
    cout << "Debug: State Main Screen Deconstructor" << endl;

    return;
}

void ExistenceClientStateMainScreen::Enter()
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Debug
    cout << "Debug: State Main Screen Enter" << endl;

    /// Clear Everything
    ///Existence->scene_->Clear();
    ui->Clear();

    /// Setup the screen UI
    Existence->SetupScreenViewport();
    Existence->SetupScreenUI();

    /// Load the user interace
    MainScreen();

    return;
}

void ExistenceClientStateMainScreen::Exit()
{
    /// Debug
    cout << "Debug: State Main Screen Exit" << endl;

    return;
}

void ExistenceClientStateMainScreen::OnUpdate(StringHash eventType, VariantMap& eventData)
{
    /// Debug
    cout << "Debug: State Main Screen OnUpdate" << endl;

    return;
}


void ExistenceClientStateMainScreen::SetParameter(String parameter_)
{
    /// Do Nothing
    return;
}


/// Main screen
void ExistenceClientStateMainScreen::MainScreen(void)
{
    /// Set variables
    bool CurrentStateIsMainScreen=true;


    /// Remove Existence Logo Node if it exist
    if(Existence->scene_->GetChild("ExistenceLogo",true))
    {
        Existence->scene_->GetChild("ExistenceLogo",true)->RemoveAllComponents();
        Existence->scene_->GetChild("ExistenceLogo",true)->Remove();
    }

    MainScreenUI();

    /// load hud
    Existence->loadUIXML(UIMINIQUICKMENU,0,0,0);


    return;
}


/// Main screen user interface function
void ExistenceClientStateMainScreen::MainScreenUI(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();


    /// Create player mesh node and scale
    Node* playermeshNode = Existence-> scene_->CreateChild("playerMesh");

    Existence -> LoadAccountPlayers();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// initialize variables to create in
    UIElement * newcharacterUIElement = new UIElement (context_);
    UIElement * menuUIElements = new UIElement (context_);

    Button * newcharacterButton = new Button(context_);
    Text * newcharacterText = new Text(context_);

    Button * playerupdatesButton = new Button(context_);
    Button * exitButton = new Button(context_);

    /// create new character element
    newcharacterUIElement -> SetPosition(1,128);
    newcharacterUIElement -> SetMinSize(470,66);
    newcharacterUIElement -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470,66));
    newcharacterUIElement -> SetAlignment(HA_LEFT, VA_TOP);
    newcharacterUIElement -> SetStyleAuto();
    newcharacterUIElement -> SetOpacity(.8);

    newcharacterButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470, 66));
    newcharacterButton -> SetAlignment(HA_LEFT, VA_TOP);
    newcharacterButton -> SetFixedSize(470, 66);
    newcharacterButton -> SetPosition(0, 0);
    newcharacterButton -> SetName("newcharacterButton");

    newcharacterText -> SetPosition(0, 0);
    newcharacterText -> SetAlignment(HA_LEFT, VA_CENTER);
    newcharacterText ->	SetTextAlignment(HA_LEFT);
    newcharacterText -> SetText("  Create New Character");

    /// Set font and text color
    newcharacterText ->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 16);

    newcharacterUIElement -> AddChild(newcharacterButton);
    newcharacterUIElement -> AddChild(newcharacterText);

    /// Create exit button settings/media/home2/vivienne/Existence/src
    menuUIElements -> SetPosition(width-200,height-100);
    menuUIElements -> SetMinSize(200,32);
    menuUIElements -> SetLayout(LM_FREE, 6, IntRect(0, 0, 200, 32));
    menuUIElements  -> SetAlignment(HA_LEFT, VA_TOP);
    menuUIElements  -> SetStyleAuto();
    menuUIElements  -> SetOpacity(.8);

    exitButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 200, 32));
    exitButton -> SetAlignment(HA_LEFT, VA_TOP);
    exitButton -> SetFixedSize(200, 32);
    exitButton -> SetPosition(0, 0);
    exitButton -> SetName("exitButton");

    menuUIElements -> AddChild(exitButton);

    /// add window elements
    Existence-> uiRoot_->AddChild(newcharacterUIElement);

    Existence-> uiRoot_->AddChild(menuUIElements);

    unsigned int placement=8; /// Set pace between buttons variable

    /// Loop through all players recovered
    for(unsigned int i=0; i< Existence->TemporaryAccountPlayerListLimit; ++i)
    {

        UIElement * playerUIElement = new UIElement (context_);

        UIElement * playernameUIElement = new UIElement (context_);
        UIElement * playerinfoUIElement = new UIElement (context_);
        Button * playernameButton = new Button(context_);
        Button * playerinfoButton = new Button(context_);
        Button * playerstartButton = new Button(context_);
        Text *  playernameText = new Text(context_);

        UIElement * playerupdatesUIElement = new UIElement (context_);
        Button * playerupdatesButton = new Button(context_);

        /// create new character element
        playerUIElement -> SetPosition(1,(128+66)+placement);
        playerUIElement  -> SetMinSize(540,66);
        playerUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470,66));
        playerUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playerUIElement  -> SetStyleAuto();

        playernameUIElement -> SetPosition(0,0);
        playernameUIElement  -> SetMinSize(470,48);
        playernameUIElement  -> SetMaxSize(470,48);
        playernameUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470,48));
        playernameUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playernameUIElement  -> SetStyleAuto();

        playerinfoUIElement -> SetPosition(416,50);
        playerinfoUIElement  -> SetMinSize(16,16);
        playerinfoUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 16,16));
        playerinfoUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playerinfoUIElement  -> SetStyleAuto();

        playernameButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470, 48));
        playernameButton -> SetAlignment(HA_LEFT, VA_TOP);
        playernameButton -> SetFixedSize(470, 48);
        playernameButton -> SetPosition(0, 0);

        playerinfoButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 16, 16));
        playerinfoButton -> SetAlignment(HA_LEFT, VA_TOP);
        playerinfoButton-> SetFixedSize(16, 16);
        playerinfoButton -> SetPosition(0, 0);

        playerstartButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 16, 16));
        playerstartButton -> SetAlignment(HA_LEFT, VA_TOP);
        playerstartButton-> SetFixedSize(16, 16);
        playerstartButton -> SetPosition(17, 0);

        playernameText -> SetPosition(0, 8);
        playernameText -> SetAlignment(HA_LEFT, VA_TOP);
        playernameText -> SetTextAlignment(HA_LEFT);

        /// Create a username to display on player button
        string playername= "  "+Existence->TemporaryAccountPlayerList[i].GetPlayerInfo().firstname+" "+ Existence->TemporaryAccountPlayerList[i].GetPlayerInfo().lastname;

        playernameText -> SetText(String(playername.c_str()));

        playernameText -> SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 13);

        playerupdatesUIElement -> SetPosition(476, 0);
        playerupdatesUIElement  -> SetMinSize(66,48);
        playerupdatesUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 66,48));
        playerupdatesUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playerupdatesUIElement  -> SetStyleAuto();
        playerupdatesUIElement -> SetOpacity(.8);

        playerupdatesButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 66, 48));
        playerupdatesButton -> SetAlignment(HA_LEFT, VA_TOP);
        playerupdatesButton -> SetFixedSize(66, 48);
        playerupdatesButton -> SetPosition(0, 0);

        playernameUIElement -> SetOpacity(.8);

        playernameUIElement -> AddChild(playernameButton);
        playernameUIElement -> AddChild(playernameText);

        playerinfoUIElement -> AddChild(playerinfoButton);
        playerinfoUIElement -> AddChild(playerstartButton);

        playerupdatesUIElement -> AddChild(playerupdatesButton);

        playerUIElement ->AddChild(playernameUIElement);
        playerUIElement ->AddChild(playerinfoUIElement);
        playerUIElement ->AddChild(playerupdatesUIElement);


        Existence-> uiRoot_->AddChild(playerUIElement);

        playernameButton -> SetStyle("playermainscreenButton");

        string playerbuttontext="playermainscreenButton ";
        string playerinfobuttontext="playermainscreeninfoButton";
        string playerstartbuttontext="";

        switch(i)
        {
        case 0:
            playerbuttontext.append("0");
            playerinfobuttontext.append("1");
            playerstartbuttontext.append("0");

            break;
        case 1:
            playerbuttontext.append("1");
            playerinfobuttontext.append("2");
            playerstartbuttontext.append("1");
            break;
        case 2:
            playerbuttontext.append("2");
            playerinfobuttontext.append("3");
            playerstartbuttontext.append("2");
            break;
        case 3:
            playerbuttontext.append("3");
            playerinfobuttontext.append("4");
            playerstartbuttontext.append("3");
        }

        playernameButton -> SetName(String(playerbuttontext.c_str()));
        playerinfoButton -> SetName(String(playerinfobuttontext.c_str()));
        playerstartButton -> SetName(String(playerstartbuttontext.c_str()));

        playerupdatesButton -> SetStyle("playermainscreenButton2");
        playerinfoButton -> SetStyle("Player16x16Button");
        playerstartButton -> SetStyle("Select16x16Button");

        SubscribeToEvent(playernameButton, E_RELEASED, HANDLER(ExistenceClientStateMainScreen, HandleCharacterSelectedReleased));
        SubscribeToEvent(playerinfoButton, E_RELEASED, HANDLER(ExistenceClientStateMainScreen, HandleCharacterSelectedInfoButtonReleased));
        SubscribeToEvent(playerstartButton, E_RELEASED, HANDLER(ExistenceClientStateMainScreen, HandleCharacterStartButtonReleased));

        placement=placement+(66+8);

    }

    /// Set exitbutton and new character style
    exitButton -> SetStyle("logoutButton");
    newcharacterButton -> SetStyle("newcharacterButton");

    Vector3 playermeshPosition=Vector3(1.0f,-2.0f,0.0f);

    playermeshNode ->SetPosition(playermeshPosition);
    playermeshNode ->SetRotation(Quaternion(0.0, 0.0,0.0));

    playermeshNode->SetScale(2);

    /// See if a camera exist
    Node * cameraNode=Existence-> scene_->GetChild("Camera");

    cameraNode->SetPosition(Vector3(2.0f,-0.5f,4.0f));

    /// Add three point lighting
    Node* FillLightNode =  Existence->scene_->CreateChild("DirectionalLight");
    FillLightNode->SetDirection(playermeshPosition+Vector3(1.0f,0.0f,1.0f)); /// The direction vector does not need to be normalized
    Light* FillLight = FillLightNode->CreateComponent<Light>();
    FillLight->SetLightType(LIGHT_DIRECTIONAL);
    FillLight ->SetBrightness(1);
    FillLight ->SetSpecularIntensity(0);
    FillLightNode->LookAt(playermeshPosition);

    /// Add three point lighting
    Node* KeyLightNode =  Existence->scene_->CreateChild("DirectionalLight");
    KeyLightNode->SetDirection(playermeshPosition+Vector3(1.0f,0.0f,-1.0f)); /// The direction vector does not need to be normalized
    Light* KeyLight = KeyLightNode->CreateComponent<Light>();
    KeyLight->SetLightType(LIGHT_DIRECTIONAL);
    KeyLight ->SetBrightness(.3);
    KeyLight ->SetSpecularIntensity(1);
    KeyLightNode->LookAt(playermeshPosition);

    /// Add three point lighting
    Node* BackLightNode =  Existence->scene_->CreateChild("DirectionalLight");
    BackLightNode->SetDirection(playermeshPosition+Vector3(-1.0f,0.0f,-1.0f)); /// The direction vector does not need to be normalized
    Light* BackLight = BackLightNode->CreateComponent<Light>();
    BackLight->SetLightType(LIGHT_DIRECTIONAL);
    BackLight ->SetBrightness(.3);
    BackLight ->SetSpecularIntensity(0);
    BackLightNode->LookAt(playermeshPosition);

    /// Load the player mesh based on display random if no character and charcter
    if( Existence->TemporaryAccountPlayerListLimit==0)
    {
        Existence-> loadplayerMesh(playermeshNode,  Existence->TemporaryPlayer.GetAlliance().alienrace,  Existence->TemporaryPlayer.GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);
    }
    else
    {
        Existence-> loadplayerMesh(playermeshNode, Existence->TemporaryAccountPlayerList[0].GetAlliance().alienrace,  Existence->TemporaryAccountPlayerList[0].GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);
    }

    /// Subscribe to events
    SubscribeToEvent(newcharacterButton, E_RELEASED, HANDLER(ExistenceClientStateMainScreen, MainScreenUIHandleClosePressed));
    SubscribeToEvent(exitButton, E_RELEASED, HANDLER(ExistenceClientStateMainScreen, MainScreenUIHandleClosePressed));

    return ;

}


/// Main screen user interface handle close pressed
void ExistenceClientStateMainScreen::MainScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// Resource
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Get control that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());


    /// Check for a click and which button
    if(clicked)
    {

        String clickedname=clicked->GetName();
        string clickednamestring=clickedname.CString();

        if(clickednamestring=="newcharacterButton")
        {
            /// remove child nodeAddItem (UIElement *item)
            Existence-> scene_->GetChild("playerMesh",true)->Remove();

            /// Clear screen
            Existence->eraseScene();

            Console* console = GetSubsystem<Console>();

            console -> SetVisible(false);

            gamestatehandlercomponent_ ->SetConsoleState(UI_CONSOLEOFF);

            /// Enable OS cursor
            GetSubsystem<Input>()->SetMouseVisible(true);

            UnsubscribeFromAllEvents();

            /// Create a event
            VariantMap gamestatechange;
            gamestatechange[GameState::P_CMD] = GAME_STATE_PLAYERCREATE;

            cout << "Debug: Attempt to send a state change" << endl;

            this->SendEvent(G_STATES_CHANGE,gamestatechange);

        }
        else if(clickednamestring=="exitButton")
        {
            ///engine_->Exit();
            Existence -> Exit();
        }
    }


    return;
}

/// Handle character selection
void ExistenceClientStateMainScreen::HandleCharacterStartButtonReleased(StringHash eventType, VariantMap& eventData)
{

    /// Get the button that was clicked
    UI* ui_ = GetSubsystem<UI>();

    UIElement* clickedinfo = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    /// Get Button String
    String clickedButtonString(clickedinfo->GetName());

    /// Convert text to a integer number
    int button = clickedButtonString.Back()-'0';

    /// Load UI root
    UIElement * uiroot =  ui_ ->	GetRoot ();

    /// Locate Window PlayerWindow
    Window* SceneLoaderWindow = dynamic_cast<Window*>( uiroot ->GetChild("SceneLoaderWindow", true));

    /// IF PlayerWindow exist then enable visibility
    if(SceneLoaderWindow)
    {
        SceneLoaderWindow-> SetDeepEnabled(true);
        SceneLoaderWindow-> GetParent() -> SetVisible(true);
        SceneLoaderWindow-> GetParent() -> SetBringToFront(true);

    }
    else
    {
        /// Load Player WIndow UI
        Existence-> loadUIXML(UISCENESELECTWINDOW,200,100, button);
    }

    return;
}


/// Handle character selection
void ExistenceClientStateMainScreen::HandleCharacterSelectedReleased(StringHash eventType, VariantMap& eventData)
{

    /// get the button that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());
    UI* ui_ = GetSubsystem<UI>();

    String clickedButtonString(clicked->GetName());

    string clickedtext;
    int clickedbutton=0;

    stringstream ss(clickedButtonString.CString());
    ss >> clickedtext >> clickedbutton;

    /// If the main screen player button was clicked continue
    if(clickedtext=="playermainscreenButton")
    {

        /// Get player mesh node and remove children
        Node* playermeshNode = Existence-> scene_->GetChild("playerMesh");

        playermeshNode->RemoveAllChildren();

        /// Set selected player
        Existence-> TemporaryAccountPlayerSelected=clickedbutton;

        /// Load player mesh in main screen ui
        Existence-> loadplayerMesh(playermeshNode,  Existence->TemporaryAccountPlayerList[clickedbutton].GetAlliance().alienrace,  Existence->TemporaryAccountPlayerList[clickedbutton].GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);
    }

    return;
}



/// Handle character selection
void ExistenceClientStateMainScreen::HandleCharacterSelectedInfoButtonReleased(StringHash eventType, VariantMap& eventData)
{

    /// Get the button that was clicked
    UI* ui_ = GetSubsystem<UI>();

    UIElement* clickedinfo = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    /// Get Button String
    String clickedButtonString(clickedinfo->GetName());

    /// Convert text to a integer number
    int button = clickedButtonString.Back()-'0';

    /// Load UI root
    UIElement * uiroot = ui_ ->	GetRoot ();

    /// Locate Window PlayerWindow
    Window* PlayerWindow = dynamic_cast<Window*>(uiroot ->GetChild("PlayerWindow", true));

    /// IF PlayerWindow exist then enable visibility
    if(PlayerWindow)
    {
        PlayerWindow-> SetDeepEnabled(true);
        PlayerWindow-> GetParent() -> SetVisible(true);
        PlayerWindow-> GetParent() -> SetBringToFront(true);

    }
    else
    {
        /// Load Player WIndow UI
        Existence-> loadUIXML(UIPLAYERWINDOW,200,200, button);
    }

    return;
}

void ExistenceClientStateMainScreen::ListenToLoad(StringHash eventType, VariantMap& eventData)
{
    /// Get Datas
    int  Status=eventData[LoadState::P_CMD].GetInt();
    String Arguments=eventData[LoadState::P_ARG].GetString();

    /// check if a a 100 was received
    if(Status==100)
    {
        /// Create a event
        VariantMap gamestatechange;
        gamestatechange[GameState::P_CMD] = GAME_STATE_PROGRESS;
        gamestatechange[GameState::P_ARG] = Arguments;

        cout << "Debug: Attempt to send a state change. Send Change State to Game from ListenToLoad" << endl;

        SendEvent(G_STATES_CHANGE,gamestatechange);
    }

    return;
}


