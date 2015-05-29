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
#include "GameStateEvents.h"
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
bool ExistenceClient::loadUIXML(int windowtype, const int positionx, const int positiony, int selected)
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

    /// chose based on menu type
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

    HUDFileElement->BringToFront();

    /// Get the child and assign a close pressed
    Button * closebutton = (Button *) HUDFileElement -> GetChild("closeButton",true);

    /// Check loaded XML for any UI update
    if(windowtype==UIPLAYERWINDOW )
    {
        /// Get the child and assign a close pressed
        Window * healthWindow = (Window *) HUDFileElement -> GetChild("HealthWindow",true);

        /// set healt to invisible
        healthWindow -> SetVisible(false);

        /// load temporary player
        if(selected)
        {
            LoadTemporaryPlayer(selected-1);
        }

        /// Update Player Info Update
        PlayerWindowUpdateUI(selected);

        /// get buttons assign a event
        Button * infoButton  = (Button *) HUDFileElement -> GetChild("PlayerInfoButton",true);
        Button * healthButton  = (Button *) HUDFileElement -> GetChild("PlayerHealthButton",true);

        SubscribeToEvent(infoButton, E_RELEASED, HANDLER(ExistenceClient, PlayerWindowHandleDisplaySelection));
        SubscribeToEvent(healthButton, E_RELEASED, HANDLER(ExistenceClient, PlayerWindowHandleDisplaySelection));

        /// Get the view window
        View3D * playerViewWindow = (View3D *) HUDFileElement -> GetChild("PlayerViewWindow",true);
        View3D * playerViewWindow2 = (View3D *) HUDFileElement -> GetChild("PlayerViewWindow2",true);

        /// Get camera NOde
        Node * cameraNodePlayerUI_ = scenePlayerUI_-> GetChild("uicamera", true);

        /// Add scene and camera to window
        playerViewWindow -> SetView(scenePlayerUI_,(Camera *)cameraNodePlayerUI_->GetComponent<Camera>(),true);
        playerViewWindow2 -> SetView(scenePlayerUI_,(Camera *)cameraNodePlayerUI_->GetComponent<Camera>(),true);

        /// Remove node children
        if(Node * characterNode = scenePlayerUI_->GetChild("Character"))
        {
            /// Clear everything
            characterNode -> RemoveAllChildren();
            characterNode -> RemoveAllComponents();

        }

        if(selected)
        {
            /// Load a Character Mesh
            LoadCharacterMesh(CHARACTERUISCENE, "Character",TemporaryPlayer.GetAlliance().alienrace,TemporaryPlayer.GetCharacteristics().gender);
        }
        else
        {

            /// Load a Character Mesh
            LoadCharacterMesh(CHARACTERUISCENE, "Character",character_->GetAlliance().alienrace,character_->GetCharacteristics().gender);
        }


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

    if(windowtype==UISCENESELECTWINDOW)
    {

        /// Add a playerid UIElement
        /// Get continue button
        Text * PlayerSelected = new Text(context_);

        HUDFileElement->AddChild(PlayerSelected);

        /// Add player selected
        PlayerSelected->SetName("PlayerSelected");
        PlayerSelected->SetText(String(ConvertUIntToString(selected).c_str()));


        /// add hidden
        UpdateUISceneLoader();
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
    /// Get roott
    UI* ui_ = GetSubsystem<UI>();

    UIElement * RootUIElement= ui_->GetRoot();


    /// Get control that was clicked
    UIElement* clicked = static_cast<UIElement*>(eventData[UIMouseClick::P_ELEMENT].GetPtr());

    /// Get the parent
    UIElement* selfparent = clicked -> GetParent();

    /// Disable and hide
    selfparent -> SetDeepEnabled(false);
    selfparent -> GetParent() -> Remove();


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
        if(ExistenceGameState->GetDebugHudMode()==true)
        {
            ExistenceGameState->SetDebugHudMode(false);
            GetSubsystem<DebugHud>()->ToggleAll();
        }

        eraseScene();

        GetSubsystem<Input>()->SetMouseVisible(true);

        /// set ui state to none
        ExistenceGameState->SetUIState(UI_CHARACTERSELECTIONINTERFACE);
        ///ExistenceGameState->SetGameState(STATE_MAIN);

        /// setup scene
        SetupScreenViewport();

        ExistenceGameState->SetCameraMode(CAMERAMODE_DEFAULT);

        ExistenceGameState->SendEvent("GAME_STATE_MAINSCREEN");

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
            PlayerWindow -> SetFocus(true);
        }
        else
        {
            loadUIXML(UIPLAYERWINDOW,200,200,0);
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
            PreferencesWindow -> SetFocus(true);
        }
        else
        {
            loadUIXML(UIPREFERENCESWINDOW,200,200,0);
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
            ConfigurationWindow -> SetFocus (true);
        }
        else
        {
            loadUIXML(UICONFIGURATIONWINDOW,200,200,0);
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
            AboutWindow -> SetFocus(true);
        }
        else
        {
            loadUIXML(UIABOUTWINDOW,200,200,0);
        }
    }

}

void ExistenceClient::UpdateUI(float timestep)
{
    return;
}

void ExistenceClient::PlayerWindowUpdateUI(int selected)
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
    string username;

    if(selected)
    {
        username=TemporaryPlayer.GetPlayerInfo().lastname + " " + TemporaryPlayer.GetPlayerInfo().firstname;
    }
    else
    {
        username=character_->GetPlayerInfo().lastname + " " + character_->GetPlayerInfo().firstname;
    }

    /// Set test
    String playername(username.c_str());

    PlayerNameText -> SetText(playername.CString());

    /// player alienrce
    unsigned int playeralienrace=0;

    if(selected)
    {
        playeralienrace = TemporaryPlayer.GetAlliance().alienrace;
    }
    else
    {
        playeralienrace = character_ -> GetAlliance().alienrace;
    }

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

            int aligned;

            if(selected)
            {
                TemporaryPlayer.GetAlliance().alienalliancealigned;
            }
            else
            {
                character_ -> GetAlliance().alienalliancealigned;
            }

            if(aligned==true)
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
    unsigned int level = 0;

    if(selected)
    {
        level=floor(TemporaryPlayer.GetLevels().level/10);
    }
    else
    {
        level=floor(character_->GetLevels().level/10);
    }

    string levelstring=ConvertUIntToString(level);

    string levelstext=levels[level];

    /// Set hud sting to level and character name
    string playerrank=levelstext+" ("+levelstring+") ";

    PlayerRankText -> SetText(playerrank.c_str());

    /// Create the ranking info
    string playerexperience;
    string playerreputationpercent;
    string playerreputation;
    string playerhealth;


    if(selected)
    {
        playerexperience=ConvertUIntToString(TemporaryPlayer.GetLevels().experience);
        playerreputationpercent=ConvertUIntToString((TemporaryPlayer.GetLevels().reputation/100000)*10);
        playerreputation=ConvertUIntToString(TemporaryPlayer.GetLevels().reputation);
        playerhealth=ConvertUIntToString(TemporaryPlayer.GetHealth())+"%";

    }
    else
    {

        playerexperience=ConvertUIntToString(character_->GetLevels().experience);
        playerreputationpercent=ConvertUIntToString((character_->GetLevels().reputation/100000)*10);
        playerreputation=ConvertUIntToString(character_->GetLevels().reputation);
        playerhealth=ConvertUIntToString(character_->GetHealth())+"%";

    }

    /// If experience element exist
    if(Text * PlayerExperienceResultText = (Text *)PlayerWindow -> GetChild("PlayerExperienceResultText",true))
    {
        PlayerExperienceResultText -> SetText(playerexperience.c_str());
    }

    /// If reputation element exist
    if(Text * PlayerReputationResultText = (Text *)PlayerWindow -> GetChild("PlayerReputationResultText",true))
    {
        string combinedreputation = playerreputation+" ("+playerreputationpercent+"%)";
        PlayerReputationResultText -> SetText(combinedreputation.c_str());
    }

    /// If reputation element exist
    if(Text * PlayerHealthResultText = (Text *)PlayerWindow -> GetChild("PlayerHealthResultText",true))
    {
        PlayerHealthResultText->SetText(playerhealth.c_str());
    }

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


// Load account information from a account file
int ExistenceClient::UpdateUISceneLoader(void)
{
    // Get resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    UI * ui_ = GetSubsystem<UI>();

    /// Get SceneListViewElement
    UIElement * uiroot = ui_ -> GetRoot();

    ListView * ScenesListView = dynamic_cast <ListView *>( uiroot->GetChild("ScenesListView",true));

    String scenesfile;

    /// Create scenes file
    scenesfile.Append(filesystem->GetProgramDir().CString());
    scenesfile.Append("CoreData/");
    scenesfile.Append("scenes.xml");

    /// Set XML related elements
    XMLFile* scenesload = cache->GetResource<XMLFile>(scenesfile);
    XMLElement scenesRootElement;
    XMLElement NextSibling;

    /// set flags
    ScenesListView->SetHighlightMode(HM_ALWAYS);
    ScenesListView->SetSelectOnClickEnd(false);
    ScenesListView->SetClearSelectionOnDefocus(false);
    ScenesListView->SetMultiselect (false);

        /// Set Counter
        unsigned int counter=0;

    /// If scenes file exist
    if(scenesload!=NULL)
    {
        /// get account root
        scenesRootElement= scenesload -> GetRoot("scenes");


        /// Get first child
        NextSibling=scenesRootElement.GetChild("scene");

        /// Loop to file till next
        do
        {
            /// Get  attributes
            XMLElement AttributeSceneNameElement  = NextSibling.GetChild("attribute");
            XMLElement AttributeSceneFileNameElement  =  AttributeSceneNameElement.GetNext();

            String SceneName = AttributeSceneNameElement.GetAttribute("value");
            String SceneFilename = AttributeSceneFileNameElement.GetAttribute("value");

            /// Get attributes information and generate the menu
            Text * SceneAdd = new Text(context_);

            /// Create value
            SceneAdd->SetEditable(true);
            SceneAdd->SetEnabled(true);

            SceneAdd->SetName(SceneFilename);
            SceneAdd->SetText(SceneName);


            SceneAdd->SetSelectionColor (Color(0.0f,0.0f,0.5f));
            SceneAdd->SetHoverColor (Color(0.0f,0.0f,1.0f));

            ScenesListView -> AddItem(SceneAdd);

            SceneAdd->SetStyleAuto();

            counter++;
        }
        while(NextSibling=NextSibling.GetNext());
    }

    /// set style
    ScenesListView->SetStyleAuto();
    ScenesListView->SetSelection(0);

    /// Get continue button
    Button * continueButton = dynamic_cast <Button*>( uiroot->GetChild("continueButton",true));

    if(counter)
    {

        /// Subscribe
        SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClient, SceneLoaderHanderPress));
    }

    return 0;
}



void ExistenceClient::SceneLoaderHanderPress(StringHash eventType, VariantMap& eventData)
{
    /// Get the button that was clicked
    UI* ui_ = GetSubsystem<UI>();

    /// Get needed info
    ListView* ScenesListView= (ListView*)ui_->GetRoot()->GetChild("ScenesListView", true);
    Text* PlayerSelected= (Text*)ui_->GetRoot()->GetChild("PlayerSelected", true);
    Text * selectedScene = dynamic_cast <Text *> (ScenesListView->GetSelectedItem ());

    /// Get Button String
    String clickedButtonString(PlayerSelected->GetText());

    /// Convert text to a integer number
    int button = clickedButtonString.Back()-'0';

    /// Set selected player
    TemporaryAccountPlayerSelected=button;

    /// Get Button String
    String clicked("/scene file "+selectedScene->GetName());

    /// erase scene
    eraseScene();

    /// change state
    ExistenceGameState->SetUIState(UI_GAMECONSOLE);

    ExistenceGameState->SendEvent("GAME_STATE_GAMEMODELOAD");

    /// load scene
    ///  loadScene(1,  clicked.CString());

    return;
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
