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
#include "../../../Urho3D/Graphics/Light.h"
#include "../../../Urho3D/Resource/ResourceCache.h"
#include "../../../Urho3D/Graphics/Renderer.h"
#include "../../../Urho3D/Graphics/Camera.h"
#include "../../../Urho3D/UI/Window.h"
#include "../../../Urho3D/UI/Button.h"
#include "../../../Urho3D/UI/LineEdit.h"
#include "../../../Urho3D/UI/UIElement.h"
#include "../../../Urho3D/Math/BoundingBox.h"
#include "../../../Urho3D/UI/UIEvents.h"
#include "../../../Urho3D/UI/View3D.h"
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

    /// Check loaded XML for any UI update
    if(windowtype==UIPLAYERWINDOW )
    {
        /// Get the child and assign a close pressed
        Window * healthWindow = (Window *) HUDFileElement -> GetChild("HealthWindow",true);

        /// set healt to invisible
        healthWindow -> SetVisible(false);

        /// Update Player Info Update
        PlayerWindowUpdateUI();

        /// get buttons assign a event
        Button * infoButton  = (Button *) HUDFileElement -> GetChild("PlayerInfoButton",true);
        Button * healthButton  = (Button *) HUDFileElement -> GetChild("PlayerHealthButton",true);

        SubscribeToEvent(infoButton, E_RELEASED, HANDLER(ExistenceClient, PlayerWindowHandleDisplaySelection));
        SubscribeToEvent(healthButton, E_RELEASED, HANDLER(ExistenceClient, PlayerWindowHandleDisplaySelection));

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
        //zone->SetAmbientColor(Color(0.01f, 0.01f, .01f));
        zone->SetFogColor(Color(.08f, .08f, .08f));
        zone->SetFogStart(10.0f);
        zone->SetFogEnd(20.0f);
        zone->SetHeightFog (false);

        ///Node* planeNode = scenePlayerUI_->CreateChild("Plane");
        ///planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
        ///StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
        ///planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

        /// Add Camera
        Node * cameraNodePlayerUI_ = scenePlayerUI_->CreateChild("uicamera");
        cameraNodePlayerUI_->CreateComponent<Camera>();

        /// Set an initial position for the camera scene node above the plane
        cameraNodePlayerUI_->SetPosition(Vector3(0.0f, 0.8f, 3.0f));

        /// Get the view window
        View3D * playerViewWindow = (View3D *) HUDFileElement -> GetChild("PlayerViewWindow",true);
        View3D * playerViewWindow2 = (View3D *) HUDFileElement -> GetChild("PlayerViewWindow2",true);

        /// Add scene and camera to window
        playerViewWindow -> SetView(scenePlayerUI_,(Camera *)cameraNodePlayerUI_->GetComponent<Camera>(),true);
        playerViewWindow2 -> SetView(scenePlayerUI_,(Camera *)cameraNodePlayerUI_->GetComponent<Camera>(),true);

        Node * emptyNode= scenePlayerUI_->CreateChild("uiempty");
        emptyNode->SetPosition(Vector3(0.0f,0.73f,0.0f));

        /// Create character node;
        Node * characterNode= scenePlayerUI_->CreateChild("Character");
        characterNode->SetPosition(Vector3(0.0f,0.0f,0.0f));

        cameraNodePlayerUI_ -> LookAt(Vector3(emptyNode->GetPosition()));
        lightNode -> Rotate(Quaternion(.398377,0.854323,0.141073,-0.302532));

        /// Load a Character Mesh
        LoadCharacterMesh(CHARACTERUISCENE, "Character",character_->GetAlliance().alienrace,character_->GetCharacteristics().gender);
    }

    /// If load is quickmenu on side assign buttons and move position  to far right
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


    /// If the close button exist
    if(closebutton)
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


    /// If exit was clicked
    if (clickedtext=="ExitButton")
    {
        if(ExistenceGameState.GetDebugHudMode()==true)
        {
            ExistenceGameState.SetDebugHudMode(false);
            GetSubsystem<DebugHud>()->ToggleAll();
        }

        scenePlayerUI_ -> Remove();

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
    Text * PlayerRankText = (Text *)PlayerWindow -> GetChild("PlayerRankText",true);
    Text * PlayerProgressText = (Text *)PlayerWindow -> GetChild("PlayerProgressText",true);

    /// Set hud sting to level and character name
    string username=character_->GetPlayerInfo().lastname + " " + character_->GetPlayerInfo().firstname;

    /// Set test
    String playername(username.c_str());

    PlayerNameText -> SetText(playername.CString());

    /// player alienrce
    unsigned int playeralienrace = character_ -> GetAlliance().alienrace;

    /// Temporarily define faction information (Might make this a class)
    unsigned int factionslimit=4;
    unsigned int alienslimit=4;

    factions Faction[factionslimit];
    alienraces Alien[alienslimit];

    String Alliance("Rouge");

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

    /// Loop through until a match
    for(unsigned i=0; i<alienslimit; i++)
    {
        /// find matching race
        if(playeralienrace==Alien[i].uniqueid)
        {

            if(character_ -> GetAlliance().alienalliancealigned==true)
            {


                for(unsigned int j=0; j<factionslimit; j++)
                {
                    unsigned int raceuniqueid =Alien[i].uniqueid;

                    /// configure faction
                    std::string raceuniqueidstring;
                    std::stringstream out;
                    out << raceuniqueid;
                    raceuniqueidstring = out.str();

                    /// find matching alliance
                    if(Faction[j].alliedraces.find(raceuniqueidstring))
                    {
                        Alliance.Clear();
                        Alliance.Append(Faction[j].name.c_str());
                        break;
                    }
                }
            }

            String TempText = String(Alien[i].name.c_str())+String(" "+Alliance);

            PlayerRaceText -> SetText(TempText);
        }
    }

    /// Get player level and level text
    unsigned int level=floor(character_->GetLevels().level/10);

    string levelstring=ConvertUIntToString(level);

    string levelstext=levels[level];

    /// Set hud sting to level and character name
    string playerrank=levelstext+" ("+levelstring+") ";

    PlayerRankText -> SetText(playerrank.c_str());

    /// Create the ranking info
    string playerexperience=ConvertUIntToString(character_->GetLevels().experience);
    string playerreputation=ConvertUIntToString((character_->GetLevels().reputation/100000)*10);

    string progresstext = "Experience "+playerexperience+" Reputation "+playerreputation+"%";

    PlayerProgressText -> SetText(progresstext.c_str());


}

void ExistenceClient::PlayerWindowHandleDisplaySelection(StringHash eventType, VariantMap& eventData)
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

    /// Get root element and the rest of the windows
    UIElement * uiroot = ui_ -> GetRoot();

    Window * PlayerWindow = (Window *) uiroot -> GetChild ("PlayerWindow", true);
    Window * healthWindow = (Window *) PlayerWindow -> GetChild ("HealthWindow", true);
    Window * infoWindow = (Window *) PlayerWindow -> GetChild ("InfoWindow", true);

    /// Debug output clicked button
    ///cout << clickedtext << endl;

    /// click Configuration window
    if(clickedtext=="PlayerInfoButton")
    {
        /// Turn off health window
        healthWindow -> SetDeepEnabled(false);
        healthWindow -> SetVisible(false);
        healthWindow -> SetFocus(false);

        infoWindow -> SetDeepEnabled(true);
        infoWindow -> SetVisible(true);
        infoWindow -> SetFocus(true);
    }

    /// click Configuration window
    if(clickedtext=="PlayerHealthButton")
    {

        infoWindow -> SetDeepEnabled(false);
        infoWindow -> SetVisible(false);
        infoWindow -> SetFocus(false);

        /// Turn off health window
        healthWindow -> SetDeepEnabled(true);
        healthWindow -> SetVisible(true);
        healthWindow -> SetFocus(true);
    }

    /// based on click choise activate a specific menu
    return;
}

