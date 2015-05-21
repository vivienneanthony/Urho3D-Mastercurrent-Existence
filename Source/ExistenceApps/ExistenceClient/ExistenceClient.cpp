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
#include "../PlayerLevels.h"

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

}

vector<string> split(const string& s, const string& delim, const bool keep_empty = true)
{
    vector<string> result;
    if (delim.empty())
    {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true)
    {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty())
        {
            result.push_back(temp);
        }
        if (subend == s.end())
        {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}


time_t ConvertStringToTime(const char * buff, time_t timeseed);

time_t ConvertStringToTime(const char * buff, time_t timeseed)
{
    /// set up needed variables
    int yy, mm, dd, hour, min, sec;
    struct tm when;
    time_t tme;

    /// return failure if it can't be converted
    if(!sscanf(buff, "%d:%d:%d:%d:%d:%d", &mm, &dd, &yy, &hour, &min, &sec))
    {
        return 0;
    }

    /// copy structure
    when.tm_year = yy;
    when.tm_mon = mm-1;
    when.tm_mday = dd;
    when.tm_hour = hour;
    when.tm_min = min;
    when.tm_sec = sec;

    return mktime(&when);
}

string GenerateName(char group, char subgroup);

string ConvertUIntToString(unsigned int val)
{
    char buff[32];
    sprintf(buff, "%d", val);
    string str = buff;
    return(str);
}

float cutoff(float inputvalue, float pointmid, float range,bool debug);

/// Fuction Cutoff
/// Take input from a slope for lerping to gradiant
float cutoff(float inputvalue, float pointmid, float range, bool debug)
{
    /// Create valuables to calculate
    float midpoint=pointmid;

    float midpoint_low=midpoint-range;
    float midpoint_high=midpoint+range;
    float midpoint_range=midpoint_range=midpoint_high-midpoint_low;

    float result;

    /// Double check for overaflow
    if(midpoint_low<0.0f)
    {
        midpoint_low=0.0f;
    }
    if(midpoint_high>1.0f)
    {
        midpoint_high=1.0f;
    }


    /// Calculate gradiant based on lerp results
    if(midpoint_low>inputvalue)
    {
        result=0;
    }
    else if(inputvalue>midpoint_high)
    {
        result=1;

    }
    else
    {
        result=(inputvalue-midpoint_low)/midpoint_range;
    }

    return result;
}

/// Function StringtoFloat
/// Strign to Float
float StringToFloat(string buffer)
{

    istringstream bufferinput(buffer);
    float temp;
    bufferinput >> temp;
    return temp;
}

// using range = std::pair<float,float> ; // C++11
typedef std::pair<float,float> range ;

// range make_range( float a, float b ) { return { std::min(a,b), std::max(a,b) } ; } // C++11
range make_range( float a, float b )
{
    return range( std::min(a,b), std::max(a,b) ) ;
}

bool intersects( range a, range b )
{
    if( a > b ) std::swap(a,b) ;
    return a.second >= b.first ;
}

Vector3 NormalizedToWorld(Image *height, Terrain *terrain, Vector2 normalized);

Vector3 NormalizedToWorld(Image *height, Terrain *terrain, Vector2 normalized)
{
    if(!terrain || !height) return Vector2(0,0);
    Vector3 spacing=terrain->GetSpacing();
    int patchSize=terrain->GetPatchSize();
    IntVector2 numPatches=IntVector2((height->GetWidth()-1) / patchSize, (height->GetHeight()-1) / patchSize);
    Vector2 patchWorldSize=Vector2(spacing.x_*(float)(patchSize*numPatches.x_), spacing.z_*(float)(patchSize*numPatches.y_));
    Vector2 patchWorldOrigin=Vector2(-0.5f * patchWorldSize.x_, -0.5f * patchWorldSize.y_);
    return Vector3(patchWorldOrigin.x_+normalized.x_*patchWorldSize.x_, 0, patchWorldOrigin.y_+normalized.y_*patchWorldSize.y_);
}

/// Main program execution code
void ExistenceClient::Start()
{
    /// Execute base class startup
    ExistenceApp::Start();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_ = GetSubsystem<EnvironmentBuild>();

    ///Manager* manager = GetSubsystem<Manager>();

    UI* ui = GetSubsystem<UI>();



    /// create variables (urho3d)
    String additionresourcePath;

    additionresourcePath.Append(filesystem->GetProgramDir().CString());
    additionresourcePath.Append("Resources/");

    /// add resource path to last
    cache -> AddResourceDir(additionresourcePath);

    /// Initialize rudimentary state handler
    ExistenceGameState.Start();

    /// Configure rudimentary state handler
    ExistenceGameState.SetUIState(UI_NONE);
    ExistenceGameState.SetGameState(STATE_MAIN);

    /// Set the loaded style as default style
    uiRoot_->SetDefaultStyle(style);

    CreateCursor();

    ui->GetCursor()->SetVisible(true);

    /// Set game active status
    accountexist=false;

    /// Setup Screen and Viewport
    SetupScreenViewport();
    AddLogoViewport();

    /// Initialize Console
    InitializeConsole();

    GetSubsystem<Input>()->Update();

    /// Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    /// load account
    LoadAccount();

    /// Star+t Login UI
    LoginUI(accountexist);


    /// Finally subscribe to the update event. Note that by subscribing events at this point we have already missed some events
    /// like the ScreenMode event sent by the Graphics subsystem when opening the application window. To catch those as well we
    /// could subscribe in the constructor instead.
    SubscribeToEvents();

    /// Randomize timer
    srand (time(NULL));

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
    SubscribeToEvent(E_KEYDOWN, HANDLER(ExistenceClient, HandleKeyDown));

    ///  Handle Post Updates
    SubscribeToEvent(E_POSTUPDATE,HANDLER(ExistenceClient,HandlePostUpdates));
}

/// Handle post updates
void ExistenceClient::HandlePostUpdates(StringHash eventType, VariantMap& eventData)
{
    UI* ui = GetSubsystem<UI>();

    /// check if in game mode
    if(ExistenceGameState.GetGameState()==STATE_GAME)
    {
        Sprite* healthBar = (Sprite*)ui->GetRoot()->GetChild("PlayerInfoHealthBarIndicate", true);

        if(healthBar!=NULL)
        {

            float scale=character_->GetHealth()/100;

            healthBar -> SetScale(scale,1.0f);
        }
    }
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

// Log UI Code
void ExistenceClient::LoginUI(bool exist)
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
    ExistenceGameState.SetUIState(UI_LOGININTERFACE);

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
        SubscribeToEvent(loginButton, E_RELEASED, HANDLER(ExistenceClient, LoginScreenUILoginHandleClosePressed));
    }
    else
    {
        SubscribeToEvent(newaccountButton, E_RELEASED, HANDLER(ExistenceClient, LoginScreenUINewAccountHandleClosePressed));
    }

    return;
}

// Handlers for login screen (Handler)
void ExistenceClient::LoginScreenUILoginHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ExistenceGameState.SetUIState(UI_LOGININTERFACE);

    /// Get Needed SubSystems
    UI* ui_= GetSubsystem<UI>();

    UIElement * RootUIElement = ui_->GetRoot();

    LineEdit* lineEdit = (LineEdit*)ui_->GetRoot()->GetChild("usernameInput", true);
    String username = lineEdit->GetText();

    /// remove Existence Logo Node
    scene_->GetChild("ExistenceLogo",true)->RemoveAllComponents();
    scene_->GetChild("ExistenceLogo",true)->Remove();

    /// Call progress screen function
    ProgressScreenUI();

    return;
}

/// Login screen handler function
void ExistenceClient::LoginScreenUINewAccountHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ExistenceGameState.SetUIState(UI_LOGININTERFACE);

    /// remove Existence Logo Node
    scene_->GetChild("ExistenceLogo",true)->RemoveAllComponents();
    scene_->GetChild("ExistenceLogo",true)->Remove();


    /// Call create play screen function
    CreateAccountScreenUI();

    return;
}

/// Create progress screen UI
void ExistenceClient::ProgressScreenUI()
{

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    ui->Clear();

    /// set ui state to none
    ExistenceGameState.SetUIState(UI_PROGRESSINTERFACE);

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// Create the Window and add it to the UI's root node
    window_= new Window(context_);

    uiRoot_->AddChild(window_);
    UIElement* titleBar = new UIElement(context_);
    Text* windowTitle = new Text(context_);
    Text* progressText=new Text(context_);
    Button* continueButton = new Button(context_);

    /// Set Window size and layout settings
    window_->SetMinSize(384, 192);
    window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window_->SetAlignment(HA_CENTER, VA_CENTER);
    window_->SetName("LoginWindow");
    window_->SetMovable(false);
    window_->SetOpacity(.6);

    /// Create Window 'titlebar' container
    titleBar->SetMinSize(0,32);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    windowTitle->SetName("Login");
    windowTitle->SetText("Accessing Server");

    progressText->SetName("progressText");
    progressText->SetText("Loading ghost characters states... ");
    progressText->SetStyleAuto();

    /// addlones
    continueButton->SetName("Login");
    continueButton->SetStyle("continueButton");

    /// Add the controls to the title bar
    titleBar->AddChild(windowTitle);
    window_->AddChild(titleBar);
    window_->AddChild(progressText);
    window_->AddChild(continueButton);

    /// Apply styles
    window_->SetStyleAuto();
    windowTitle->SetStyleAuto();

    SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClient, ProgressScreenUIHandleClosePressed));

    return;
}

// Progress screen UI function handler
void ExistenceClient::ProgressScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ExistenceGameState.SetUIState(UI_PROGRESSINTERFACE);

    mainScreenUI();

    return;
}

/// Create a account UI
void ExistenceClient::CreateAccountScreenUI()
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    ui->Clear();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// set ui state to none
    ExistenceGameState.SetUIState(UI_ACCOUNTCREATIONINTERFACE);

    /// Create the Window and add it to the UI's root node
    window_= new Window(context_);
    uiRoot_->AddChild(window_);

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
    window_->SetMinSize(384, 192);
    window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window_->SetAlignment(HA_CENTER, VA_CENTER);
    window_->SetName("CreateAccountWindow");
    window_->SetMovable(false);
    window_->SetOpacity(.6);

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
    window_->AddChild(titleBar);
    window_->AddChild(usernameInput);
    window_->AddChild(emailText);
    window_->AddChild(emailInput);
    window_->AddChild(confirmpassword1UIElement);
    window_->AddChild(passwordInput1);
    window_->AddChild(confirmpassword2UIElement);
    window_->AddChild(passwordInput2);

    blankUIElement-> AddChild(blankText);
    window_->AddChild(blankUIElement);

    blank2UIElement-> AddChild(blank2Text);

    playerUIElement-> AddChild(nameText);

    playernameinputUIElement->AddChild(firstnameInput);
    playernameinputUIElement->AddChild(middlenameInput);
    playernameinputUIElement->AddChild(lastnameInput);

    /// declare buttons
    newaccount2Button->SetName("NewAccountLogin");
    newaccount2Button->SetStyle("continueButton");

    /// add children
    window_->AddChild(playerUIElement);
    window_->AddChild(playernameinputUIElement);
    window_->AddChild(blank2UIElement);

    window_->AddChild(newaccount2Button);

    /// Apply styles
    window_->SetStyleAuto();
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
    SubscribeToEvent(newaccount2Button, E_RELEASED, HANDLER(ExistenceClient, CreateAccountUIHandleClosePressed));

    return;
}

void ExistenceClient::CreateAccountUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui_ = GetSubsystem<UI>();

    /// Set UI state to account creation interface
    ExistenceGameState.SetUIState(UI_ACCOUNTCREATIONINTERFACE);

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
    SaveAccount(account);

    eraseScene();

    CreatePlayerScreenUI();

    return;
}


/// Create a player screen UI
void ExistenceClient::CreatePlayerScreenUI()
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
    ExistenceGameState.SetUIState(UI_CHARACTERCREATIONINTERFACE);
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
    SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClient, CreatePlayerUIHandleClosePressed));
    SubscribeToEvent(faction0button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(faction1button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(faction2button,  E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(faction3button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(alien0button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(alien1button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(alien2button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(alien3button, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(gendermalebutton, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(genderfemalebutton, E_RELEASED, HANDLER(ExistenceClient, HandleMouseReleased));
    SubscribeToEvent(PersonalitySelection, E_ITEMCLICKED,HANDLER(ExistenceClient,HandlePersonalitySelectionItemClick));
    SubscribeToEvent(camerachangeorientationButton, E_RELEASED, HANDLER(ExistenceClient, HandlerCameraOrientation));

    /// Subscribe also to all UI mouse clicks just to see where we have clicked
    SubscribeToEvent(E_UIMOUSECLICK, HANDLER(ExistenceClient, CreatePlayerUIHandleControlClicked));

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
void ExistenceClient::HandlerCameraOrientation(StringHash eventType, VariantMap& eventData)
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
void ExistenceClient::CameraOrientationRotateMove (float degrees, int movement)
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
void ExistenceClient::HandleCharacterSelectedReleased(StringHash eventType, VariantMap& eventData)
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
        Node* playermeshNode = scene_->GetChild("playerMesh");

        playermeshNode->RemoveAllChildren();

        /// Set selected player
        TemporaryAccountPlayerSelected=clickedbutton;

        /// Load player mesh in main screen ui
        loadplayerMesh(playermeshNode, TemporaryAccountPlayerList[clickedbutton].GetAlliance().alienrace, TemporaryAccountPlayerList[clickedbutton].GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);
    }
}

/// Handle character selection
void ExistenceClient::HandleMouseReleased(StringHash eventType, VariantMap& eventData)
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
void ExistenceClient::CreatePlayerUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{

    /// Set ui state to character creation
    ExistenceGameState.SetUIState(UI_CHARACTERCREATIONINTERFACE);

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

    ProgressScreenUI();

}


/// Character zoom mode pressed
void ExistenceClient::CreatePlayerUIHandleControlClicked(StringHash eventType, VariantMap& eventData)
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


/// Handle updates
void ExistenceClient::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    using namespace Update;

    /// Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    Input* input = GetSubsystem<Input>();

    /// Move the camera, scale movement with time step
    MoveCamera(timeStep);


    /// Handle UI updates
    if(ExistenceGameState.GetGameState()==STATE_GAME)
    {
        UpdateUI(timeStep);
    }

    /// controls movement
    if(ExistenceGameState.GetCameraMode()==CAMERAMODE_FIRSTPERSON&&ExistenceGameState.GetGameState()==STATE_GAME)
    {
        /// Clear previous controls
        character_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT | CTRL_JUMP | CTRL_FIRE, false);

        /// check if UI element is active
        if(!GetSubsystem<UI>()->GetFocusElement())
        {

            /// Clear previous controls

            /// Update controls using keys
            UI* ui = GetSubsystem<UI>();

            character_->controls_.Set(CTRL_FORWARD, input->GetKeyDown('W'));
            character_->controls_.Set(CTRL_BACK, input->GetKeyDown('S'));
            character_->controls_.Set(CTRL_LEFT, input->GetKeyDown('A'));
            character_->controls_.Set(CTRL_RIGHT, input->GetKeyDown('D'));
            character_->controls_.Set(CTRL_FIRE, input->GetKeyDown('Q'));

            character_->controls_.Set(CTRL_JUMP, input->GetKeyDown(KEY_SPACE));

            /// Get character information
            Node * characterNode = scene_->GetChild("Character",true);

            RigidBody* body = characterNode->GetComponent<RigidBody>();

            Node * firstpersonCameraNode = characterNode -> GetChild("CameraFirstPerson",true);

            float YAW_SENSITIVITY=.5;

            /// Updaw character yaw and povement
            character_->controls_.yaw_ +=(float)input->GetMouseMoveX() * YAW_SENSITIVITY;
            character_->controls_.pitch_ +=(float)input->GetMouseMoveY() * YAW_SENSITIVITY;

            character_->controls_.yaw_ = Clamp(character_->controls_.yaw_, -80.0f, 80.0f);
            character_->controls_.pitch_ = Clamp(character_->controls_.pitch_, -80.0f, 80.0f);

            /// Copy yaw and pitch
            float yaw_=character_->controls_.yaw_;
            float pitch_=character_->controls_.pitch_;

            firstpersonCameraNode->SetRotation(Quaternion(pitch_,yaw_, 0.0f));

        }
    }

    return;
}


/// Handler for keydown
void ExistenceClient::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{

    UI* ui = GetSubsystem<UI>();

    ExistenceGameState.SetConsoleState(GetSubsystem<Console>()->IsVisible());

    /// Unlike the other samples, exiting the engine when ESC is pressed instead of just closing the console
    if (eventData[KeyDown::P_KEY].GetInt() == KEY_F12)
    {
        if((ExistenceGameState.GetUIState()==UI_CHARACTERSELECTIONINTERFACE)||(ExistenceGameState.GetUIState()==UI_GAMECONSOLE))
        {
            if(ExistenceGameState.GetConsoleState())
            {
                Console* console = GetSubsystem<Console>();

                console -> SetVisible(false);

                UI* ui = GetSubsystem<UI>();

                ExistenceGameState.SetConsoleState(false);

            }
            else
            {
                Console* console = GetSubsystem<Console>();

                console -> SetVisible(true);

                ExistenceGameState.SetConsoleState(true);

            }
        }

        return;
    }

    if (eventData[KeyDown::P_KEY].GetInt() == KEY_F10&&ExistenceGameState.GetGameState()==STATE_GAME)
    {
        /// load window
        UIElement * uiroot = ui ->	GetRoot ();

        UIElement * PlayerWindowUIElement = uiroot->GetChild("PlayerWindow",true);

        /// Enable
        if(PlayerWindowUIElement)
        {
            PlayerWindowUIElement -> SetVisible(true);
            PlayerWindowUIElement -> SetDeepEnabled(true);

        }
        else
        {
            loadUIXML(UIPLAYERWINDOW,200,200);
        }

    }


    /// Check if game is in first person camera mode and in game state
    if(ExistenceGameState.GetCameraMode()==CAMERAMODE_FIRSTPERSON&&ExistenceGameState.GetGameState()==STATE_GAME)
    {
        /// check if UI element is active
        if(GetSubsystem<UI>()->GetFocusElement())
        {
            return;
        }

        /// Locate camera in scene
        Node * characterNode = scene_->GetChild("Character",true);
        RigidBody* body = characterNode->GetComponent<RigidBody>();

        float force=.2;
    }

    return;
}

/// Load a player mesh into a node
int ExistenceClient::loadplayerMesh(Node * playermeshNode, int alienrace, int gender, int mode)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Temporarily define faction information (Might make this a class)
    unsigned int factionslimit=4;
    unsigned int alienslimit=4;

    /// select active
    alienraces Alien[alienslimit];
    string alienracename;

    /// Define structure
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

    alienracename.clear();



    /// Select currentalien
    for(unsigned int idx=0; idx<alienslimit; idx++)
    {

        if( alienrace==Alien[idx].uniqueid)
        {
            alienracename.append(Alien[idx].name);
            break;
        }
    }

    Node * playermeshNode1 = playermeshNode -> CreateChild("playermeshNode1");
    Node * playermeshNode2 = playermeshNode -> CreateChild("playermeshNode2");

    /// Created Animated MOdel
    AnimatedModel* playermeshObject1 = playermeshNode1 ->CreateComponent<AnimatedModel>();
    AnimatedModel* playermeshObject2 = playermeshNode2 ->CreateComponent<AnimatedModel>();

    /// Add Animation Controller
    playermeshNode1->CreateComponent<AnimationController>();
    playermeshNode2->CreateComponent<AnimationController>();

    /// Setup mesh and mesh details nodes and stactic models
    if((gender<51&&mode==DISPLAYMESH_SINGLECHARACTER)||mode==DISPLAYMESH_MUILTIPLECHARACTER)
    {
        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("female");

        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";

        playermeshObject1->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        playermeshObject1->ApplyMaterialList(String(temporarytexturefilename.c_str()));
        playermeshObject1->SetCastShadows(true);

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel * playermeshObject1outfitbottom = playermeshNode1->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".txt";

        playermeshObject1outfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject1outfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject1outfittop =  playermeshNode1->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".txt";

        playermeshObject1outfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject1outfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject1teeth =  playermeshNode1->CreateComponent<AnimatedModel>();
        playermeshObject1teeth->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleteeth.mdl"));
        playermeshObject1teeth->ApplyMaterialList("Resources/Models/standardfemaleteeth.txt");

        AnimatedModel* playermeshObject1eyes =  playermeshNode1->CreateComponent<AnimatedModel>();
        playermeshObject1eyes->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleeyes.mdl"));
        playermeshObject1eyes->ApplyMaterialList("Resources/Models/standardfemaleeyes.txt");

        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianfemaleIdleAction.ani");

        playermeshObject1 -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleFemaleAnimation");

        /// Get Controller
        AnimationController * playermeshAnimationController1 = playermeshNode1 -> GetComponent<AnimationController>();

        /// Set Morph Weight
        playermeshAnimationController1-> SetWeight("IdleFemaleAnimation",1);
        playermeshAnimationController1-> SetTime("IdleFemaleAnimation",1.80991);

        playermeshAnimationController1->Play("IdleFemaleAnimation",0,1,0);
    }

    /// Setup mesh and mesh details nodes and stactic models
    if((gender>50&&mode==DISPLAYMESH_SINGLECHARACTER)||mode==DISPLAYMESH_MUILTIPLECHARACTER)
    {
        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("male");


        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";

        playermeshObject2->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        playermeshObject2->ApplyMaterialList(String(temporarytexturefilename.c_str()));

        playermeshObject2->SetCastShadows(true);

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel* playermeshObject2outfitbottom = playermeshNode2->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".txt";

        playermeshObject2outfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject2outfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject2outfittop =  playermeshNode2->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".txt";

        playermeshObject2outfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject2outfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject2teeth =  playermeshNode2->CreateComponent<AnimatedModel>();
        playermeshObject2teeth->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleteeth.mdl"));
        playermeshObject2teeth->ApplyMaterialList("Resources/Models/standardmaleteeth.txt");

        AnimatedModel* playermeshObject2eyes =  playermeshNode2->CreateComponent<AnimatedModel>();
        playermeshObject2eyes->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleeyes.mdl"));
        playermeshObject2eyes->ApplyMaterialList("Resources/Models/standardmaleeyes.txt");


        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianmaleIdleGuardAction.ani");

        playermeshObject2 -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleMaleAnimation");

        /// Get Controller
        AnimationController * playermeshAnimationController2 = playermeshNode2 -> GetComponent<AnimationController>();

        /// Set Morph Weight
        playermeshAnimationController2-> SetWeight("IdleMaleAnimation",1);
        playermeshAnimationController2-> SetTime("IdleMaleAnimation",1.80991);

        playermeshAnimationController2->Play("IdleMaleAnimation",0,1,0);

    }

    /// Position static models if muiltiple charaters shown
    if(mode==DISPLAYMESH_MUILTIPLECHARACTER)
    {
        playermeshNode1 -> SetPosition(Vector3(0.5f,0.0f,0.0f));
        playermeshNode1 ->SetRotation(Quaternion(0.0,10.0,0.0));
        playermeshNode2 -> SetPosition(Vector3(-0.5f,0.0f,0.0f));
        playermeshNode2 ->SetRotation(Quaternion(0.0,-10,0.0));
    }

    return 1;
}

/// Main screen user interface function
int ExistenceClient::mainScreenUI(void)
{
    /// set ui state to none
    ExistenceGameState.SetUIState(UI_CHARACTERSELECTIONINTERFACE);

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    ui->Clear();

    /// Create player mesh node and scale
    Node* playermeshNode = scene_->CreateChild("playerMesh");

    LoadAccountPlayers();

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
    uiRoot_->AddChild(newcharacterUIElement);

    uiRoot_->AddChild(menuUIElements);

    unsigned int placement=8; /// Set pace between buttons variable

    /// Loop through all players recovered
    for(unsigned int i=0; i<TemporaryAccountPlayerListLimit; ++i)
    {

        UIElement * playerUIElement = new UIElement (context_);

        UIElement * playernameUIElement = new UIElement (context_);
        Button * playernameButton = new Button(context_);
        Text *  playernameText = new Text(context_);

        UIElement * playerupdatesUIElement = new UIElement (context_);
        Button * playerupdatesButton = new Button(context_);

        /// create new character element
        playerUIElement -> SetPosition(1,(128+66)+placement);
        playerUIElement  -> SetMinSize(540,66);
        playerUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470,66));
        playerUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playerUIElement  -> SetStyleAuto();

        playernameUIElement -> SetPosition(1,1);
        playernameUIElement  -> SetMinSize(470,66);
        playernameUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470,66));
        playernameUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playernameUIElement  -> SetStyleAuto();

        playernameButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 470, 66));
        playernameButton -> SetAlignment(HA_LEFT, VA_TOP);
        playernameButton -> SetFixedSize(470, 66);
        playernameButton -> SetPosition(0, 0);

        playernameText -> SetPosition(0, 0);
        playernameText -> SetAlignment(HA_LEFT, VA_CENTER);
        playernameText -> SetTextAlignment(HA_LEFT);

        /// Create a username to display on player button
        string playername= "  "+TemporaryAccountPlayerList[i].GetPlayerInfo().firstname+" "+TemporaryAccountPlayerList[i].GetPlayerInfo().lastname;

        playernameText -> SetText(String(playername.c_str()));

        playernameText -> SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 13);

        playerupdatesUIElement -> SetPosition(476, 1);
        playerupdatesUIElement  -> SetMinSize(66,66);
        playerupdatesUIElement  -> SetLayout(LM_FREE, 6, IntRect(0, 0, 66,66));
        playerupdatesUIElement -> SetAlignment(HA_LEFT, VA_TOP);
        playerupdatesUIElement  -> SetStyleAuto();
        playerupdatesUIElement -> SetOpacity(.8);

        playerupdatesButton -> SetLayout(LM_FREE, 6, IntRect(0, 0, 66, 66));
        playerupdatesButton -> SetAlignment(HA_LEFT, VA_TOP);
        playerupdatesButton -> SetFixedSize(66, 66);
        playerupdatesButton -> SetPosition(0, 0);

        playernameUIElement -> SetOpacity(.8);

        playernameUIElement -> AddChild(playernameButton);
        playernameUIElement -> AddChild(playernameText);

        playerupdatesUIElement -> AddChild(playerupdatesButton);

        playerUIElement ->AddChild(playernameUIElement);
        playerUIElement ->AddChild(playerupdatesUIElement);

        uiRoot_->AddChild(playerUIElement);

        playernameButton -> SetStyle("playermainscreenButton");

        string playerbuttontext="playermainscreenButton ";

        switch(i)
        {
        case 0:
            playerbuttontext.append("0");
            break;
        case 1:
            playerbuttontext.append("1");
            break;
        case 2:
            playerbuttontext.append("2");
            break;
        case 3:
            playerbuttontext.append("3");
        }

        playernameButton -> SetName(String(playerbuttontext.c_str()));
        playerupdatesButton -> SetStyle("playermainscreenupdateButton");

        SubscribeToEvent(playernameButton, E_RELEASED, HANDLER(ExistenceClient, HandleCharacterSelectedReleased));

        placement=placement+(66+8);

    }

    /// Set exitbutton and new character style
    exitButton -> SetStyle("logoutButton");
    newcharacterButton -> SetStyle("newcharacterButton");

    Vector3 playermeshPosition=Vector3(1.0f,-2.0f,0.0f);

    playermeshNode ->SetPosition(playermeshPosition);
    playermeshNode ->SetRotation(Quaternion(0.0, 0.0,0.0));

    playermeshNode->SetScale(2);

    Node * cameraNode=scene_->GetChild("Camera");
    cameraNode->SetPosition(Vector3(2.0f,-0.5f,4.0f));

    /// Add three point lighting
    Node* FillLightNode = scene_->CreateChild("DirectionalLight");
    FillLightNode->SetDirection(playermeshPosition+Vector3(1.0f,0.0f,1.0f)); /// The direction vector does not need to be normalized
    Light* FillLight = FillLightNode->CreateComponent<Light>();
    FillLight->SetLightType(LIGHT_DIRECTIONAL);
    FillLight ->SetBrightness(1);
    FillLight ->SetSpecularIntensity(0);
    FillLightNode->LookAt(playermeshPosition);

    /// Add three point lighting
    Node* KeyLightNode = scene_->CreateChild("DirectionalLight");
    KeyLightNode->SetDirection(playermeshPosition+Vector3(1.0f,0.0f,-1.0f)); /// The direction vector does not need to be normalized
    Light* KeyLight = KeyLightNode->CreateComponent<Light>();
    KeyLight->SetLightType(LIGHT_DIRECTIONAL);
    KeyLight ->SetBrightness(.3);
    KeyLight ->SetSpecularIntensity(1);
    KeyLightNode->LookAt(playermeshPosition);

    /// Add three point lighting
    Node* BackLightNode = scene_->CreateChild("DirectionalLight");
    BackLightNode->SetDirection(playermeshPosition+Vector3(-1.0f,0.0f,-1.0f)); /// The direction vector does not need to be normalized
    Light* BackLight = BackLightNode->CreateComponent<Light>();
    BackLight->SetLightType(LIGHT_DIRECTIONAL);
    BackLight ->SetBrightness(.3);
    BackLight ->SetSpecularIntensity(0);
    BackLightNode->LookAt(playermeshPosition);

    /// Load the player mesh based on display random if no character and charcter
    if(TemporaryAccountPlayerListLimit==0)
    {
        loadplayerMesh(playermeshNode, TemporaryPlayer.GetAlliance().alienrace, TemporaryPlayer.GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);
    }
    else
    {
        loadplayerMesh(playermeshNode,TemporaryAccountPlayerList[0].GetAlliance().alienrace, TemporaryAccountPlayerList[0].GetCharacteristics().gender,DISPLAYMESH_SINGLECHARACTER);
    }

    /// Subscribe to events
    SubscribeToEvent(newcharacterButton, E_RELEASED, HANDLER(ExistenceClient, MainScreenUIHandleClosePressed));
    SubscribeToEvent(exitButton, E_RELEASED, HANDLER(ExistenceClient, MainScreenUIHandleClosePressed));



    return 1;

}


/// Main screen user interface handle close pressed
void ExistenceClient::MainScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// Set ui state to UI_CHARACTERSELECTIONINTERFACE
    ExistenceGameState.SetUIState(UI_CHARACTERSELECTIONINTERFACE);

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
            scene_->GetChild("playerMesh",true)->Remove();

            /// Clear screen
            eraseScene();

            /// Create a player UI
            CreatePlayerScreenUI();

            Console* console = GetSubsystem<Console>();

            console -> SetVisible(false);

            ExistenceGameState.SetConsoleState(UI_CONSOLEOFF);

            /// Enable OS cursor
            GetSubsystem<Input>()->SetMouseVisible(true);
        }
        else if(clickednamestring=="exitButton")
        {
            engine_->Exit();
        }
    }


    return;
}

/// Load account information from a account file
void ExistenceClient::LoadAccount(void)
{
    String accountconfigfilename=ACCOUNTFILE;
    XMLElement accountElement;

    accountexist = NULL;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* accountconfig = cache->GetResource<XMLFile>(accountconfigfilename);

    /// if account file is found
    if(accountconfig!=NULL)
    {
        /// get account root
        accountElement = accountconfig -> GetRoot("account");

        /// return because of error no account
        if(accountElement==0)
        {
            return;
        }

        /// read info
        XMLElement idElement = accountElement . GetChild("uniqueid");
        XMLElement emailElement = accountElement . GetChild("email");
        XMLElement firstnameElement = accountElement . GetChild("firstname");
        XMLElement middlenameElement = accountElement . GetChild("middlename");
        XMLElement lastnameElement = accountElement . GetChild("lastname");
        XMLElement passwordElement = accountElement . GetChild("password");
    }

    return;
}


/// Load account information from a account file
int ExistenceClient::LoadAccountPlayers(void)
{

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    String accountplayersfile;

    accountplayersfile.Append(filesystem->GetProgramDir().CString());
    accountplayersfile.Append("CoreData/");
    accountplayersfile.Append(PLAYERFILE);

    /// Set XML related elements
    XMLFile* charactersplayer = cache->GetResource<XMLFile>(accountplayersfile);
    XMLElement charactersRootElement;
    XMLElement NextSibling;

    /// if account file is found
    if(charactersplayer!=NULL)
    {

        /// get account root
        charactersRootElement = charactersplayer -> GetRoot("characters");

        /// Set Counter
        unsigned int counter=0;

        NextSibling=charactersRootElement .GetChild();

        /// Resize memory to character list
        TemporaryAccountPlayerList = new Player[4];

        /// Loop through xml data
        do
        {

            /// Temporay player Info
            playerbasicinfo tempplayerinfo;
            playercharacteristics tempplayercharacteristics;
            playeralliance tempplayeralliance;

            /// Get elements
            XMLElement firstnameElement  = NextSibling.GetChild("firstname");
            XMLElement middlenameElement  = NextSibling.GetChild("middlename");
            XMLElement lastnameElement  = NextSibling.GetChild("lastname");
            XMLElement reputation1Element  = NextSibling.GetChild("reputation1");
            XMLElement reputation2Element  = NextSibling.GetChild("reputation2");
            XMLElement reputation3Element  = NextSibling.GetChild("reputation3");
            XMLElement reputation4Element  = NextSibling.GetChild("reputation4");
            XMLElement experienceElement  = NextSibling.GetChild("experience");
            XMLElement alienraceElement  = NextSibling.GetChild("alienrace");
            XMLElement alienalliancealignedElement  = NextSibling.GetChild("alienalliancealigned");
            XMLElement genderElement  = NextSibling.GetChild("gender");
            XMLElement personalitytraitElement  = NextSibling.GetChild("personalitytrait");

            /// Assisgn values
            tempplayerinfo.firstname.append(firstnameElement.GetAttributeCString("firstname"));
            tempplayerinfo.middlename.append(middlenameElement.GetAttributeCString("middlename"));
            tempplayerinfo.lastname.append(lastnameElement.GetAttributeCString("lastname"));

            tempplayercharacteristics.gender=genderElement.GetUInt("gender");
            tempplayercharacteristics.personalitytrait=genderElement.GetUInt("personalitytrait");

            tempplayeralliance.alienrace=alienraceElement.GetUInt("alienrace");
            tempplayeralliance.alienalliancealigned=alienalliancealignedElement.GetUInt("alienalliancealigned");

            /// Save information to table
            TemporaryAccountPlayerList[counter].SetAlliance(tempplayeralliance);
            TemporaryAccountPlayerList[counter].SetPlayerInfo(tempplayerinfo);
            TemporaryAccountPlayerList[counter].SetCharacteristics(tempplayercharacteristics);

            /// Get next info
            counter++;

            /// Break do loop
            if(counter==3)
            {
                break;
            }
        }
        while((NextSibling=NextSibling.GetNext())!=NULL);

        /// Save limit
        TemporaryAccountPlayerListLimit=counter++;

        return 1;
    }

    return 0;
}


/// Save account information to a file
void ExistenceClient::SaveAccount(accountinformation account)
{
    String accountconfigfilename;

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    accountconfigfilename.Append(filesystem->GetProgramDir().CString());
    accountconfigfilename.Append("CoreData/");
    accountconfigfilename.Append(ACCOUNTFILE);

    File saveFile(context_, accountconfigfilename.CString(), FILE_WRITE);

    /// Check if the account file information exist
    if(!filesystem->FileExists(accountconfigfilename.CString()))
    {
        //cout << "\r\nAccount file ("<< accountconfigfilename.CString() << ") does not exist.";
    }

    XMLFile * accountconfig  = new XMLFile(context_);

    XMLElement configElem = accountconfig -> CreateRoot("account");
    XMLElement idElement = configElem.CreateChild("uniqueid");
    XMLElement emailElement= configElem.CreateChild("email");
    XMLElement usernameElement  = configElem.CreateChild("username");
    XMLElement firstnameElement  = configElem.CreateChild("firstname");
    XMLElement middlenameElement  = configElem.CreateChild("middlename");
    XMLElement lastnameElement  = configElem.CreateChild("lastname");
    XMLElement passwordElement = configElem.CreateChild("password");

    String AccountEmail(account.email.c_str());
    String AccountUsername(account.username.c_str());
    String AccountFirstname(account.firstname.c_str());
    String AccountMiddlename(account.middlename.c_str());
    String AccountLastname(account.lastname.c_str());
    String AccountPassword(account.password.c_str());

    idElement.SetString("uniqueid", "test");
    emailElement.SetString("email", AccountEmail);
    usernameElement.SetString("username", AccountUsername);
    firstnameElement.SetString("firstname", AccountFirstname);
    middlenameElement.SetString("middlename", AccountMiddlename);
    lastnameElement.SetString("lastname", AccountLastname);
    passwordElement.SetString("password", AccountPassword);

    accountconfig->Save(saveFile);

    return;
}

/// Save player information to a file
void ExistenceClient::SavePlayer(int writemode)
{

    /// Get resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    int fileexist=0;

    String playerconfigfilename;

    //Debug
    //out << filesystem->GetProgramDir().CString()+"CoreData/"+playerconfigfilename.CString();
    playerconfigfilename.Append(filesystem->GetProgramDir().CString());
    playerconfigfilename.Append("CoreData/");
    playerconfigfilename.Append(PLAYERFILE);

    /// check if player file exist
    if(!filesystem->FileExists(playerconfigfilename.CString()))
    {
        //Debug
        //cout << "\r\nPlayer file ("<< playerconfigfilename.CString() << ") does not exist.";
        fileexist=false;
    }


    /// check if player file exist
    if(filesystem->FileExists(playerconfigfilename.CString()))
    {
        fileexist=true;
    }

    /// get the playerinformation
    playerbasicinfo TempBasicInfo = TemporaryPlayer.GetPlayerInfo();
    playercharacteristics TempCharacteristics = TemporaryPlayer.GetCharacteristics();
    playeralliance TempAlliance = TemporaryPlayer.GetAlliance();

    /// if writemode equal 1 then new player
    {

    }

    /// if file did not exist
    if (!fileexist)
    {
        File saveFile(context_, playerconfigfilename.CString(), FILE_READWRITE);

        XMLFile * playerconfig  = new XMLFile(context_);

        /// create xml child
        XMLElement configElement = playerconfig -> CreateRoot("characters");
        XMLElement characterElement =  configElement.CreateChild("character");
        XMLElement firstnameElement  = characterElement.CreateChild("firstname");
        XMLElement middlenameElement  = characterElement.CreateChild("middlename");
        XMLElement lastnameElement  = characterElement.CreateChild("lastname");
        XMLElement reputation1Element  = characterElement.CreateChild("reputation1");
        XMLElement reputation2Element  = characterElement.CreateChild("reputation2");
        XMLElement reputation3Element  = characterElement.CreateChild("reputation3");
        XMLElement reputation4Element  = characterElement.CreateChild("reputation4");
        XMLElement experienceElement  = characterElement.CreateChild("experience");
        XMLElement alienraceElement  = characterElement.CreateChild("alienrace");
        XMLElement alienalliancealignedElement  = characterElement.CreateChild("alienalliancealigned");
        XMLElement genderElement  = characterElement.CreateChild("gender");
        XMLElement personalitytraitElement  = characterElement.CreateChild("personalitytrait");

        /// copy player information into xml elements
        characterElement.SetString("character", "character");
        firstnameElement.SetString("firstname", String(TempBasicInfo.firstname.c_str()));
        middlenameElement.SetString("middlename", String(TempBasicInfo.middlename.c_str()));
        lastnameElement.SetString("lastname", String(TempBasicInfo.lastname.c_str()));
        reputation1Element.SetUInt("reputation1",0);
        reputation2Element.SetUInt("reputation2",0);
        reputation3Element.SetUInt("reputation3",0);
        reputation4Element.SetUInt("reputation4",0);
        experienceElement.SetUInt("experience",0);
        alienraceElement.SetUInt("alienrace",TempAlliance.alienrace);
        alienalliancealignedElement.SetBool("alienalliancealigned",0);
        genderElement.SetUInt("gender",TempCharacteristics.gender);
        personalitytraitElement.SetUInt("personalitytrait",TempCharacteristics.personalitytrait);


        playerconfig->Save(saveFile);
    }
    else
    {

        File saveFile(context_, playerconfigfilename.CString(), FILE_READ);

        XMLFile * playerconfig  = new XMLFile(context_);

        playerconfig->Load(saveFile);

        /// create xml child
        XMLElement configElement = playerconfig -> GetRoot("characters");
        XMLElement characterElement = configElement .CreateChild("character");
        XMLElement firstnameElement  = characterElement.CreateChild("firstname");
        XMLElement middlenameElement  = characterElement.CreateChild("middlename");
        XMLElement lastnameElement  = characterElement.CreateChild("lastname");
        XMLElement reputation1Element  = characterElement.CreateChild("reputation1");
        XMLElement reputation2Element  = characterElement.CreateChild("reputation2");
        XMLElement reputation3Element  = characterElement.CreateChild("reputation3");
        XMLElement reputation4Element  = characterElement.CreateChild("reputation4");
        XMLElement experienceElement  = characterElement.CreateChild("experience");
        XMLElement alienraceElement  = characterElement.CreateChild("alienrace");
        XMLElement alienalliancealignedElement  = characterElement.CreateChild("alienalliancealigned");
        XMLElement genderElement  = characterElement.CreateChild("gender");
        XMLElement personalitytraitElement  = characterElement.CreateChild("personalitytrait");

        /// copy player information into xml elements
        characterElement.SetString("character", "character");
        firstnameElement.SetString("firstname", String(TempBasicInfo.firstname.c_str()));
        middlenameElement.SetString("middlename", String(TempBasicInfo.middlename.c_str()));
        lastnameElement.SetString("lastname", String(TempBasicInfo.lastname.c_str()));
        reputation1Element.SetUInt("reputation1",0);
        reputation2Element.SetUInt("reputation2",0);
        reputation3Element.SetUInt("reputation3",0);
        reputation4Element.SetUInt("reputation4",0);
        experienceElement.SetUInt("experience",0);
        alienraceElement.SetUInt("alienrace",TempAlliance.alienrace);
        alienalliancealignedElement.SetBool("alienalliancealigned",0);
        genderElement.SetUInt("gender",TempCharacteristics.gender);
        personalitytraitElement.SetUInt("personalitytrait",TempCharacteristics.personalitytrait);

        File updateFile(context_, playerconfigfilename.CString(), FILE_WRITE);

        playerconfig->Save(updateFile);

    }

    return;
}

/// Initialize the main console
void ExistenceClient::InitializeConsole(void)
{
    /// create basic console
    Console* console = GetSubsystem<Console>();
    console->SetNumRows(2);
    console->SetNumBufferedRows(32);
    console->SetCommandInterpreter(GetTypeName());
    console->SetVisible(false);
    console->GetCloseButton()->SetVisible(false);

    ExistenceGameState.SetConsoleState(UI_CONSOLEOFF);
}

/// code to handle console command inputs
void ExistenceClient::HandleConsoleCommand(StringHash eventType, VariantMap& eventData)
{
    using namespace ConsoleCommand;

    if (eventData[P_ID].GetString() == GetTypeName())
        HandleInput(eventData[P_COMMAND].GetString());
}

/// Code to handle actual commans
void ExistenceClient::HandleInput(const String& input)
{
    /// Get needed resources
    Renderer* renderer = GetSubsystem<Renderer>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();

    /// Get input and lower case it
    /// then convert to normal string
    String inputLower = input.ToLower().Trimmed();
    string inputtext = inputLower.CString();

    const vector<string> parseinput = split(inputtext, " ");

    int parseinputcommands=parseinput.size();
    int mode;

    string * parseinputarray;
    parseinputarray= new string[parseinput.size()];

    for(int i=0; i< parseinput.size(); i++)
    {
        parseinputarray[i] =  parseinput [i];//Copy the vector to the string
    }

    if (parseinput[0] == "/scene")
    {

        if(ExistenceGameState.GetGameState()==STATE_GAME)
        {
            return;

        }
        eraseScene();


        ExistenceGameState.SetUIState(UI_GAMECONSOLE);
        ExistenceGameState.SetGameState(STATE_GAME);

        loadScene(1, input.CString());
    }
    /// if it the game is in game mode go here
    else if (ExistenceGameState.GetGameState()==STATE_GAME)
    {

        /// Check termination - Check Action
        if(parseinput[0] == "/end")
        {

            if(ExistenceGameState.GetDebugHudMode()==true)
            {
                ExistenceGameState.SetDebugHudMode(false);
                GetSubsystem<DebugHud>()->ToggleAll();
            }

            eraseScene();

            GetSubsystem<Input>()->SetMouseVisible(true);

            /// set ui state to none
            ExistenceGameState.SetUIState(UI_CHARACTERSELECTIONINTERFACE);
            ExistenceGameState.SetGameState(STATE_MAIN);

            /// setup scene
            SetupScreenViewport();

            ExistenceGameState.SetCameraMode(CAMERAMODE_DEFAULT);

            mainScreenUI();

            /// Create a scene node for the camera, which we will move around
            /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
            cameraNode_ = scene_->GetChild("Camera");

            /// Set an initial position for the camera scene node above the plane
            cameraNode_->SetPosition(Vector3(2.0,0.0,5.0));

        }
        /// go to environment function
        if(parseinput[0] == "/environment")
        {
            /// go to environment function
            ConsoleActionEnvironment(input.CString());
        }

        /// go to camera functions
        if(parseinput[0] == "/camera")
        {
            /// go to environment function
            ConsoleActionCamera(input.CString());
        }

        /// go to camera functions
        if(parseinput[0] == "/build")
        {
            /// go to environment function
            ConsoleActionBuild(input.CString());
        }



        /// go to debug functions
        if(parseinput[0] == "/debug")
        {
            /// go to environment function
            ConsoleActionDebug(input.CString());

        }

        /// go to debug functions
        if(parseinput[0] == "/character")
        {
            /// go to environment function
            ConsoleActionCharacter(input.CString());

        }

        /// go to debug functions
        if(parseinput[0] == "/renderer")
        {
            /// go to environment function
            ConsoleActionRenderer(input.CString());
        }
    }
}

/// Erase a scene from the UI and Scene
void ExistenceClient::eraseScene(void)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    ui_->Clear();
    scene_->Clear();

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_ -> CreateComponent<Camera>();

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0,0.0,0.0));

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;

}

/// Load a dummy scene
void ExistenceClient::loadDummyScene(void)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    scene_-> CreateComponent<Octree>();
    scene_-> CreateComponent<DebugRenderer>();

    /// Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
    /// show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
    /// is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
    /// optimizing manner

    /// Create a child scene node (at world origin) and a StaticModel component into it. Set the StaticModel to show a simple
    /// plane mesh with a "stone" material. Note that naming the scene nodes is optional. Scale the scene node larger
    /// (100 x 100 world units)
    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    /// Create rigidbody, and set non-zero mass so that the body becomes dynamic
    RigidBody* body = planeNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);

    /// Set a capsule shape for collision
    CollisionShape* shape = planeNode->CreateComponent<CollisionShape>();

    /// Set shape collision
    shape->SetBox(Vector3::ONE);
    shape->SetLodLevel(1);

    /// Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    /// light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    /// The light will use default settings (white light, no shadows)
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); /// The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    /// Create more StaticModel objects to the scene, randomly positioned, rotated and scaled. For rotation, we construct a
    /// quaternion from Euler angles where the Y angle (rotation about the Y axis) is randomized. The mushroom model contains
    /// LOD levels, so the StaticModel component will automatically select the LOD level according to the view distance (you'll
    /// see the model get simpler as it moves further away). Finally, rendering a large number of the same object with the
    /// same material allows instancing to be used, if the GPU supports it. This reduces the amount of CPU work in rendering the
    /// scene.
    const unsigned NUM_OBJECTS = 200;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* mushroomNode = scene_->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(0.5f + Random(2.0f));
        StaticModel* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
        mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
    }

    Node* characterNode = scene_->CreateChild("Character");
    characterNode->SetName("Character");

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}


/// Load a scene
void ExistenceClient::loadSceneCreationCreation( const char * lineinput)
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
        loadDummyScene();
    }


    /// Get the Camera Node and setup the viewport
    Node * cameraNode_ = scene_->GetChild("Camera");

    /// Change viewport to camera node
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}




/// Load a scene
void ExistenceClient::loadScene(const int mode, const char * lineinput)
{
    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Set Scene
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_= GetSubsystem<EnvironmentBuild>();

    /// clear api
    manager_->SetScene(scene_);
    environmentbuild_->SetScene(scene_);

    Input* input = GetSubsystem<Input>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 40)
    {
        ssin >> argument[idx];
        ++idx;
    }

    int testgenerate=1;
    int padded=0;

    GetSubsystem<Input>()->SetMouseVisible(false);

    unsigned long long int creationtime = 0; /// Default

    /// Run trhrough arugments - first check if GENERATE
    if(argument[1]=="generate")
    {

        if(argument[2]=="random")
        {
            /// get current time
            creationtime = 0;
        }
        else if(argument[2]=="reset")
        {
            /// get current time
            time_t tempseed = time(NULL);

            creationtime = static_cast<unsigned long int> (tempseed);

            GetSubsystem<Input>()->SetMouseVisible(true);

            return;
        }
        else if(argument[2] =="datetime")
        {

            /// get current time
            time_t tempseed = time(NULL);

            /// Convert time and change timespeed if valid
            if(tempseed = ConvertStringToTime(argument[3].c_str(), tempseed))
            {
                creationtime = static_cast<unsigned long long int > (tempseed);
            }
            else
            {

                creationtime = static_cast<unsigned long long int > (tempseed);
            }
            padded=1;
        }
        else if(argument[2]=="seed")
        {
            std::string::size_type sz;   // alias of size_t

            creationtime=atoi(argument[3].c_str());
            padded=1;
        }
        else
        {

            /// get current time
            creationtime = 0;
        }


        /// Copy rule information
        terrain_rule terrainrule;

        /// Copy rule
        terrainrule.worldtype=(int)atoi(argument[3+padded].c_str());
        terrainrule.subworldtype=(int)atoi(argument[4+padded].c_str());
        terrainrule.sealevel=(float)atof(argument[5+padded].c_str());

        /// Set timeseed
        /// terrainrule.timeseed=timeseed;   /// temporary
        terrainrule.creationtime=creationtime; /// new for randomizing

        /// generate a seen
        GenerateScene(terrainrule, argument[6+padded].c_str());
    }
/// Run trhrough arugments - first check if FILE
    else if (argument[1]=="file")
    {
        /// Create variables (urho3d)
        String InputDataFile;

        InputDataFile.Append(filesystem->GetProgramDir().CString());
        InputDataFile.Append("Resources/Scenes/");
        InputDataFile.Append(argument[2].c_str());

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
            loadDummyScene();
        }

    }
    else
    {
        /// load dummy scene
        loadDummyScene();
    }



/// Get the Camera Node and setup the viewport
    if((cameraNode_ = scene_->GetChild("Camera")))
    {

        /*SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);*/
    }
    else
    {
        /// Create a scene node for the camera, which we will move around
        /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
        cameraNode_ = new Node(context_);

        cameraNode_ = scene_->CreateChild("Camera");
        cameraNode_->CreateComponent<Camera>();

        Camera* camera = cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(1000.0f);

        ///Set an initial position for the camera scene node above the ground
        cameraNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

    }

/// Setup viewport
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

/// Loop through the whole scene and get the root Node
    Node * RootNode = scene_ -> GetParent();

/// Get node list
    PODVector <Node *> NodesVector;
    scene_ -> GetChildren (NodesVector, true);

/// Set necessary objects
    Node * OrphanNode;
    String Nodename;

/// loop nodes
    for(int i=0; i < NodesVector.Size(); i++)
    {
        /// Do nothing like copy the node vector to a node
        OrphanNode = NodesVector[i];

        /// Prevent double game objects
        /// Add a component
        if (!OrphanNode -> HasComponent<GameObject>())
        {
            GameObject * OrphanNodeGameObject = OrphanNode-> CreateComponent<GameObject>();

            OrphanNodeGameObject -> SetLifetime(-1);
        }

    }

/// Create a character
/// Copy character information to player
    CreateCharacter();

/// Load main UI area
    loadSceneUI();

    /// rest of UI
    loadHUDFile("Resources/UI/MainTopBarWindow.xml",0,0);
    loadHUDFile("Resources/UI/PlayerInfoWindow.xml",0,34);

    /// load hud
    loadUIXML(UIQUICKMENU,0,0);

/// Get player info  name from temporary list and put it into the character object
    Text* PlayerNameText = (Text*)ui->GetRoot()->GetChild("PlayerNameText", true);

/// Get level text
    string levelstext=levels[0];

/// Set hud sting to level and character name
    string username="(0) "+levelstext+" "+character_->GetPlayerInfo().lastname;

    String playername(username.c_str());

    PlayerNameText -> SetText(playername.CString());

    UpdatePlayerInfoBar();

/// use UI cursor
/// Enable OS cursor
    ui->GetCursor()->SetVisible(true);

    if(ui->GetCursor()->IsVisible())
    {
        Print ("Cursor Exist");

        Print(ui->GetCursor()->GetAppliedStyle());
    }
    else
    {
        Print ("Cursor Does Not Exist");
    }

    return;
}


/// Load a scene UI
void ExistenceClient::loadSceneUI(void)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    ui->Clear();

    /// Removed old code use load quickmenu

    return;
}


/// Move camera used in camera fly mode
void ExistenceClient::MoveCamera(float timeStep)
{
    /// Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement()||ExistenceGameState.GetConsoleState())
    {
        return;
    }

    if(ExistenceGameState.GetGameState()==STATE_MAIN)
    {
        return;
    }

    Input* input = GetSubsystem<Input>();

    if(ExistenceGameState.GetCameraMode()==CAMERAMODE_FLY)
    {


        /// Movement speed as world units per second
        float MOVE_SPEED=5.0f;

        /// Mouse sensitivity as degrees per pixel
        const float MOUSE_SENSITIVITY = 0.2f;

        /// Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -180.0f, 180.0f);

        /// Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

        /// Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
        /// Use the Translate() function (default local space) to move relative to the node's orientation.
        if (input->GetKeyDown('W'))
        {
            cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('S'))
        {
            cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('A'))
        {
            cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('D'))
        {
            cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
        }

        /// added controls for up and down movement
        if (input->GetKeyDown('Q'))
        {
            cameraNode_->Translate(Vector3::UP * MOVE_SPEED * timeStep);
        }

        if (input->GetKeyDown('E'))
        {
            cameraNode_->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
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

/// Create a chacter
void ExistenceClient::CreateCharacter(void)
{
    /// Get all Revelant resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    Node* objectNode = scene_->GetChild("Character");

    /// Register Character component
    character_ = objectNode->CreateComponent<Character>();

    /// Copy character information
    character_ -> SetAlliance(TemporaryAccountPlayerList[TemporaryAccountPlayerSelected].GetAlliance());
    character_ -> SetCharacteristics(TemporaryAccountPlayerList[TemporaryAccountPlayerSelected].GetCharacteristics());
    character_ -> SetPlayerInfo(TemporaryAccountPlayerList[TemporaryAccountPlayerSelected].GetPlayerInfo());

    character_ -> SetHealth(100);

    /// Load a Character Mesh
    LoadCharacterMesh("Character",character_->GetAlliance().alienrace,character_->GetCharacteristics().gender);

    GameObject* charaterGameObject = objectNode -> CreateComponent<GameObject>();

    /// Create rigidbody, and set non-zero mass so that the body becomes dynamic
    RigidBody* body = objectNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);

    body->SetMass(.2f);

    /// Get static model and bounding box, calculate offset
    AnimatedModel * staticmodelreference = objectNode->GetComponent<AnimatedModel>();
    Model * staticmodel=staticmodelreference->GetModel();

    BoundingBox   staticmodelbox = staticmodel->GetBoundingBox();
    Vector3  staticmodelboxcenter= Vector3(staticmodelbox.max_-staticmodelbox.min_)/2;

    /// Set zero angular factor so that physics doesn't turn the character on its own.
    /// Instead we will control the character yaw manually
    body->SetAngularFactor(Vector3::ZERO);

    /// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(COLLISION_ALWAYS);

    /// Set a capsule shape for collision
    CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();

    /// Set shape collision
    shape->SetBox(Vector3::ONE);
    shape->SetPosition(Vector3(staticmodelboxcenter));
    shape->SetLodLevel(1);

    /// Gets the child position
    Node* headNode = objectNode->GetChild("head", true);

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    Node * cameraNode_ = headNode ->CreateChild("CameraFirstPerson");

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f,0.0f,0.15f));
    cameraNode_->SetRotation(Quaternion(0.0,0.0,0.0));

    /// Set first person camera Node
    Camera* cameraObject = cameraNode_->CreateComponent<Camera>();
    cameraObject->SetOrthographic(0);
    cameraObject->SetZoom(1);
    cameraObject->SetNearClip(0.0f);
    cameraObject->SetUseClipping(false);

    Node * crossboxNode = objectNode ->CreateChild("CrossBox");

    /// Set an initial position for the camera scene node above the plane
    crossboxNode->SetPosition(Vector3(0.0f,staticmodelboxcenter.y_+.75f,1.0f));
    crossboxNode->SetRotation(Quaternion(0.0,0.0,0.0));

    /// Get static model and bounding box, calculate offset
    StaticModel * crossboxModel = crossboxNode->CreateComponent<StaticModel>();

    crossboxModel ->SetModel(cache->GetResource<Model>("Resources/Models/CrossBox.mdl"));
    crossboxModel ->ApplyMaterialList("Resources/Models/CrossBox.txt");

    /// Set camera to first person
    ExistenceGameState.SetCameraMode(CAMERAMODE_FIRSTPERSON);

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    /// viewport -> SetCamera(cameraObject);
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraObject));
    renderer->SetViewport(0, viewport);

    effectRenderPath = viewport->GetRenderPath() -> Clone();
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));

    /// Make the bloom mixing parameter more pronounced
    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.9f, 0.6f));
    effectRenderPath->SetEnabled("Bloom", false);
    effectRenderPath->SetEnabled("FXAA3", false);
    viewport->SetRenderPath(effectRenderPath);

    character_->controls_.pitch_ = cameraNode_->GetRotation().PitchAngle();
    character_->controls_.yaw_ = cameraNode_->GetRotation().YawAngle();

    return;
}

/// Print to output buffe console
void ExistenceClient::Print(const String& output)
{
    /// Logging appears both in the engine console and stdout
    LOGRAW(output + "\n");
}

/// Load Character Mesh
int ExistenceClient::LoadCharacterMesh(String nodename, unsigned int alienrace, unsigned int gender)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    Node* objectNode = scene_->GetChild(nodename);

    /// Temporarily define faction information (Might make this a class)
    unsigned int factionslimit=4;
    unsigned int alienslimit=4;

    // select active
    alienraces Alien[alienslimit];
    string alienracename;

    /// Define structure
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

    alienracename.clear();

    /// Select currentalien
    for(unsigned int idx=0; idx<alienslimit; idx++)
    {
        if(alienrace==Alien[idx].uniqueid)
        {
            alienracename.append(Alien[idx].name);

        }
    }

    AnimatedModel* objectNodemodel=objectNode->CreateComponent<AnimatedModel>();

    /// Add Animation Controller
    objectNode->CreateComponent<AnimationController>();

    /// Setup mesh and mesh details nodes and stactic models
    if(gender<51)
    {
        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("female");

        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";

        objectNodemodel->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        objectNodemodel->ApplyMaterialList(String(temporarytexturefilename.c_str()));

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel* objectNodeoutfitbottom = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".txt";

        objectNodeoutfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeoutfittop = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".txt";

        objectNodeoutfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeteeth = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeteeth->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleteeth.mdl"));
        objectNodeteeth->ApplyMaterialList("Resources/Models/standardfemaleteeth.txt");

        AnimatedModel* objectNodeeyes = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeeyes->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleeyes.mdl"));
        objectNodeeyes->ApplyMaterialList("Resources/Models/standardfemaleeyes.txt");

        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianfemaleIdleAction.ani");

        objectNodemodel -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleAnimation");

        /// Add Walking Animation
        Animation * AddAnimation = new Animation(context_);
        AddAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianFemaleWalkAction.ani");

        objectNodemodel -> AddAnimationState(AddAnimation);
        AddAnimation -> SetAnimationName ("WalkAnimation");

    }
    else
    {

        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("male");

        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";
        objectNodemodel->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        objectNodemodel->ApplyMaterialList(String(temporarytexturefilename.c_str()));

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel* objectNodeoutfitbottom = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".txt";

        objectNodeoutfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeoutfittop = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".txt";

        objectNodeoutfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeteeth = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeteeth->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleteeth.mdl"));
        objectNodeteeth->ApplyMaterialList("Resources/Models/standardmaleteeth.txt");

        AnimatedModel* objectNodeeyes = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeeyes->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleeyes.mdl"));
        objectNodeeyes->ApplyMaterialList("Resources/Models/standardmaleeyes.txt");


        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianmaleIdleGuardAction.ani");

        objectNodemodel -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleAnimation");


        /// Add Walking Animation
        Animation * AddAnimation = new Animation(context_);
        AddAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianMaleWalkAction.ani");

        objectNodemodel -> AddAnimationState(AddAnimation);
        AddAnimation -> SetAnimationName ("WalkAnimation");

    }

    /// Get Controller
    AnimationController * playermeshAnimationController1 = objectNode -> GetComponent<AnimationController>();

    /// Set Morph Weight
    playermeshAnimationController1-> SetWeight("IdleAnimation",1);
    playermeshAnimationController1-> SetTime("IdleAnimation",1.89753);
    playermeshAnimationController1-> SetLayer("IdleAnimation",0);
    playermeshAnimationController1-> SetLooped("IdleAnimation",true);

    playermeshAnimationController1-> SetWeight("WalkAnimation",0);
    playermeshAnimationController1-> SetTime("WalkAnimation",2.45833);
    playermeshAnimationController1-> SetLayer("WalkAnimation",1);
    playermeshAnimationController1-> SetLooped("WalkAnimation",true);

    /// playermeshAnimationController1->Play("IdleAnimation",0,true,0);

    /// Set shado
    objectNodemodel->	SetCastShadows(true);


    return 1;
}

// code to handle actual commans
void ExistenceClient::GenerateScene(terrain_rule terrainrule,const char *differentialfilename)
{

    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_= GetSubsystem<EnvironmentBuild>();
    /// create variables (urho3d)
    String InputDataFile;

    /// Create Scene components
    scene_-> CreateComponent<Octree>();
    scene_-> CreateComponent<PhysicsWorld>();
    scene_-> CreateComponent<DebugRenderer>();

    /// clear api
    manager_->SetScene(scene_);
    environmentbuild_->SetScene(scene_);

    /// test creation
    if(terrainrule.creationtime==0)
    {
        int pick1=rand() % 1024;
        int pick2=rand() % 1024;

        terrainrule.creationtime=(unsigned long long int)pick1*pick2;
    }


    /// Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
    /// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
    /// generate the necessary 3D texture coordinates for cube mapping
    Node* skyNode = scene_->CreateChild("Sky");

    Skybox* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

    skyNode->SetScale(500.0f); /// The scale actually does not matter
    skyNode->SetName("GeneratedSkybox_Skybox1");


    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = scene_->CreateChild("DirectionalLight1");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(false);
    light->SetSpecularIntensity(0.4f);
    light->SetBrightness(.8);
    light->SetColor(Color(0.251f, 0.612f, 1.0f));

    lightNode->SetPosition(Vector3(0.0f,3.0f,0.0f));
    lightNode->SetName("GeneratedLight_Light1");

    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode2 = scene_->CreateChild("DirectionalLight2");
    Light* light2 = lightNode2->CreateComponent<Light>();
    light2->SetLightType(LIGHT_DIRECTIONAL);
    light2->SetCastShadows(true);
    light2->SetSpecularIntensity(.3f);
    light2->SetBrightness(.6);
    light2->SetColor(Color(1.0f, 1.0f,.95f));

    lightNode2->SetRotation(Quaternion(55.7392,0,0));
    lightNode2->SetPosition(Vector3(0.0f,3.0f,0.0f));
    lightNode2->SetName("GeneratedLight_Light2");

    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode3 = scene_->CreateChild("DirectionalLight3");
    Light* light3 = lightNode3->CreateComponent<Light>();
    light3->SetLightType(LIGHT_DIRECTIONAL);
    light3->SetCastShadows(true);
    light3->SetSpecularIntensity(0.2f);
    light3->SetBrightness(.4);
    light3->SetColor(Color(1.0f, 1.0f,.95f));

    lightNode3->SetRotation(Quaternion(39.1376,-180,-180));
    lightNode3->SetPosition(Vector3(0.0f,3.0f,0.0f));
    lightNode3->SetName("GeneratedLight_Light3");

    /// Define Terrain component information
    Node* terrainNode = scene_->CreateChild("Terrain");
    terrainNode ->SetName("GeneratedTerrainRule_TerrainRoot");

    Terrain* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(64);
    terrain->SetSpacing(Vector3((float)1/(4096/2048), 0.5f, (float)1/(4096/2048))); /// Spacing between vertices and vertical resolution of the height map
    terrain->SetSmoothing(true);
    terrain->SetCastShadows(true);

    /// Addcomponent
    ProceduralTerrain * terrainProcedural = terrainNode->CreateComponent<ProceduralTerrain>();

    /// Replacement random generator here
    RandomNumberGenerator RandomGenerator;

    RandomGenerator.SetRandomSeed(terrainrule.creationtime);

    /// Temporary use rule seed to choose octaves - set base values
    int octaves= RandomGenerator.RandRange(2)+6;

    int baseoctave=RandomGenerator.RandRange(30)+10;
    int basepersistencerandom = RandomGenerator.RandRange(50)+10;
    basepersistencerandom = (basepersistencerandom/(4096/2048));
    float basepersistence=(float)basepersistencerandom/100;

    /// Set base values of Octaves
    float octave1 = (float)(50+baseoctave)/100;
    float octave2 = (float) octave1*basepersistence;
    float octave3 = (float) octave2*basepersistence;
    float octave4 = (float) octave3*basepersistence;
    float octave5 = (float) octave4*basepersistence;
    float octave6 = (float) octave5*basepersistence;
    float octave7 = (float) octave6*basepersistence;
    float octave8 = (float) octave7*basepersistence;
    bool override=false;
    float persistence=basepersistence;


    /// Set component
    terrainProcedural -> Initialize();
    terrainProcedural -> SetDimensions(DEFAULTSIZE,DEFAULTSIZE);
    terrainProcedural -> SetWorldType(terrainrule.worldtype, terrainrule.subworldtype, terrainrule.sealevel, terrainrule.creationtime);
    terrainProcedural -> SetOctaves(override, octaves,  persistence, octave1,octave2,octave3,octave4,octave5,octave6,octave7,octave8);

    /// Generate Produracel map
    terrain->GenerateProceduralHeightMap(terrainrule);

    Image * producedHeightMapImage = new Image(context_);
    producedHeightMapImage -> SetSize(DEFAULTSIZE+1,DEFAULTSIZE+1, 1, 4);
    producedHeightMapImage -> SetData(terrain->GetData());

    terrain->SetMaterial(cache->GetResource<Material>("Materials/TerrainEdit.xml"));

    /// Get heightmap for texture blend
    Image * terrainHeightMap= new Image(context_);

    terrainHeightMap->SetSize(DEFAULTSIZE+1,DEFAULTSIZE+1,1,4);
    terrainHeightMap ->SetData(terrain -> GetHeightMap () -> GetData());

    terrainHeightMap -> FlipVertical();

    /// Generte image

    /// Define heightmap texture
    int bw=DEFAULTSIZE+1,bh=DEFAULTSIZE+1;

    Texture2D * blendtex=new Texture2D(context_);
    blendtex -> SetNumLevels(1);
    blendtex -> SetSize(0,0,0,TEXTURE_DYNAMIC);
    terrain-> GetMaterial() -> SetTexture(TU_DIFFUSE ,blendtex);

    /// Shared pointer for blend texture
    SharedPtr<Image> blend;
    SharedPtr<Image> blendMap;

    blend = new Image(context_);
    blend -> SetSize(bw,bh,1,4);
    blend -> Clear(Color(1,0,0,0));

    blendMap = new Image(context_);
    blendMap -> SetSize(bw,bh,1,4);
    blendMap -> Clear(Color(0,0,0,0));


    float steep=0.0f;
    float steepforlerp=0.0f;

    /// create blend here
    for(unsigned int x=0; x<bw; x++)
    {
        for(unsigned int y=0; y<bh; y++)
        {

            Color terrainHeightvalue=terrainHeightMap->GetPixel(x,y);

            switch(terrainrule.worldtype)
            {
            case WORLD_DESERT:
            {
                Color currentcolor = blend -> GetPixel(x,y);
                Color resultcolor=currentcolor.Lerp(Color(0.0f,1.0f,0.0f,0.0f), 1.0f);
                blend-> SetPixel(x,y,resultcolor);
            }
            break;
            default:
                /// Compare to sealavel
                if(terrainHeightvalue.r_<terrainrule.sealevel)
                {

                    Color currentcolor = blend -> GetPixel(x,y);

                    //               float mix=1.0f-((float)terrainHeightvalue.r_/terrainrule.sealevel);
                    float mix=(float)terrainHeightvalue.r_/terrainrule.sealevel;

                    float sterpforlerp=cutoff(mix,0.05f,0.040f,false);

                    Color resultcolor=currentcolor.Lerp(Color(0.0f,1.0f,0.0f,0.0f), sterpforlerp);

                    blend-> SetPixel(x,y,resultcolor);

                }
                break;
            }

            /// blend cliff
            Vector2 nworld=Vector2(x/(float)bw, y/(float)bh);
            Vector3 worldvalue=NormalizedToWorld( producedHeightMapImage,terrain,nworld);
            Vector3 normalvalue=terrain->GetNormal(worldvalue);

            steep=1.0f-normalvalue.y_;
            steepforlerp=cutoff(steep,0.05f,0.040f,false);

            Color currentcolor = blend -> GetPixel(x,y);

            int mixfactor=rand()%99;

            float mix=(float)(mixfactor+1)/100;

            // Color resultcolor=currentcolor.Lerp(Color(0,0,mix,1.0f-mix), steepforlerp);
            Color resultcolor=currentcolor.Lerp(Color(0,0,mix,1.0f-mix), steepforlerp);

            blend-> SetPixel(x,y,resultcolor);

        }
    }

    /// Rotate image and assign texture
    blend -> 	FlipVertical ();

    blend -> SavePNG("testing2.png");

    environmentbuild_ -> SetTextureMap(blend);

    blendtex ->SetData(blend, true);

    RigidBody* terrainbody = terrainNode->CreateComponent<RigidBody>();

    CollisionShape* terrainshape = terrainNode->CreateComponent<CollisionShape>();

    terrainbody->SetCollisionLayer(1);
    terrainshape->SetTerrain();

    Vector3 position(0.0f,0.0f);
    position.y_ = terrain->GetHeight(position) + 1.0f;

    /// Add node
    manager_->AddGeneratedObject(terrainNode);


    /// Position character
    Node * characternode_ = scene_->CreateChild("Character");
    characternode_->SetPosition(Vector3(0.0f, position.y_ , 0.0f));

    /// Get the materials
    Material * skyboxMaterial = skybox->GetMaterial();

    /// Change environment
    GenerateSceneUpdateEnvironment(terrainrule);

    /// Add objects functions
    GenerateSceneBuildWorld(terrainrule);

    /// Load differiental
    if(differentialfilename)
    {
        GenerateSceneLoadDifferential(differentialfilename);

    }

    return;
}

/// Load a HUD file in a XML format in the file system
bool ExistenceClient::loadHUDFile(const char * filename, const int positionx, const int positiony)
{
    /// Get resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    UI* ui_ = GetSubsystem<UI>();

    /// get current root
    UIElement * RootUIElement = ui_->GetRoot();
    UIElement * HUDFileElement= new UIElement(context_);
    UIElement * playerInfoHudElement= new UIElement(context_);

    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    /// Configure resources
    XMLElement hudElement;

    /// Configure string to Urho friendly
    String filenameHUD = String(filename);

    /// Load Resource
    XMLFile* hudFile= cache->GetResource<XMLFile>(filenameHUD);

    /// Get root element XML
    hudElement =  hudFile->GetRoot();

    /// Add a min top bar
    HUDFileElement-> LoadXML(hudElement, style);

    /// Add a uielement for the bar
    RootUIElement -> AddChild(HUDFileElement);

    /// Position the window
    HUDFileElement -> SetPosition(positionx,positiony);

    HUDFileElement->SetStyleAuto();

    return true;
}

/// Handler for personality selection item click
void ExistenceClient::HandlePersonalitySelectionItemClick(StringHash eventType, VariantMap& eventData)
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

/// Update player info
void ExistenceClient::UpdatePlayerInfoBar(void)
{

    /// Get resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    UI* ui_ = GetSubsystem<UI>();

    /// Get  UIElement
    UIElement * PlayerInfoUIElement = (UIElement*)ui_->GetRoot()->GetChild("PlayerInfoUIElement", true);

    /// Create a sprite
    Sprite * spriteSlot = new Sprite(context_);

    /// Load sprite
    spriteSlot->SetTexture(cache->GetResource<Texture2D>("Resources/Textures/blankindicatorlarge.png"));
    spriteSlot->SetPosition(13,6);
    spriteSlot->SetPriority(1);
    spriteSlot->SetVisible(true);
    spriteSlot->SetFixedSize(32,32);


    /// Create a sprite
    Sprite * spriteSlot2 = new Sprite(context_);

    /// Load sprite
    spriteSlot2->SetTexture(cache->GetResource<Texture2D>("Resources/Textures/blankindicatorsmall.png"));
    spriteSlot2->SetPosition(186,6);
    spriteSlot2->SetPriority(1);
    spriteSlot2->SetVisible(true);
    spriteSlot2->SetFixedSize(12,12);


/// Create a sprite
    Sprite * spriteSlot3 = new Sprite(context_);

    /// Load sprite
    spriteSlot3->SetTexture(cache->GetResource<Texture2D>("Resources/Textures/blankindicatorsmall.png"));
    spriteSlot3->SetPosition(206,6);
    spriteSlot3->SetPriority(1);
    spriteSlot3->SetVisible(true);
    spriteSlot3->SetFixedSize(12,12);

    PlayerInfoUIElement -> AddChild(spriteSlot);
    PlayerInfoUIElement -> AddChild(spriteSlot2);
    PlayerInfoUIElement -> AddChild(spriteSlot3);

    for(unsigned int i=236; i<277; i+=20)
    {
        Sprite * spriteSlotAdd = new Sprite(context_);

        /// Load sprite
        spriteSlotAdd->SetTexture(cache->GetResource<Texture2D>("Resources/Textures/blankindicatorsmall.png"));
        spriteSlotAdd->SetPosition(i,6);
        spriteSlotAdd->SetPriority(1);
        spriteSlotAdd->SetVisible(true);
        spriteSlotAdd->SetFixedSize(12,12);

        PlayerInfoUIElement -> AddChild(spriteSlotAdd);

    }
}

/// Routine for Console Environment related actions
int ExistenceClient::ConsoleActionEnvironment(const char * lineinput)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 10)
    {
        ssin >> argument[idx];
        ++idx;
    }

    /// parameters for zone related command
    if(argument[1]=="generatedbasicinfo")
    {
        /// Continue if a zone exist
        if(Node * terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true))
        {
            if(ProceduralTerrain * proceduralComponent = terrainNode-> GetComponent<ProceduralTerrain>())
            {

                /// Check component
                proceduralterrain proceduralterraindefaults;

                /// Get component values
                proceduralterraindefaults = proceduralComponent -> Get();

                float sealevel=proceduralterraindefaults.sealevel;
                int subworldtype=proceduralterraindefaults.subworldtype;
                int worldtype=proceduralterraindefaults.worldtype;
                unsigned int creationtime=proceduralterraindefaults.creationtime;
            }
        }
    }


    /// parameters for zone related command
    if(argument[1]=="zone")
    {
        /// Continue if a zone exist
        if(scene_->GetChild("GeneratedZone_Zone1",true))
        {
            /// Found a zone
            Node * SceneZoneNode = scene_->GetChild("GeneratedZone_Zone1",true);
            Zone * SceneZone = SceneZoneNode -> GetComponent<Zone>();

            ///  Change parameter for start end
            if(argument[2]=="startend")
            {
                SceneZone -> SetFogStart(StringToFloat(argument[3]));
                SceneZone -> SetFogEnd(StringToFloat(argument[4]));
            }


            ///  Change parameter for ambient color
            if(argument[2]=="ambientcolor")
            {
                SceneZone -> SetAmbientColor(Color(StringToFloat(argument[3]),StringToFloat(argument[4]),StringToFloat(argument[5])));

            }

            ///  Change parameter for fog color
            if(argument[2]=="fogcolor")
            {
                SceneZone -> SetFogColor(Color(StringToFloat(argument[3]),StringToFloat(argument[4]),StringToFloat(argument[5])));

            }
        }
    }

    /// parameters for zone related command
    if(argument[1]=="light")
    {
        /// Continue if a zone exist
        if(scene_->GetChild(argument[2].c_str(),true))
        {

            /// Found a zone
            Node * SceneLightNode = scene_->GetChild(argument[2].c_str(),true);
            Light * SceneLight = SceneLightNode -> GetComponent<Light>();

            if(argument[3]=="color")
            {

                SceneLight -> SetColor(Color(StringToFloat(argument[4]),StringToFloat(argument[5]),StringToFloat(argument[6])));

            }

            if(argument[3]=="brightness")
            {

                SceneLight -> SetBrightness(StringToFloat(argument[4]));

            }

            if(argument[3]=="specular")
            {

                SceneLight -> SetSpecularIntensity(StringToFloat(argument[4]));

            }

            if(argument[3]=="lighttype")
            {
                /// Use filename to change type
                if(argument[4]=="directional")
                {
                    SceneLight -> SetLightType(LIGHT_DIRECTIONAL);
                }

                if(argument[4]=="spot")
                {
                    SceneLight -> SetLightType(LIGHT_SPOT);
                }

                if(argument[4]=="point")
                {
                    SceneLight -> SetLightType(LIGHT_POINT);
                }
            }

        }
    }

/// parameters for zone related command - Old code
    if(argument[1]=="pointsteep")
    {
        /// Get Terrain
        Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
        Terrain * terrain = terrainNode -> GetComponent<Terrain>();

        /// get testing values
        float x_ =  StringToFloat(argument[3]);
        float y_ =  StringToFloat(argument[4]);
        float midpoint =  StringToFloat(argument[3]);
        float range =  StringToFloat(argument[4]);

        Image * producedHeightMapImage = new Image(context_);
        producedHeightMapImage -> SetSize(DEFAULTSIZE+1,DEFAULTSIZE+1, 1, 4);
        producedHeightMapImage -> SetData(terrain -> GetData());

        /// Testing
        float bw=DEFAULTSIZE+1;
        float bh=DEFAULTSIZE+1;

        /// Get steepness
        Vector2 nworld=Vector2(x_/bw, y_/bh);
        Vector3 worldvalue=NormalizedToWorld( producedHeightMapImage,terrain,nworld);

        Vector3 normalvalue=terrain->GetNormal(worldvalue);
        float steep=1.0f-abs(normalvalue.DotProduct(Vector3(0,1,0)));
        float steepforlerp=cutoff(steep,midpoint, range,true);

        std::ostringstream oss;
        oss << " Steep" << steep << " "<< normalvalue.x_<< " " << normalvalue.y_ << " " << normalvalue.z_<< " " << steepforlerp;
        std::string stringoutput = oss.str();

        Print(stringoutput.c_str());
    }

/// parameters for zone related command
    if(argument[1]=="terrainsize")
    {

        /// Get Terrain
        Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
        Terrain * terrain = terrainNode -> GetComponent<Terrain>();


        if(terrain)
        {

            Vector3 spacing=terrain->GetSpacing();
            int patchSize=terrain->GetPatchSize();

            IntVector2 numPatches= terrain -> GetNumPatches ();

            Vector2 patchWorldSize=Vector2(spacing.x_*(float)(patchSize*numPatches.x_), spacing.z_*(float)(patchSize*numPatches.y_));

            String Output=String("Terrain Size"+terrain->GetNumVertices().ToString()+"Patch World Size"+patchWorldSize.ToString());

            Print(Output);
        }

    }


    return 1;
}



/// Routine for Console Camera related actions
int ExistenceClient::ConsoleActionCamera(const char * lineinput)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 10)
    {
        ssin >> argument[idx];
        ++idx;
    }


    /// parameters for zone related command
    if(argument[1]=="firstpersonmode")
    {
        Node * cameraNode = scene_->GetChild("CameraFirstPerson",true);
        Camera* cameraObject = cameraNode->GetComponent<Camera>();

        /// create a new viewport
        SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraObject));
        renderer->SetViewport(0, viewport);

        ExistenceGameState.SetCameraMode(CAMERAMODE_FIRSTPERSON);
    }


    /// parameters for zone related command
    if(argument[1]=="flymode")
    {
        Node * cameraNode = scene_->GetChild("Camera",true);
        Camera* cameraObject = cameraNode->GetComponent<Camera>();

        /// create a new viewport
        SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraObject));
        renderer->SetViewport(0, viewport);

        ExistenceGameState.SetCameraMode(CAMERAMODE_FLY);
    }


    return 1;
}

/// Routine for Console Debug related actions
int ExistenceClient::ConsoleActionDebug(const char * lineinput)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 10)
    {
        ssin >> argument[idx];
        ++idx;
    }

    /// parameters for debug related command
    if(argument[1]=="hud")
    {
        /// toggle debug hud
        if(ExistenceGameState.GetDebugHudMode()==false)
        {
            ExistenceGameState.SetDebugHudMode(true);
            GetSubsystem<DebugHud>()->ToggleAll();
        }
        else
        {
            ExistenceGameState.SetDebugHudMode(false);
            GetSubsystem<DebugHud>()->ToggleAll();
        }
    }

    return 1;
}


/// Routine for Console Environment related actions
int ExistenceClient::ConsoleActionCharacter(const char * lineinput)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 10)
    {
        ssin >> argument[idx];
        ++idx;
    }

    /// parameters for debug related command
    if(argument[1]=="locate"||argument[1]=="getposition")
    {

        /// Get character node
        if(Node* objectNode = scene_->GetChild("Character"))
        {
            /// Get node position
            Vector3 position = objectNode->GetWorldPosition();

            /// Print character position
            Print ("Character located at Vector3("+position.ToString()+")");
        }
        else
        {
            Print ("Character node not located");
        }
    }

    return 1;
}

/// Routine for Console Environment related actions
int ExistenceClient::ConsoleActionRenderer(const char * lineinput)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 10)
    {
        ssin >> argument[idx];
        ++idx;
    }

    /// parameters for debug related command
    if(argument[1]=="bloom")
    {
        RenderPath* effectRenderPath = GetSubsystem<Renderer>()->GetViewport(0)->GetRenderPath();

        effectRenderPath->ToggleEnabled("Bloom");
    }

    /// parameters for debug related command
    if(argument[1]=="fxaa3")
    {
        RenderPath* effectRenderPath = GetSubsystem<Renderer>()->GetViewport(0)->GetRenderPath();

        effectRenderPath->ToggleEnabled("FXAA3");
    }

    return 1;
}

/// Build world
int ExistenceClient::GenerateSceneBuildWorld(terrain_rule terrainrule)
{
    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    EnvironmentBuild * environmentbuild_ = GetSubsystem<EnvironmentBuild>();

    /// Build world
    ///Node * WorldObjectNode = scene_-> CreateChild("EnvironmentBuildNode");
    ///EnvironmentBuild * EnvironmentBuildObjects = WorldObjectNode  -> CreateComponent<EnvironmentBuild>();

    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    /// Initialize
    environmentbuild_-> Initialize();

    environmentbuild_ -> GenerateWorldObjects(0, terrainrule);


    return 1;
}

/// Change environment
int ExistenceClient::GenerateSceneUpdateEnvironment(terrain_rule terrainrule)
{
    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Get skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
    /// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
    /// generate the necessary 3D texture coordinates for cube mapping
    Node* skyNode = scene_->GetChild("GeneratedSkybox_Skybox1",true);
    Skybox* skybox = skyNode->GetComponent<Skybox>();

    /*    /// Get a Zone component for ambient lighting & fog control
        Node* zoneNode = scene_->GetChild("GeneratedZone_Zone1",true);
        Zone* zone = zoneNode->GetComponent<Zone>();*/

    /// Get a directional light to the world. Enable cascaded shadows on it
    Node* lightNode1 = scene_->GetChild("GeneratedLight_Light1",true);
    Light* light1 = lightNode1->GetComponent<Light>();

    /// Get a directional light to the world. Enable cascaded shadows on it
    Node* lightNode2 = scene_->GetChild("GeneratedLight_Light2",true);
    Light* light2 = lightNode2->GetComponent<Light>();

    /// Get a directional light to the world. Enable cascaded shadows on it
    Node* lightNode3 = scene_->GetChild("GeneratedLight_Light3",true);
    Light* light3 = lightNode3->GetComponent<Light>();

    /// Generate Terrain
    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
    Terrain* terrain = terrainNode->GetComponent<Terrain>();

    /// Change texture
    switch (terrainrule.worldtype)
    {
    case WORLD_DESERT:
        /// Set light and skybox material
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox_Desert.xml"));
        LoadEnvironmentSettings("Skybox_Desert.xml");
        break;
    case WORLD_ICE:
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox_Ice.xml"));
        terrain->SetMaterial(cache->GetResource<Material>("Materials/TerrainTriPlanar-Ice.xml"));
        break;
    default:
        break;
    }

    return 1;
}

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


/// Routine for Console Environment related actions
int ExistenceClient::ConsoleActionBuild(const char * lineinput)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_ = GetSubsystem<EnvironmentBuild>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 10)
    {
        ssin >> argument[idx];
        ++idx;
    }

    /// Conole Command :/build move
    if(argument[1]=="moveobject")
    {
        /// Get character node
        if(Node* objectNode = scene_->GetChild(argument[2].c_str(),true))
        {

            /// Get node position
            Vector3 position = Vector3(StringToFloat(argument[3]),StringToFloat(argument[4]),StringToFloat(argument[5]));

            objectNode->SetPosition(position);

            /// Print character position
            Print ("Node "+String(argument[2].c_str())+" moved to ("+position.ToString()+").");
        }
        else
        {
            Print ("Missing Node. Node "+String(argument[2].c_str())+" cannot be moved");
        }
    }

    /// Conole Command :/build rotate
    if(argument[1]=="rotateobject")
    {
        /// Get character node
        if(Node* objectNode = scene_->GetChild(argument[2].c_str(),false))
        {

            /// Get node position
            Quaternion rotation = Quaternion(StringToFloat(argument[3]),StringToFloat(argument[4]),StringToFloat(argument[5]));

            ///RigidBody * objectNodeRigid=objectNode->GetComponent<RigidBody>();

            objectNode->SetRotation(rotation);

            /// Print character position
            Print ("Node "+String(argument[2].c_str())+" rotated to ("+rotation.ToString()+").");
        }
        else
        {
            Print ("Missing Node. Node "+String(argument[2].c_str())+" cannot be rotated.");
        }
    }

/// Conole Command :/build rotate
    if(argument[1]=="massobject")
    {
        /// Get character node
        if(Node* objectNode = scene_->GetChild(argument[2].c_str(),true))
        {

            if(RigidBody * objectNodeRigid=objectNode->GetComponent<RigidBody>())
            {
                /// if rigid body was created
                if(objectNodeRigid)
                {

                    /// Get node position
                    float mass= StringToFloat(argument[3]);

                    /// avoid zero size
                    if(mass<0)
                    {
                        mass=0;
                    }

                    /// Set Mass
                    objectNodeRigid -> SetMass(mass);
                }
            }
        }
        else
        {
            // blank
        }
    }
/// Conole Command :/build rotate
    if(argument[1]=="scaleobject")
    {
        /// Get character node
        if(Node* objectNode = scene_->GetChild(argument[2].c_str(),true))
        {

            /// Get node position
            float scale= StringToFloat(argument[3]);

            /// avoid zero size
            if(scale==0)
            {
                scale=0.01f;
            }

            objectNode->SetScale(scale);

            /// Print character position
            Print ("Node "+String(argument[2].c_str())+" scaled to "+scale+".");
        }
        else
        {
            Print ("Missing Node. Node "+String(argument[2].c_str())+" cannot be scaled.");
        }
    }


    /// Set Scene
    if(argument[1]=="setscene")
    {
        manager_->SetScene(scene_);
        environmentbuild_->SetScene(scene_);
    }

    /// Save scene
    if(argument[1]=="savescene")
    {
        manager_->SaveManagedNodes(argument[2].c_str());
        manager_->SaveManagedGeneratedNodes(argument[2].c_str());
    }


    /// Save scene
    if(argument[1]=="savemanaged")
    {
        manager_->SaveManagedNodes(argument[2].c_str());
    }


    /// Save scene
    if(argument[1]=="loadmanaged")
    {
        manager_->LoadManagedNodes(argument[2].c_str());
    }



    /// Save scene
    if(argument[1]=="savesceneraw")
    {
        manager_->SaveSceneRaw(argument[2].c_str());
    }


    /// parameters for debug related command
    if(argument[1]=="addobject")
    {
        int collisiontype=COLLISION_BOX;

        if (argument[8]=="box")
        {
            collisiontype=COLLISION_BOX;
        }
        else if(argument[8]=="convex")
        {
            collisiontype=COLLISION_CONVEX;
        }
        else if(argument[8]=="triangle")
        {
            collisiontype=COLLISION_TRIANGLE;
        }
        else
        {
            collisiontype=COLLISION_BOX;
        }

        /// add collision
        manager_-> AddObject(atoi(argument[2].c_str()),argument[3].c_str(), StringToFloat(argument[4]), StringToFloat(argument[5]), StringToFloat(argument[6]), argument[7].c_str(),collisiontype);
    }

    return 1;
}

int ExistenceClient::GenerateSceneLoadDifferential(const char *filename)
{
    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();

    /// create variables (urho3d)
    String additionresourcePath;

    additionresourcePath.Append(filesystem->GetProgramDir().CString());
    additionresourcePath.Append("Resources/ScenesData/");

    /// load
    manager_->LoadManagedNodes(filename);

}



/// Load account information from a account file
int ExistenceClient::LoadEnvironmentSettings(const char *environment)
{

    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    bool findmatch=false;

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 1;
    }

    /// Create String
    String loadenvironmentfile;

    /// Set directory
    loadenvironmentfile.Append(filesystem->GetProgramDir().CString());
    loadenvironmentfile.Append("Resources/ScenesData/");
    loadenvironmentfile.Append("environments.xml");

    /// Check if the account file information exist
    if(!filesystem->FileExists(loadenvironmentfile.CString()))
    {
        return 0;
    }

    XMLFile * loadingfile = new XMLFile(context_);

    /// Create a file in current context
    File LoadFile(context_, loadenvironmentfile.CString(), FILE_READ);

    loadingfile->Load(LoadFile);

    XMLElement ElementRoot = loadingfile->GetRoot("environments");

    XMLElement nextElement = ElementRoot.GetChild("node");

    do
    {
        /// Is enabled
        XMLElement nameElement;

        /// Is enabled
        nameElement = nextElement.GetChild ("attribute");
        /// name
        nameElement = nameElement.GetNext ("attribute");

        if(string(nameElement.GetAttributeCString("value"))==environment)
        {
            findmatch=true;
            break;
        }
        else
        {
            nextElement=nextElement.GetNext();
        }

    }
    while(nextElement.NotNull());

    /// return if node is not found
    if(findmatch==false)
    {
        return 0;
    }


    /// remove nodes
    Node * GeneratedLight1 = scene_ -> GetChild("GeneratedLight_Light1");
    Node * GeneratedLight2 = scene_ -> GetChild("GeneratedLight_Light2");
    Node * GeneratedLight3 = scene_ -> GetChild("GeneratedLight_Light3");

    /// remove
    GeneratedLight1 -> Remove();
    GeneratedLight2 -> Remove();
    GeneratedLight3 -> Remove();

    XMLElement lightElements = nextElement.GetChild("node");

    /// loop through each node
    do
    {
        /// Is enabled
        XMLElement AttributesPosition;

        /// Is enabled
        AttributesPosition = lightElements.GetChild ("attribute");

        /// name
        AttributesPosition = AttributesPosition.GetNext ("attribute");

        /// position
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        Vector3 NodePosition= AttributesPosition .GetVector3("value");

        /// rotation
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        Quaternion NodeRotation =  AttributesPosition.GetQuaternion ("value");

        /// rotation
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        float nodeScale = AttributesPosition.GetFloat("Scale");

        Node * newNode = scene_->InstantiateXML (lightElements,(const Vector3 &)Vector3(NodePosition),(const Quaternion &)Quaternion(NodeRotation),REPLICATED);

        lightElements=lightElements.GetNext();

    }
    while(lightElements.NotNull());


    return 1;
}



