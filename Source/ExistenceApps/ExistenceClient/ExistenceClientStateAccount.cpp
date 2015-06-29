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
#include "ExistenceClientStateAccount.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"


#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;


/// State Account Constructor
ExistenceClientStateAccount::ExistenceClientStateAccount(Context* context):
    ExistenceClientStateSingleton (context)
    ,Existence(NULL)
{
    /// Debug
    cout << "Debug: State Account Constructor" << endl;

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();
    /// Set aApplication
    Existence = gamestatehandlercomponent_->GetApplication();

    /// Set UI State
    gamestatehandlercomponent_->SetUIState(UI_ACCOUNTCREATIONINTERFACE);

    /// Debug
    cout << "Debug: State Account Constructor Test Value " << Existence->testvalue << endl;

    return;
}

/// State Account Desctructor
ExistenceClientStateAccount::~ExistenceClientStateAccount()
{
     /// Debug
    cout << "Debug: State Account Deconstructor" << endl;

    return;
}

/// State Account Enter
void ExistenceClientStateAccount::Enter()
{
    /// Debug
    cout << "Debug: State Account Enter" << endl;

    /// create UI
    Account();

    return;
}

/// State Account Exit
void ExistenceClientStateAccount::Exit()
{
    /// Debug
    cout << "Debug: State Account Exit" << endl;

    return;
}

/// State Account On Update
void ExistenceClientStateAccount::OnUpdate(StringHash eventType, VariantMap& eventData)
{
       /// Debug
    cout << "Debug: State Account OnUpdate" << endl;

    return;
}

void ExistenceClientStateAccount::SetParameter(String parameter_)
{
    /// Do Nothing
    return;
}

/// State Account UI
void ExistenceClientStateAccount::Account(void)
{
    /// Set variables
    bool CurrentStateIsLogin=true;

    /// Load the user interace
    CreateAccountScreenUI();

     return;
}


/// Create a account UI
void ExistenceClientStateAccount::CreateAccountScreenUI(void)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Clear screen
    ui->Clear();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// Create the Window and add it to the UI's root node
    Existence->window_= new Window(context_);
    Existence->uiRoot_->AddChild(Existence->window_);

    /// Define UIElements and childrens types
    UIElement* titleBar = new UIElement(context_);
    UIElement* confirmpassword1UIElement = new UIElement(context_);
    UIElement* confirmpassword2UIElement = new UIElement(context_);
    UIElement* playerUIElement = new UIElement(context_);
    UIElement* playernameinputUIElement = new UIElement(context_);

    UIElement* blankUIElement = new UIElement(context_);
    Text* blankText = new Text(context_);

    UIElement* blank2UIElement = new UIElement(context_);
    Text* blank2Text = new Text(context_);

    Button* newaccount2Button = new Button(context_);
    Text* windowTitle = new Text(context_);
    LineEdit* usernameInput=new LineEdit(context_);
    LineEdit* emailInput=new LineEdit(context_);
    LineEdit* passwordInput1=new LineEdit(context_);
    LineEdit* passwordInput2=new LineEdit(context_);
    LineEdit* firstnameInput=new LineEdit(context_);
    LineEdit* middlenameInput=new LineEdit(context_);
    LineEdit* lastnameInput=new LineEdit(context_);
    Text* confirmpassword1Text = new Text(context_);
    Text* nameText = new Text(context_);
    Text* confirmpassword2Text = new Text(context_);
    Text* emailText = new Text(context_);

    /// Setup alignments
    titleBar->SetMinSize(0,32);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    confirmpassword1UIElement->SetLayoutMode(LM_HORIZONTAL);
    confirmpassword2UIElement->SetLayoutMode(LM_HORIZONTAL);
    playerUIElement->SetLayoutMode(LM_HORIZONTAL);

    playernameinputUIElement->SetMinSize(0,32);
    playernameinputUIElement->SetLayoutMode(LM_HORIZONTAL);

    blankUIElement->SetLayoutMode(LM_HORIZONTAL);
    blankUIElement->SetFixedHeight(16);

    /// Set Window size and layout settings
    Existence ->window_->SetMinSize(384, 192);
    Existence ->window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    Existence ->window_->SetAlignment(HA_CENTER, VA_CENTER);
    Existence ->window_->SetName("CreateAccountWindow");
    Existence ->window_->SetMovable(false);
    Existence ->window_->SetOpacity(.6);

    /// Create Window 'titlebar' container
    windowTitle->SetName("CreateAccount");
    windowTitle->SetText("Username");

    usernameInput->SetName("usernameInput");
    usernameInput->SetText("<Enter Username>");
    usernameInput->SetMaxLength(24);
    usernameInput->SetMinHeight(24);

    emailText->SetName("emailText");
    emailText->SetText("Email");

    emailInput->SetName("emailInput");
    emailInput->SetText("<Enter Email>");
    emailInput->SetMaxLength(24);
    emailInput->SetMinHeight(24);

    /// Setup first password text
    confirmpassword1Text->SetName("Password");
    confirmpassword1Text->SetText("Password");

    passwordInput1->SetName("passwordInput1");
    passwordInput1->SetText("<Enter Password>");
    passwordInput1->SetMaxLength(24);
    passwordInput1->SetMinHeight(24);

    /// Setup confirm password text
    confirmpassword2Text->SetName("Password");
    confirmpassword2Text->SetText("Confirm Password");

    passwordInput2->SetName("passwordInput2");
    passwordInput2->SetText("<Enter Password2>");
    passwordInput2->SetMaxLength(24);
    passwordInput2->SetMinHeight(24);

    /// Add the controls to the title bar
    confirmpassword1UIElement-> AddChild(confirmpassword1Text);
    confirmpassword2UIElement-> AddChild(confirmpassword2Text);

    nameText->SetName("Name");
    nameText->SetText("Name");

    firstnameInput->SetName("firstnameInput");
    firstnameInput->SetText("<firstname>");
    firstnameInput->SetMaxLength(24);
    firstnameInput->SetMinHeight(24);
    firstnameInput->SetFixedWidth(125);

    middlenameInput->SetName("middlenameInput");
    middlenameInput->SetText("<middlename>");
    middlenameInput->SetMaxLength(24);
    middlenameInput->SetMinHeight(24);
    middlenameInput->SetFixedWidth(125);

    lastnameInput->SetName("lastnameInput");
    lastnameInput->SetText("<lastname>");
    lastnameInput->SetMaxLength(24);
    lastnameInput->SetMinHeight(24);
    lastnameInput->SetFixedWidth(125);

    blankText->SetName("blank");
    blankText->SetText(" ");

    blank2Text->SetName("blank2");
    blank2Text->SetText(" ");

    /// Add Children
    titleBar->AddChild(windowTitle);
    Existence->window_->AddChild(titleBar);
    Existence->window_->AddChild(usernameInput);
    Existence->window_->AddChild(emailText);
    Existence->window_->AddChild(emailInput);
    Existence->window_->AddChild(confirmpassword1UIElement);
    Existence->window_->AddChild(passwordInput1);
    Existence->window_->AddChild(confirmpassword2UIElement);
    Existence->window_->AddChild(passwordInput2);

    blankUIElement-> AddChild(blankText);
    Existence->window_->AddChild(blankUIElement);

    blank2UIElement-> AddChild(blank2Text);

    playerUIElement-> AddChild(nameText);

    playernameinputUIElement->AddChild(firstnameInput);
    playernameinputUIElement->AddChild(middlenameInput);
    playernameinputUIElement->AddChild(lastnameInput);

    /// declare buttons
    newaccount2Button->SetName("NewAccountLogin");
    newaccount2Button->SetStyle("continueButton");

    /// add children
    Existence ->window_->AddChild(playerUIElement);
    Existence ->window_->AddChild(playernameinputUIElement);
    Existence ->window_->AddChild(blank2UIElement);

    Existence->window_->AddChild(newaccount2Button);

    /// Apply styles
    Existence->window_->SetStyleAuto();
    windowTitle->SetStyleAuto();
    confirmpassword1Text->SetStyleAuto();
    confirmpassword2Text->SetStyleAuto();
    passwordInput2->SetStyleAuto();
    passwordInput1->SetStyleAuto();
    nameText->SetStyleAuto();
    emailText->SetStyleAuto();
    emailInput->SetStyleAuto();
    firstnameInput->SetStyleAuto();
    middlenameInput->SetStyleAuto();
    lastnameInput->SetStyleAuto();
    usernameInput->SetStyleAuto();
    blank2Text->SetStyleAuto();
    blankText->SetStyleAuto();

    /// Subscribe to Handler
    SubscribeToEvent(newaccount2Button, E_RELEASED, HANDLER(ExistenceClientStateAccount, CreateAccountUIHandleClosePressed));

    return;
}

/// Create Accooount UI  Handler
void ExistenceClientStateAccount::CreateAccountUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// Get Needed SubSystems
    UI* ui_ = GetSubsystem<UI>();
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();


    /// Get line objects
    LineEdit* firstnameLineEdit = (LineEdit*)ui_->GetRoot()->GetChild("firstnameInput", true);
    LineEdit* middlenameLineEdit = (LineEdit*)ui_->GetRoot()->GetChild("middlenameInput", true);
    LineEdit* lastnameLineEdit= (LineEdit*)ui_->GetRoot()->GetChild("lastnameInput", true);
    LineEdit* usernameLineEdit= (LineEdit*)ui_->GetRoot()->GetChild("usernameInput", true);
    LineEdit* password1LineEdit = (LineEdit*)ui_->GetRoot()->GetChild("passwordInput1", true);
    LineEdit* password2LineEdit = (LineEdit*)ui_->GetRoot()->GetChild("passwordInput2", true);
    LineEdit* emailLineEdit = (LineEdit*)ui_->GetRoot()->GetChild("emailInput", true);

    String firstnameInput = firstnameLineEdit->GetText();
    String middlenameInput = middlenameLineEdit->GetText();
    String lastnameInput = lastnameLineEdit->GetText();
    String usernameInput = usernameLineEdit->GetText();
    String password1Input = password1LineEdit->GetText();
    String password2Input = password2LineEdit->GetText();
    String emailInput = emailLineEdit->GetText();

    accountinformation account;

    account.email=emailInput.CString();
    account.username=usernameInput.CString();
    account.firstname=firstnameInput.CString();
    account.lastname=lastnameInput.CString();
    account.middlename=middlenameInput.CString();
    account.password=password1Input.CString();

    /// Save Account Information
    Existence -> SaveAccount(account);

    /// Erase the UI
    Existence -> EraseUI();


    ///ExistenceGameState -> SendEvent("GAME_STATE_PLAYERCREATE");
    return;
}

