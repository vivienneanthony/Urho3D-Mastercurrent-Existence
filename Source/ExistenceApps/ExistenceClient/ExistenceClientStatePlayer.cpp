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
#include "ExistenceClientStatePlayer.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"


#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;


/// Constructor Destror
ExistenceClientStatePlayer::ExistenceClientStatePlayer(Urho3D::Context* context):
    ExistenceClientStateSingleton (context)
    ,Existence(baseclass)
{
    /// create UI
    Player();
}

ExistenceClientStatePlayer::
~ExistenceClientStatePlayer()
{
    //dtor
}

void ExistenceClientStatePlayer::OnUpdate(StringHash eventType, VariantMap& eventData)
{
    //
}

void ExistenceClientStatePlayer::Enter()
{
    //dtor
}

void ExistenceClientStatePlayer::Exit()
{
    //dtor
}

void ExistenceClientStatePlayer::Player(void)
{
    /// Set variables
    bool CurrentStateIsPlayer=true;

    /// Load the user interace
    CreatePlayerScreenUI();

    /// Loop
    do
    {
    }
    while(CurrentStateIsPlayer);

    return;
}

/// Create a player screen UI
void ExistenceClientStatePlayer::CreatePlayerScreenUI()
{

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Clear user interface
    ui->Clear();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// set ui state to none
    ExistenceGameState->SetUIState(UI_CHARACTERCREATIONINTERFACE);
    TemporaryPlayer.Clear();

    /// Load XML
    loadSceneCreationCreation("charactercreationroom1.xml");

    /// Login screen
    /// Create the Window and add it to the UI's root node
    window_= new Window(context_);
    window2_=new Window(context_);

    /// create first and secondary windows
    uiRoot_->AddChild(window_);
    uiRoot_->AddChild(window2_);

    UIElement* titleBar = new UIElement(context_);
    UIElement* contineButtonUIElement  = new UIElement(context_);
    UIElement* camerachangeorientationUIElement = new UIElement(context_);

    Text* windowTitle = new Text(context_);
    LineEdit* firstnameInput=new LineEdit(context_);
    LineEdit* middlenameInput=new LineEdit(context_);
    LineEdit* lastnameInput=new LineEdit(context_);
    Button* continueButton = new Button(context_);
    Button* createnewplayerfacezoomButton = new Button(context_);
    Button* camerachangeorientationButton = new Button(context_);

    /// Set Window size and layout settings
    window_->SetFixedSize(384, height-100-100);
    window_->SetLayout(LM_VERTICAL,12, IntRect(6, 6, 378, height-100-100));
    window_->SetPosition(30, 100);
    window_->SetName("PlayerCreatorWindow");
    window_->SetMovable(false);
    window_->SetOpacity(.8);

    window2_->SetName("FocusCreaterWindow");
    window2_->SetFixedSize(64,height-100-100);
    window2_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window2_->SetPosition(1024+256,100);
    window2_->SetMovable(false);
    window2_->SetOpacity(.6);

    /// Create Window 'titlebar' container
    titleBar->SetMinSize(0,32);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    windowTitle->SetName("PlayerCreatorTitle");
    windowTitle->SetText("Player Creator");

    contineButtonUIElement->SetMinSize(0,32);
    contineButtonUIElement->SetVerticalAlignment(VA_BOTTOM);
    continueButton ->SetPosition(6,300);

    contineButtonUIElement->AddChild(continueButton);

    createnewplayerfacezoomButton->SetPosition(700,200);
    createnewplayerfacezoomButton->SetName("createnewplayerfacezoomButton");
    createnewplayerfacezoomButton->SetStyle("createnewplayerfacezoomButton");

    /// Add the controls to the title bar
    titleBar->AddChild(windowTitle);

    /// add components to the window
    window_->AddChild(titleBar);

    window2_->AddChild(createnewplayerfacezoomButton);

    /// Apply styles
    window_->SetStyleAuto();
    window2_->SetStyleAuto();

    windowTitle->SetStyleAuto();

    /// Create a player node
    Node* playermeshNode = scene_->CreateChild("playerMesh");

    playermeshNode ->SetPosition(Vector3(0.0,0,0.0));
    playermeshNode ->SetRotation(Quaternion(0.0, 0.0,0.0));

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->GetChild("Camera");

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0,1.0,5.0));
    cameraNode_->SetRotation(Quaternion(0.0, 180.0,0.0));

    int windowwidth=384;
    int alienlimit=3;

    //Button* factionbutton = new Button(context_);
    UIElement * faction0buttonUIElement = new UIElement(context_);
    UIElement * faction1buttonUIElement = new UIElement(context_);
    UIElement * faction2buttonUIElement = new UIElement(context_);
    UIElement * faction3buttonUIElement = new UIElement(context_);

    Button * faction0button = new  Button(context_);
    Button * faction1button = new  Button(context_);
    Button * faction2button = new  Button(context_);
    Button * faction3button = new  Button(context_);

    UIElement * alien0buttonUIElement = new UIElement(context_);
    UIElement * alien1buttonUIElement = new UIElement(context_);
    UIElement * alien2buttonUIElement = new UIElement(context_);
    UIElement * alien3buttonUIElement = new UIElement(context_);

    Button * alien0button = new  Button(context_);
    Button * alien1button = new  Button(context_);
    Button * alien2button = new  Button(context_);
    Button * alien3button = new  Button(context_);

    Button * gendermalebutton = new  Button(context_);
    Button * genderfemalebutton = new  Button(context_);

    /// set faction an
    faction0button->SetName("factionButton 0");
    faction1button->SetName("factionButton 1");
    faction2button->SetName("factionButton 2");
    faction3button->SetName("factionButton 3");

    faction0buttonUIElement->SetName("factionButtonUIElement 0");
    faction1buttonUIElement->SetName("factionButtonUIElement 1");
    faction2buttonUIElement->SetName("factionButtonUIElement 2");
    faction3buttonUIElement->SetName("factionButtonUIElement 3");

    alien0button->SetName("alienButton 0");
    alien1button->SetName("alienButton 1");
    alien2button->SetName("alienButton 2");
    alien3button->SetName("alienButton 3");

    alien0button->SetEnabled(false);
    alien1button->SetEnabled(false);
    alien2button->SetEnabled(false);
    alien3button->SetEnabled(false);

    alien0buttonUIElement->SetName("alienButtonUIElement 0");
    alien1buttonUIElement->SetName("alienButtonUIElement 1");
    alien2buttonUIElement->SetName("alienButtonUIElement 2");
    alien3buttonUIElement->SetName("alienButtonUIElement 3");

    gendermalebutton->SetName("gendermaleButton");
    genderfemalebutton->SetName("genderfemaleButton");

    gendermalebutton->SetEnabled(false);
    genderfemalebutton->SetEnabled(false);

    /// set layout
    int area=windowwidth/4;

    UIElement * factionselectionUIElement = new UIElement(context_);
    factionselectionUIElement->SetFixedHeight(32);

    faction0buttonUIElement->SetFixedSize(area-2, 32);
    faction1buttonUIElement->SetFixedSize(area-2, 32);
    faction2buttonUIElement->SetFixedSize(area-2, 32);
    faction3buttonUIElement->SetFixedSize(area-6, 32);

    faction0buttonUIElement->SetPosition(0, 1);
    faction1buttonUIElement->SetPosition(area*1, 1);
    faction2buttonUIElement->SetPosition(area*2, 1);
    faction3buttonUIElement->SetPosition(area*3, 1);


    faction0buttonUIElement->AddChild(faction0button);
    faction1buttonUIElement->AddChild(faction1button);
    faction2buttonUIElement->AddChild(faction2button);
    faction3buttonUIElement->AddChild(faction3button);

    factionselectionUIElement->AddChild(faction0buttonUIElement);
    factionselectionUIElement->AddChild(faction1buttonUIElement);
    factionselectionUIElement->AddChild(faction2buttonUIElement);
    factionselectionUIElement->AddChild(faction3buttonUIElement);

    UIElement * alienselectionUIElement = new UIElement(context_);

    alienselectionUIElement->SetFixedHeight(32);

    alien0buttonUIElement->SetFixedSize(area-2, 32);
    alien1buttonUIElement->SetFixedSize(area-2, 32);
    alien2buttonUIElement->SetFixedSize(area-2, 32);
    alien3buttonUIElement->SetFixedSize(area-6, 32);

    alien0buttonUIElement->SetPosition(0, 1);
    alien1buttonUIElement->SetPosition(area*1, 1);
    alien2buttonUIElement->SetPosition(area*2, 1);
    alien3buttonUIElement->SetPosition(area*3, 1);

    alien0buttonUIElement->AddChild(alien0button);
    alien1buttonUIElement->AddChild(alien1button);
    alien2buttonUIElement->AddChild(alien2button);
    alien3buttonUIElement->AddChild(alien3button);

    alienselectionUIElement->AddChild(alien0buttonUIElement);
    alienselectionUIElement->AddChild(alien1buttonUIElement);
    alienselectionUIElement->AddChild(alien2buttonUIElement);
    alienselectionUIElement->AddChild(alien3buttonUIElement);

    UIElement * genderselectionUIElement = new UIElement(context_);
    UIElement * genderfemalebuttonUIElement = new UIElement(context_);
    UIElement * gendermalebuttonUIElement = new UIElement(context_);

    genderselectionUIElement->SetFixedHeight(32);

    genderfemalebuttonUIElement->SetFixedSize((area*2)-2, 32);
    gendermalebuttonUIElement->SetFixedSize((area*2)-2, 32);

    genderfemalebuttonUIElement->SetPosition(0,  1);
    gendermalebuttonUIElement->SetPosition(area*2, 1);

    genderfemalebuttonUIElement->AddChild(genderfemalebutton);
    gendermalebuttonUIElement->AddChild(gendermalebutton);

    genderselectionUIElement->AddChild(genderfemalebuttonUIElement);
    genderselectionUIElement->AddChild(gendermalebuttonUIElement);

    genderselectionUIElement->SetFixedSize(0,16);

    UIElement * blankUIElement = new UIElement(context_);
    Text * blankText = new Text(context_);

    blankUIElement->SetLayoutMode(LM_HORIZONTAL);
    blankUIElement->SetFixedHeight(32);

    blankText->SetName("blank");
    blankText->SetText(" ");

    blankUIElement-> AddChild(blankText);

    UIElement * playerUIElement = new UIElement(context_);
    Text * nameText = new Text(context_);

    playerUIElement->SetLayoutMode(LM_HORIZONTAL);

    nameText->SetName("Player");
    nameText->SetText("Player (First, Middle, Last)");

    playerUIElement-> AddChild(nameText);

    firstnameInput->SetName("firstnameInput");
    firstnameInput->SetText(String(GenerateName(0,0).c_str()));
    firstnameInput->SetMaxLength(24);
    firstnameInput->SetMinHeight(24);
    firstnameInput->SetFixedWidth(125);
    firstnameInput->SetStyleAuto();

    middlenameInput->SetName("middlenameInput");
    middlenameInput->SetText(String(GenerateName(0,0).c_str()));
    middlenameInput->SetMaxLength(24);
    middlenameInput->SetMinHeight(24);
    middlenameInput->SetFixedWidth(125);
    middlenameInput->SetStyleAuto();

    lastnameInput->SetName("lastnameInput");

    lastnameInput->SetText(String(GenerateName(0,0).c_str()));
    lastnameInput->SetMaxLength(24);
    lastnameInput->SetMinHeight(24);
    lastnameInput->SetFixedWidth(125);
    lastnameInput->SetStyleAuto();

    UIElement * playernameinputUIElement = new UIElement(context_);

    playernameinputUIElement->SetMinSize(0,32);
    playernameinputUIElement->SetLayoutMode(LM_HORIZONTAL);

    playernameinputUIElement->AddChild(firstnameInput);
    playernameinputUIElement->AddChild(middlenameInput);
    playernameinputUIElement->AddChild(lastnameInput);

    /// Personality
    UIElement * PersonalitytextUIElement = new UIElement(context_);
    Text * PersonalityText = new Text(context_);

    PersonalitytextUIElement->SetLayoutMode(LM_HORIZONTAL);

    PersonalityText->SetName("Personality");
    PersonalityText->SetText("Personality");

    PersonalitytextUIElement-> AddChild(PersonalityText);

    /// list view part
    UIElement * PersonalityUIElement1 = new UIElement(context_);
    UIElement * PersonalityUIElement2 = new UIElement(context_);
    UIElement * PersonalityUIElements = new UIElement(context_);

    ListView * PersonalitySelection = new ListView(context_);

    Text * Personality1 = new Text(context_);
    Text * Personality2 = new Text(context_);

    //Personality1->SetName("Stubborn");
    Personality1->SetName("Stubborn");
    Personality1->SetText("Stubborn");
    Personality2->SetName("Seductive");
    Personality2->SetText("Seductive");

    Personality1->SetEditable(false);
    Personality1->SetEnabled(true);
    Personality2->SetEditable(false);
    Personality2->SetEnabled(true);

    Personality1->SetSelectionColor (Color(0.0f,0.0f,0.5f));
    Personality1->SetHoverColor (Color(0.0f,0.0f,1.0f));
    Personality2->SetSelectionColor (Color(0.0f,0.0f,0.5f));
    Personality2->SetHoverColor (Color(0.0f,0.0f,1.0f));

    PersonalitySelection->SetFixedSize(300,64);
    PersonalitySelection->SetEditable(true);
    PersonalitySelection->SetName("PersonalitySelection");
    PersonalitySelection->SetEnabled(true);
    PersonalitySelection->SetHighlightMode(HM_ALWAYS);
    PersonalitySelection->SetSelectOnClickEnd(false);
    PersonalitySelection->SetClearSelectionOnDefocus(false);
    PersonalitySelection->SetMultiselect (false);

    PersonalitySelection->AddItem(Personality1);
    PersonalitySelection->AddItem(Personality2);

    PersonalityUIElements->SetLayoutMode(LM_HORIZONTAL);
    PersonalityUIElements->AddChild(PersonalitySelection);

    camerachangeorientationUIElement->SetPosition((width/2)-50, 128);
    camerachangeorientationUIElement->SetFixedSize(100, 56);
    camerachangeorientationUIElement->SetOpacity(0.8f);

    /// add the rest of the window elements
    window_->AddChild(factionselectionUIElement);
    window_->AddChild(alienselectionUIElement);
    window_->AddChild(genderselectionUIElement);
    window_->AddChild(blankUIElement);
    window_->AddChild(playerUIElement);
    window_->AddChild(playernameinputUIElement);
    window_->AddChild(blankUIElement);
    window_->AddChild(PersonalitytextUIElement);
    window_->AddChild(PersonalityUIElements);

    window_->AddChild(contineButtonUIElement);

    camerachangeorientationUIElement->AddChild(camerachangeorientationButton);

    uiRoot_->AddChild(camerachangeorientationUIElement);

    camerachangeorientationButton->SetStyleAuto();
    camerachangeorientationButton->SetStyle("cameraButton");
    camerachangeorientationButton->SetName("cameraorientationButton");


    nameText->SetStyleAuto();
    firstnameInput->SetStyleAuto();
    middlenameInput->SetStyleAuto();
    lastnameInput->SetStyleAuto();
    Personality1->SetStyleAuto();
    Personality2->SetStyleAuto();
    PersonalitySelection->SetStyleAuto();
    PersonalityText->SetStyleAuto();

    /// set button style
    faction0button->SetStyle("logofederationalliance");
    faction1button->SetStyle("logoromulanalliance");
    faction2button->SetStyle("logoklingonalliance");
    faction3button->SetStyle("logononalliance");

    alien0button->SetStyle("blankButton");
    alien0button->SetOpacity(0.6);
    alien1button->SetStyle("blankButton");
    alien1button->SetOpacity(0.6);
    alien2button->SetStyle("blankButton");
    alien2button->SetOpacity(0.6);
    alien3button->SetStyle("blankButton");
    alien3button->SetOpacity(0.6);

    genderfemalebutton->SetStyle("blankButton2");
    genderfemalebutton->SetOpacity(0.4);
    gendermalebutton->SetStyle("blankButton2");
    gendermalebutton->SetOpacity(0.4);

    continueButton->SetStyle("continueButton");

    Personality1->SetSelectionColor (Color(0.0f,0.0f,0.5f));
    Personality2->SetSelectionColor (Color(0.0f,0.0f,0.5f));

    PersonalitySelection->SetSelection(0);

    /// Subscribe to events
    SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClientStatePlayer, CreatePlayerUIHandleClosePressed));
    SubscribeToEvent(faction0button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(faction1button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(faction2button,  E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(faction3button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(alien0button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(alien1button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(alien2button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(alien3button, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(gendermalebutton, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(genderfemalebutton, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandleMouseReleased));
    SubscribeToEvent(PersonalitySelection, E_ITEMCLICKED,HANDLER(ExistenceClientStatePlayer,HandlePersonalitySelectionItemClick));
    SubscribeToEvent(camerachangeorientationButton, E_RELEASED, HANDLER(ExistenceClientStatePlayer, HandlerCameraOrientation));

    /// Subscribe also to all UI mouse clicks just to see where we have clicked
    SubscribeToEvent(E_UIMOUSECLICK, HANDLER(ExistenceClientStatePlayer, CreatePlayerUIHandleControlClicked));

    /// Temporary data
    playeralliance temporaryAlliance = TemporaryPlayer.GetAlliance();
    playercharacteristics temporaryCharacteristics = TemporaryPlayer.GetCharacteristics();

    int choosegender=rand()%99;
    temporaryCharacteristics.gender=choosegender+1;

    int choosefaction=rand()%3;
    choosefaction+=1;

    /// Choose a random faction
    switch (choosefaction)
    {
    case 1:
        temporaryAlliance.alienrace=1;
    case 2:
        temporaryAlliance.alienrace=100;
    case 3:
        temporaryAlliance.alienrace=200;
    case 4:
        temporaryAlliance.alienrace=1000;
    default:
        temporaryAlliance.alienrace=1;
    }

    /// Save information to temporary player
    TemporaryPlayer.SetAlliance(temporaryAlliance);
    TemporaryPlayer.SetCharacteristics(temporaryCharacteristics);

    /// Update character
    loadplayerMesh(playermeshNode, TemporaryPlayer.GetAlliance().alienrace, TemporaryPlayer.GetCharacteristics().gender,DISPLAYMESH_MUILTIPLECHARACTER);

    return;
}


/// Handle character selection
void ExistenceClientStatePlayer::HandlerCameraOrientation(StringHash eventType, VariantMap& eventData)
{

    /// get the button that was clicked
    Button* clicked = static_cast<Button*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());
    UI* ui_ = GetSubsystem<UI>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    Input* input=GetSubsystem<Input>();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// Get mouse position immediately
    IntVector2 PressedLocation=input->GetMousePosition();

    String clickedButtonString(clicked->GetName());

    string clickedtext;
    int clickedbutton=0;

    stringstream ss(clickedButtonString.CString());
    ss >> clickedtext >> clickedbutton;

    /// if button is not clicked
    if(clickedtext!="cameraorientationButton")
    {
        return;
    }

    /// Calculate mouse position
    signed int XPos=(PressedLocation.x_-(width/2));
    signed int YPos=PressedLocation.y_-128;

    /// Test if defined area for left right was clicked
    if((-46<XPos)&&(YPos>20)&&(XPos<-29)&&(YPos<33))
    {
        //cout << "left" << endl;
        CameraOrientationRotateMove(1,CAMERAORIENTATIONROTATEYAW);
    }

    /// Test if defined area for arrow right was clicked
    if((XPos>29)&&(YPos>20)&&(XPos<46)&&(YPos<33))
    {
        //cout << "left" << endl;
        CameraOrientationRotateMove(-1,CAMERAORIENTATIONROTATEYAW);
    }

    return;
}


/// Rotate a camera around a center point
void ExistenceClientStatePlayer::CameraOrientationRotateMove (float degrees, int movement)
{
    /// get the button that was clicked
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    Node * cameraNode_ = scene_->GetChild("Camera",true);
    Node * playermeshNode_ = scene_->GetChild("playermesh",true);

    /// Get position
    Vector3 playermeshPosition=playermeshNode_->GetWorldPosition();
    Vector3 Offset= playermeshPosition-cameraNode_->GetWorldPosition();

    /// Check to change camera yaw based on origin of the main object
    if(movement == CAMERAORIENTATIONROTATEYAW)
    {
        /// Create a offset
        Quaternion q = Quaternion(0.0f,1.0f*degrees,0.0);   // Construct rotation

        /// Test a offset of x equal 1
        Vector3 cameraOffset(5.0f, 0.0f, 0.0f);  // Camera offset relative to target node
        Vector3 cameraPosition =  playermeshPosition - (q * Offset);  // New rotated camera position with whatever offset you want

        /// Set the camera position
        cameraNode_->SetPosition(cameraPosition);  // Set new camera position and lookat values
    }

    /// Look at orgin
    cameraNode_->LookAt(Vector3(0.0f,1.0f,0.0f) );

    return;
}


/// Handle character selection
void ExistenceClientStatePlayer::HandleMouseReleased(StringHash eventType, VariantMap& eventData)
{

    /// get the button that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());
    UI* ui_ = GetSubsystem<UI>();


    /// Temporary data
    playeralliance temporaryAlliance = TemporaryPlayer.GetAlliance();
    playercharacteristics temporaryCharacteristics = TemporaryPlayer.GetCharacteristics();

    /// Temporarily define faction information (Might make this a class)
    unsigned int factionslimit=4;
    unsigned int alienslimit=4;

    factions Faction[factionslimit];
    alienraces Alien[alienslimit];

    unsigned int selectedcurrentfaction;

    /// Define structure
    Faction[0].uniqueid=1;
    Faction[0].name.append("Federation");
    Faction[0].button.append("logofederationalliance");
    Faction[0].alliedraces.append("1");

    Faction[1].uniqueid=100;
    Faction[1].name.append("Romulan Republic");
    Faction[1].button.append("logoromulanalliance");
    Faction[1].alliedraces.append("100");

    Faction[2].uniqueid=200;
    Faction[2].name.append("Klingon New Order");
    Faction[2].button.append("logoklingonalliance");
    Faction[2].alliedraces.append("200");

    Faction[3].uniqueid=1000;
    Faction[3].name.append("Independent Republics");
    Faction[3].button.append("logononalliance");
    Faction[3].alliedraces.append("1000");

    Alien[0].uniqueid=1;
    Alien[0].name.append("humans");
    Alien[0].button.append("logohumans");

    Alien[1].uniqueid=100;
    Alien[1].name.append("romulans");
    Alien[1].button.append("logoromulans");

    Alien[2].uniqueid=200;
    Alien[2].name.append("klingons");
    Alien[2].button.append("logoklingons");

    Alien[3].uniqueid=1000;
    Alien[3].name.append("orcins");
    Alien[3].button.append("logoorcins");

    String clickedButtonString(clicked->GetName());

    string clickedtext;
    unsigned int clickedbutton=0;

    stringstream ss(clickedButtonString.CString());
    ss >> clickedtext >> clickedbutton;

    /// Temporary Button
    string temporaryButtonName;


    unsigned int aliensarraysize;

    aliensarray.reserve(16);
    tempaliensarray.reserve(16);

    /// Verify if faction was pressed
    if(clickedtext=="factionButton")
    {
        /// Loop through each faction
        for(unsigned int factioncounter=0; factioncounter<factionslimit; factioncounter++)
        {

            /// configure faction
            std::string s;
            std::stringstream out;
            out << factioncounter;
            s = out.str();

            /// Create new name
            temporaryButtonName.append("factionButtonUIElement ");
            temporaryButtonName.append(s.c_str());

            /// Create a button
            UIElement* CurrentButton = (UIElement*)ui_->GetRoot()->GetChild(temporaryButtonName.c_str(), true);

            /// Check each button individually
            if(factioncounter==clickedbutton)
            {
                CurrentButton->SetOpacity(0.8f);

                /// Set current faction
                selectedcurrentfaction=clickedbutton;
            }
            else
            {
                CurrentButton->SetOpacity(0.3f);
            }

            /// clear string
            temporaryButtonName.clear();
        }

        /// Copy alliances to array
        vector<string> factionallianceinput = split(Faction[selectedcurrentfaction].alliedraces, " ");

        /// Allocate memory
        aliensarray.clear();
        tempaliensarray.clear();

        aliensarraysize=factionallianceinput.size();


        /// Copy alien race per faction to alien array and temp array
        for(unsigned int idx=0; idx<factionallianceinput.size(); idx++)
        {
            aliensarray.push_back(factionallianceinput.at(idx));//Copy the vector to the string
            tempaliensarray.push_back(factionallianceinput.at(idx));//Copy the vector to the string
        }

        /// Set Default based on new alliance
        unsigned int num;
        std::istringstream {aliensarray[0]} >> num;

        /// Set default alienrace and gender
        temporaryAlliance.alienrace=num;
        temporaryCharacteristics.gender=50;

        /// loop through all the buttons and clear them
        for (unsigned int temporarybutton=0; temporarybutton<4; temporarybutton++)
        {
            /// Clear Name
            temporaryButtonName.clear();

            /// configure faction
            std::string s;
            std::stringstream out;
            out << temporarybutton;
            s = out.str();

            /// Create new name
            temporaryButtonName.append("alienButton ");
            temporaryButtonName.append(s.c_str());

            Button* CurrentButton = (Button*)ui_->GetRoot()->GetChild(temporaryButtonName.c_str(), true);

            CurrentButton->SetStyle("blankButton");
            CurrentButton->SetEnabled(false);

            CurrentButton->SetOpacity(0.3f);
        }

        /// Set all variables
        unsigned int alienarraycounter=0;
        unsigned int button=0;
        unsigned int aliencounter=0;

        /// loop through first 4 choices of aliens
        do
        {

            /// Get informatio to update
            for(unsigned int alienrace=0; alienrace<4; alienrace++)
            {

                string temporaryalien=ConvertUIntToString(Alien[alienrace].uniqueid);

                /// Compare temporary alien from main array to created array
                if(temporaryalien==aliensarray.at(alienarraycounter))
                {

                    /// configure faction
                    std::string s;
                    std::stringstream out;
                    out << button;
                    s = out.str();

                    /// Avoid crash
                    if(button==0)
                    {
                        s="0";
                    }

                    /// Create new name
                    temporaryButtonName.clear();
                    temporaryButtonName.append("alienButton ");
                    temporaryButtonName.append(s.c_str());

                    /// Create a button
                    Button* CurrentButton = (Button*)ui_->GetRoot()->GetChild(temporaryButtonName.c_str(), true);

                    /// for some reason this worked
                    String testline(Alien[alienrace].button.c_str());

                    /// Change the logo to match
                    CurrentButton->SetOpacity(1.0f);
                    CurrentButton->SetStyle(testline);
                    CurrentButton->SetEnabled(true);

                    button++;
                }
            }

            /// Go to the next in array
            alienarraycounter++;
        }
        while(alienarraycounter<aliensarraysize);
    }

    /// Verify if faction was pressed
    if(clickedtext=="alienButton")
    {
        /// Set Default based on new alliance
        unsigned int num=0;

        std::istringstream {tempaliensarray.at(clickedbutton).c_str()} >> num;

        temporaryAlliance.alienrace=num;

    }

    /// Verify if faction was pressed
    if(clickedtext=="genderfemaleButton")
    {

        temporaryCharacteristics.gender=1;
    }

    /// Verify if faction was pressed
    if(clickedtext=="gendermaleButton")
    {

        temporaryCharacteristics.gender=99;
    }

    TemporaryPlayer.SetCharacteristics(temporaryCharacteristics);
    TemporaryPlayer.SetAlliance(temporaryAlliance);

    /// Get child
    Node* playermeshNode = scene_->GetChild("playerMesh");

    playermeshNode->RemoveAllChildren();

    if(clickedtext=="factionButton")
    {
        loadplayerMesh(playermeshNode, TemporaryPlayer.GetAlliance().alienrace, TemporaryPlayer.GetCharacteristics().gender,DISPLAYMESH_MUILTIPLECHARACTER);
    }
    else     if(clickedtext=="alienButton")
    {
        loadplayerMesh(playermeshNode, TemporaryPlayer.GetAlliance().alienrace, TemporaryPlayer.GetCharacteristics().gender,DISPLAYMESH_MUILTIPLECHARACTER);
    }


    else
    {
        loadplayerMesh(playermeshNode, TemporaryPlayer.GetAlliance().alienrace, TemporaryPlayer.GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);

    }

    Button* genderfemalebutton = (Button*)ui_->GetRoot()->GetChild("genderfemaleButton", true);
    Button* gendermalebutton = (Button*)ui_->GetRoot()->GetChild("gendermaleButton", true);

    genderfemalebutton->SetStyle("genderfemaleButton");
    genderfemalebutton->SetOpacity(0.9);
    genderfemalebutton->SetEnabled(true);
    gendermalebutton->SetStyle("gendermaleButton");
    gendermalebutton->SetOpacity(0.9);
    gendermalebutton->SetEnabled(true);

    return;
}

/// Handle creater screen UI close pressed
void ExistenceClientStatePlayer::CreatePlayerUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{

    /// Set ui state to character creation
    ExistenceGameState->SetUIState(UI_CHARACTERCREATIONINTERFACE);

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    /// remove child nodeAddItem (UIElement *item)
    scene_->GetChild("playerMesh",true)->RemoveAllComponents();
    scene_->GetChild("playerMesh",true)->Remove();

    /// Copy information/// Get line objects from Urho3d
    LineEdit* firstnameLineEdit = (LineEdit*)ui_->GetRoot()->GetChild("firstnameInput", true);
    LineEdit* middlenameLineEdit = (LineEdit*)ui_->GetRoot()->GetChild("middlenameInput", true);
    LineEdit* lastnameLineEdit= (LineEdit*)ui_->GetRoot()->GetChild("lastnameInput", true);

    ListView* PersonalitySelectionListView= (ListView*)ui_->GetRoot()->GetChild("PersonalitySelection", true);

    unsigned int personaltyselection= PersonalitySelectionListView->GetSelection();

    String firstnameInput = firstnameLineEdit->GetText();
    String middlenameInput = middlenameLineEdit->GetText();
    String lastnameInput = lastnameLineEdit->GetText();

    /// Temporay player Info
    playerbasicinfo tempplayerinfo;
    playercharacteristics tempplayercharacteristics=TemporaryPlayer.GetCharacteristics();

    /// Copy string information
    tempplayerinfo.firstname=firstnameInput.CString();
    tempplayerinfo.lastname=lastnameInput.CString();
    tempplayerinfo.middlename=middlenameInput.CString();

    tempplayercharacteristics.personalitytrait=personaltyselection;

    /// Set player information
    TemporaryPlayer.SetCharacteristics(tempplayercharacteristics);
    TemporaryPlayer.SetPlayerInfo(tempplayerinfo);

    /// Save player information
    SavePlayer(0);

    eraseScene();

    SetupScreenViewport();

    //ProgressScreenUI();
    ExistenceGameState -> SendEvent("GAME_STATE_GAMEMODELOAD");

}

/// Character zoom mode pressed
void ExistenceClientStatePlayer::CreatePlayerUIHandleControlClicked(StringHash eventType, VariantMap& eventData)
{

    /// Get control that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    /// Check for a click and which button
    if(clicked)
    {
        ///        if(clicked->GetName()=="createnewplayerfacezoomButton")
        ///     {
        ///      scene_->GetChild("playerMesh",true)->SetScale(2);
        ///}
    }
}


/// Handler for personality selection item click
void ExistenceClientStatePlayer::HandlePersonalitySelectionItemClick(StringHash eventType, VariantMap& eventData)
{

    ///Get the UI interface information as a pointer
    UI* ui_ = GetSubsystem<UI>();

    /// ListView
    ListView* PersonalitySelection = (ListView*)ui_->GetRoot()->GetChild("PersonalitySelection", true);

    /// Get selection information
    unsigned int selectionIndex = eventData["Selection"].GetUInt();

    /// Set selection for new personality clicked
    PersonalitySelection -> SetSelection(selectionIndex);

    return;
}



/// Generate a name
string GenerateName(char group, char subgroup)
{
    /// Create a string
    string temporaryname;

    /// Create a base an stem limit and vector
    unsigned int namebasesize=12;
    vector<string> namebase(namebasesize);

    unsigned int namestemsize=12;
    vector<string> namestem(namestemsize);

    /// Create a base and stem array
    string namebases[]= {"A","B","C","D","L'l","X","Jes","Nex","R","N","L","M"};
    string namestems[]= {"on","el","ul","axob","uc","ath","'rth","elys","iel","loe","mma","oe"};

    /// Copy base
    for(unsigned int idx=0; idx<namebasesize; idx++)
    {
        namebase.at(idx)=namebases[idx];
    }

    /// Copy stem
    for(unsigned int idx=0; idx<namestemsize; idx++)
    {
        namestem.at(idx)=namestems[idx];
    }

    /// Select random base and stem
    unsigned int randombase= rand()%namebasesize;
    unsigned int randomstemsamount = rand()%2;
    unsigned int randomstem;

    /// Create a name
    string generatedname;

    generatedname.append(namebase.at(randombase));

    /// Always add 1 stem
    randomstemsamount+=1;

    /// Add Stems to the name
    for(unsigned int count=0; count<randomstemsamount; count++)
    {
        randomstem= rand()%namestemsize;
        generatedname.append(namestem.at(randomstem));
    }

    return generatedname;
}


// Load a scene
void ExistenceClientStatePlayer::loadSceneCreationCreation( const char * lineinput)
{
    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();


    /// Create variables (urho3d)
    String InputDataFile;

    InputDataFile.Append(filesystem->GetProgramDir().CString());
    InputDataFile.Append("Resources/Scenes/");
    InputDataFile.Append(lineinput);

    /// Check if the input data file exist
    if(filesystem->FileExists(InputDataFile))
    {
        /// Open file as a Urho3d Datafile
        File dataFile(context_, InputDataFile, FILE_READ);

        if (dataFile.IsOpen())
        {

            /// Get File Extension
            String extension = GetExtension(InputDataFile);

            /// Load File based on Extension

            if (extension != ".xml")
            {
                scene_ -> Load(dataFile);
            }
            else
            {
                scene_ ->LoadXML(dataFile);
            }

        }

    }
    else
    {
        /// Load dummy scene
//        loadDummyScene();
    }


    /// Get the Camera Node and setup the viewport
    Node * cameraNode_ = scene_->GetChild("Camera");

    /// Change viewport to camera node
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}
