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
#include "Manager.h"
#include "../factions.h"

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

#include "../../Urho3D/Engine/DebugHud.h"

#include "ExistenceClient.h"
#include "ExistenceClientUI.h"

#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;

/// Load a HUD file in a XML format in the file system
bool ExistenceClient::loadUIXML(int windowtype, const int positionx, const int positiony)
{
    /// Get resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    UI* ui_ = GetSubsystem<UI>();
    Graphics* graphics = GetSubsystem<Graphics>();

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();


    /// get current root
    UIElement * RootUIElement = ui_->GetRoot();
    UIElement * HUDFileElement= new UIElement(context_);
    UIElement * playerInfoHudElement= new UIElement(context_);

    String filenameHUD;

    if(windowtype==UIQUICKMENU)
    {
        filenameHUD.Append("Resources/UI/QuickMenu.xml");
    }
    /// select interface based on input
    else if (windowtype==UIPLAYERWINDOW)
    {
        filenameHUD.Append("Resources/UI/PlayerWindow.xml");
    }
    else if (windowtype==UISCENESELECTWINDOW)
    {
        filenameHUD.Append("Resources/UI/SceneLoaderWindow.xml");
    }
    else if (windowtype==UIPREFERENCESWINDOW)
    {
        filenameHUD.Append("Resources/UI/PreferencesWindow.xml");
    }
    else if (windowtype==UICONFIGURATIONWINDOW)
    {
        filenameHUD.Append("Resources/UI/ConfigurationWindow.xml");
    }
    else if(windowtype==UIABOUTWINDOW)
    {
        filenameHUD.Append("Resources/UI/AboutWindow.xml");
    }
    else
    {
        return 0;
    }


    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    /// Configure resources
    XMLElement hudElement;

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

    /// Get the child and assign a close pressed
    Button * closebutton = (Button *) HUDFileElement -> GetChild("closeButton",true);

    /// if window type equals playerinfo
    if(windowtype==UIPLAYERWINDOW )
    {
        /// Get the child and assign a close pressed
        Window * healthWindow = (Window *) HUDFileElement -> GetChild("HealthWindow",true);

        /// set healt to invisible
        healthWindow -> SetVisible(false);

        /// Update UI
        PlayerWindowUpdateUI();
    }

    /// Set QuickMenu
    if(windowtype==UIQUICKMENU)
    {
        /// set position
        HUDFileElement -> SetPosition(width-18,100);

        /// get buttons assign a event
        Button * playerButton  = (Button *) HUDFileElement -> GetChild("PlayerButton",true);
        Button * preferencesButton  = (Button *) HUDFileElement -> GetChild("PreferencesButton",true);
        Button * aboutButton  = (Button *) HUDFileElement -> GetChild("AboutButton",true);
        Button * configurationButton  = (Button *) HUDFileElement -> GetChild("ConfigurationButton",true);
        Button * exitButton  = (Button *) HUDFileElement -> GetChild("ExitButton",true);

        SubscribeToEvent(playerButton, E_RELEASED, HANDLER(ExistenceClient, QuickMenuPressed));
        SubscribeToEvent(preferencesButton, E_RELEASED, HANDLER(ExistenceClient, QuickMenuPressed));
        SubscribeToEvent(aboutButton, E_RELEASED, HANDLER(ExistenceClient, QuickMenuPressed));
        SubscribeToEvent(configurationButton, E_RELEASED, HANDLER(ExistenceClient, QuickMenuPressed));
        SubscribeToEvent(exitButton, E_RELEASED, HANDLER(ExistenceClient, QuickMenuPressed));
    }
    else
    {
        /// Assign close function to the button
        SubscribeToEvent(closebutton, E_RELEASED, HANDLER(ExistenceClient, loadUIXMLClosePressed));
    }

    return true;
}

/// Load a HUD file in a XML format in the file system
void ExistenceClient::loadUIXMLClosePressed(StringHash eventType, VariantMap& eventData)
{

    /// Get control that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    /// Get the parent
    UIElement* selfparent = clicked -> GetParent();

    /// Disable and hide
    selfparent -> SetDeepEnabled(false);
    selfparent -> SetVisible(false);

    return;
}

void ExistenceClient::QuickMenuPressed(StringHash eventType, VariantMap& eventData)
{

    /// Get needed resources
    Renderer* renderer = GetSubsystem<Renderer>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui_ = GetSubsystem<UI>();

    /// get the button that was clicked
    Button* clicked = static_cast<Button*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    String clickedButtonString(clicked->GetName());

    string clickedtext;
    unsigned int clickedbutton=0;

    stringstream ss(clickedButtonString.CString());
    ss >> clickedtext >> clickedbutton;


    if (clickedtext=="ExitButton")
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

        return;
    }

    /// click player window
    if(clickedtext=="PlayerButton")
    {
        /// load window
        UIElement * uiroot = ui_->	GetRoot ();

        Window * PlayerWindow = (Window *) uiroot->GetChild("PlayerWindow",true);

        if(PlayerWindow)
        {
            /// Enable Player Window
            PlayerWindow -> SetDeepEnabled(true);
            PlayerWindow -> SetVisible(true);
            PlayerWindow -> SetFocus(true);
        }
        else
        {
            loadUIXML(UIPLAYERWINDOW,200,200);
        }


    }

    /// click Preferences window
    if(clickedtext=="PreferencesButton")
    {
        /// load window
        UIElement * uiroot = ui_->	GetRoot ();

        Window * PreferencesWindow = (Window *) uiroot->GetChild("PreferencesWindow",true);

        if(PreferencesWindow)
        {
            /// Enable Preferences Window
            PreferencesWindow -> SetDeepEnabled(true);
            PreferencesWindow -> SetVisible(true);
            PreferencesWindow -> SetFocus(true);
        }
        else
        {
            loadUIXML(UIPREFERENCESWINDOW,200,200);
        }


    }

/// click Configuration window
    if(clickedtext=="ConfigurationButton")
    {
        /// load window
        UIElement * uiroot = ui_->	GetRoot ();

        Window * ConfigurationWindow = (Window *) uiroot->GetChild("ConfigurationWindow",true);

        if(ConfigurationWindow)
        {

            /// Enable Configuration Window
            ConfigurationWindow -> SetDeepEnabled(true);
            ConfigurationWindow -> SetVisible(true);
            ConfigurationWindow -> SetFocus (true);
        }
        else
        {
            loadUIXML(UICONFIGURATIONWINDOW,200,200);
        }


    }

/// click About window
    if(clickedtext=="AboutButton")
    {
        /// load window
        UIElement * uiroot = ui_->	GetRoot ();

        Window * AboutWindow = (Window *) uiroot->GetChild("AboutWindow",true);

        if(AboutWindow)
        {

            /// Enable About Window
            AboutWindow -> SetDeepEnabled(true);
            AboutWindow -> SetVisible(true);
            AboutWindow -> SetFocus(true);
        }
        else
        {
            loadUIXML(UIABOUTWINDOW,200,200);
        }
    }

}


void ExistenceClient::UpdateUI(float timestep)
{
    return;
}

void ExistenceClient::PlayerWindowUpdateUI(void)
{
    /// Get ui subsystem
    UI* ui_ = GetSubsystem<UI>();

    UIElement * uiroot = ui_->	GetRoot ();

    /// Get node and update
    Window * PlayerWindow = (Window *) uiroot->GetChild("PlayerWindow",true);
    Text * PlayerNameText = (Text *)PlayerWindow -> GetChild("PlayerNameText",true);
    Text * PlayerRaceText = (Text *)PlayerWindow -> GetChild("PlayerRaceText",true);

    /// Set hud sting to level and character name
    string username=character_->GetPlayerInfo().lastname + " " + character_->GetPlayerInfo().firstname;

    /// Set test
    String playername(username.c_str());

    PlayerNameText -> SetText(playername.CString());

    /// player alienrce
    unsigned int playeralienrace = character_ -> GetAlliance().alienrace;

    /// Temporarily define faction information (Might make this a class)
    unsigned int alienslimit=4;
    alienraces Alien[alienslimit];

    Alien[0].uniqueid=1;
    Alien[0].name.append("Human");
    Alien[0].button.append("logohumans");

    Alien[1].uniqueid=100;
    Alien[1].name.append("Romulan");
    Alien[1].button.append("logoromulans");

    Alien[2].uniqueid=200;
    Alien[2].name.append("Klingon");
    Alien[2].button.append("logoklingons");

    Alien[3].uniqueid=1000;
    Alien[3].name.append("Orcin");
    Alien[3].button.append("logoorcins");

    /// Loop through until a match
    for(unsigned i=0; i<alienslimit; i++)
    {
        if(playeralienrace==Alien[i].uniqueid)
        {
            PlayerRaceText -> SetText(Alien[i].name.c_str());
        }
    }
}
