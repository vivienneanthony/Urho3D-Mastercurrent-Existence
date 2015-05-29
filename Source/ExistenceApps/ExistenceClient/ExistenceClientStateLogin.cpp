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

#include "GameStateHandler.h"
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


/// Constructor Destror
ExistenceClientStateLogin::ExistenceClientStateLogin(Urho3D::Context* context)
:ExistenceClient(context)
{
    cout << "show login " << endl;

    /// create UI
    LoginUI(true);
}

ExistenceClientStateLogin::~ExistenceClientStateLogin()
{
    //dtor
}


void ExistenceClientStateLogin::Enter()
{
    //dtor
}

void ExistenceClientStateLogin::Exit()
{
    //dtor
}

void ExistenceClientStateLogin::OnUpdate(StringHash eventType, VariantMap& eventData)
{
    //
}


/// code
void ExistenceClientStateLogin::LoginUI(bool exist)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// Set UI gamestate to logininterface
    ExistenceGameState->SetUIState(UI_LOGININTERFACE);

    /// Login screen - Create the Window and add it to the UI's root node
    window_= new Window(context_);

    uiRoot_->AddChild(window_);
    UIElement* usernameTextUIElement = new UIElement(context_);
    Text* usernameText = new Text(context_);
    LineEdit* usernameInput=new LineEdit(context_);
    UIElement* passwordTextUIElement = new UIElement(context_);
    Text* passwordText = new Text(context_);
    LineEdit* passwordInput=new LineEdit(context_);

    /// Set Window size and layout settings
    window_->SetMinSize(384, 192);
    window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window_->SetAlignment(HA_CENTER, VA_CENTER);
    window_->SetName("LoginWindow");
    window_->SetMovable(false);
    window_->SetOpacity(.6);

    /// Create Window 'titlebar' container
    usernameTextUIElement ->SetMinSize(0,32);
    usernameTextUIElement ->SetVerticalAlignment(VA_TOP);
    usernameTextUIElement ->SetLayoutMode(LM_HORIZONTAL);

    usernameText->SetName("Login");
    usernameText->SetText("Username");

    /// Create Window 'titlebar' container
    passwordTextUIElement ->SetMinSize(0,32);
    passwordTextUIElement ->SetVerticalAlignment(VA_TOP);
    passwordTextUIElement ->SetLayoutMode(LM_HORIZONTAL);

    passwordText->SetName("Password");
    passwordText->SetText("Password");

    usernameInput->SetName("usernameInput");
    usernameInput->SetText("<Enter Email>");
    usernameInput->SetMaxLength(24);
    usernameInput->SetMinHeight(24);
    usernameInput->SetStyleAuto();

    passwordInput->SetName("passwordInput");
    passwordInput->SetText("<Enter Password>");
    passwordInput->SetMaxLength(24);
    passwordInput->SetMinHeight(24);
    passwordInput->SetStyleAuto();

    /// Add the controls to the title bar
    usernameTextUIElement->AddChild(usernameText);
    passwordTextUIElement->AddChild(passwordText);
    window_->AddChild(usernameTextUIElement);
    window_->AddChild(usernameInput);
    window_->AddChild(passwordTextUIElement);
    window_->AddChild(passwordInput);

    /// declare buttons
    Button* loginButton = new Button(context_);
    Button* newaccountButton = new Button(context_);

    /// check if account exist
    if(accountexist)
    {
        loginButton->SetName("Login");
        loginButton->SetStyle("loginButton");
        window_->AddChild(loginButton);
    }
    else
    {
        newaccountButton->SetName("NewAccountLogin");
        newaccountButton->SetStyle("newaccountButton");
        window_->AddChild(newaccountButton);
    }

    /// Apply styles
    window_->SetStyleAuto();
    usernameText->SetStyleAuto();
    passwordText->SetStyleAuto();

    /// Attach handler based on new account - Temporary
    if(accountexist)
    {
        SubscribeToEvent(loginButton, E_RELEASED, HANDLER(ExistenceClientStateLogin, LoginScreenUILoginHandleClosePressed));
    }
    else
    {
        SubscribeToEvent(newaccountButton, E_RELEASED, HANDLER(ExistenceClientStateLogin, LoginScreenUINewAccountHandleClosePressed));
    }

    return;
}

// Handlers for login screen (Handler)
void ExistenceClientStateLogin::LoginScreenUILoginHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ExistenceGameState->SetUIState(UI_LOGININTERFACE);

    /// Get Needed SubSystems
    UI* ui_= GetSubsystem<UI>();

    UIElement * RootUIElement = ui_->GetRoot();

    LineEdit* lineEdit = (LineEdit*)ui_->GetRoot()->GetChild("usernameInput", true);
    String username = lineEdit->GetText();

    /// remove Existence Logo Node
    scene_->GetChild("ExistenceLogo",true)->RemoveAllComponents();
    scene_->GetChild("ExistenceLogo",true)->Remove();

    /// Call progress screen function
        //ProgressScreenUI();
        ExistenceGameState -> SendEvent("GAME_STATE_GAMEMODELOAD");

    return;
}

/// Login screen handler function
void ExistenceClientStateLogin::LoginScreenUINewAccountHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ExistenceGameState->SetUIState(UI_LOGININTERFACE);

    /// remove Existence Logo Node
    scene_->GetChild("ExistenceLogo",true)->RemoveAllComponents();
    scene_->GetChild("ExistenceLogo",true)->Remove();


    /// Call create play screen function
    //CreateAccountScreenUI();
    ExistenceGameState -> SendEvent("GAME_STATE_PLAYERCREATE");

    return;
}

