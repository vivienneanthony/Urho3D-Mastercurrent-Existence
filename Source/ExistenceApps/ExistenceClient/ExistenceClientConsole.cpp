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
#include "ExistenceClientStateProgress.h"

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

#include "ExistenceClientConsole.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"

#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;

/// Initialize the main console
void ExistenceClient::InitializeConsole(void)
{
    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// create basic console
    Console* console = GetSubsystem<Console>();
    console->SetNumRows(2);
    console->SetNumBufferedRows(32);
    console->SetCommandInterpreter(GetTypeName());
    console->SetVisible(false);
    console->GetCloseButton()->SetVisible(false);

    gamestatehandlercomponent_ ->SetConsoleState(UI_CONSOLEOFF);
}


/// Handler for keydown
void ExistenceClient::HandlerFunctionKeyDown(StringHash eventType, VariantMap& eventData)
{
    /// Get Urho3D Subsystem
    UI* ui = GetSubsystem<UI>();

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    gamestatehandlercomponent_->SetConsoleState(GetSubsystem<Console>()->IsVisible());

    /// Unlike the other samples, exiting the engine when ESC is pressed instead of just closing the console
    if (eventData[KeyDown::P_KEY].GetInt() == KEY_F12)
    {
        if((gamestatehandlercomponent_->GetUIState()==UI_CHARACTERSELECTIONINTERFACE)||(gamestatehandlercomponent_->GetUIState()==UI_GAMECONSOLE))
        {
            if(gamestatehandlercomponent_->GetConsoleState())
            {
                /// Console Interface
                Console* console = GetSubsystem<Console>();

                console -> SetVisible(false);

                UI* ui = GetSubsystem<UI>();

                gamestatehandlercomponent_->SetConsoleState(false);

            }
            else
            {
                /// Console Interface
                Console* console = GetSubsystem<Console>();

                console -> SetVisible(true);
                gamestatehandlercomponent_->SetConsoleState(true);
            }
        }

    }
    return;
}


/// code to handle console command inputs
void ExistenceClient::HandleConsoleCommand(StringHash eventType, VariantMap& eventData)
{
    using namespace ConsoleCommand;

    if (eventData[P_ID].GetString() == GetTypeName())        HandleInput(eventData[P_COMMAND].GetString());
}

/// Code to handle actual commans
void ExistenceClient::HandleInput(const String& input)
{
    /// Get needed resources
    Renderer* renderer = GetSubsystem<Renderer>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    UI* ui = GetSubsystem<UI>();
    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();


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


    /// check if scene input
    if (parseinput[0] == "/scene")
    {

        if(gamestatehandlercomponent_->GetCurrentState()!="ExistenceClientStateMainScreen")
        {
            return;
        }

        /// remove scene
        String newString = String(input);
        newString.Replace(String("/scene"),String(""),false);

        /// erase scene
        eraseScene();

        /// Set console state
        gamestatehandlercomponent_->SetUIState(UI_GAMECONSOLE);

        /// Create a event
        VariantMap gamestatechange;
        gamestatechange[LoadState::P_CMD] = 100;
        gamestatechange[LoadState::P_ARG] = newString;

        cout << "Debug: Attempt to send a state change. Call MainScreen:ListenLoad(something like that)" << endl;

        SendEvent(P_LOAD_CHANGE,gamestatechange);


    }


    ///
    /// if it the game is in game mode go here
    else if(gamestatehandlercomponent_->GetCurrentState()=="ExistenceClientStateGameMode")
    {

        /// Check termination - Check Action
        if(parseinput[0] == "/end")
        {

            if(gamestatehandlercomponent_->GetDebugHudMode()==true)
            {
                gamestatehandlercomponent_->SetDebugHudMode(false);
                GetSubsystem<DebugHud>()->ToggleAll();
            }

            /// setup scene
            SetupScreenViewport();

            gamestatehandlercomponent_->SetCameraMode(CAMERAMODE_DEFAULT);

            /// Create a scene node for the camera, which we will move around
            /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
            cameraNode_ = scene_->GetChild("Camera");

            /// Set an initial position for the camera scene node above the plane
            cameraNode_->SetPosition(Vector3(2.0,0.0,5.0));

            /// Create a event
            VariantMap gamestatechange;
            gamestatechange[GameState::P_CMD] = GAME_STATE_MAINMENU;

            cout << "Debug: Attempt to send a state change" << endl;

            this->SendEvent(G_STATES_CHANGE,gamestatechange);

            return;

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

    /// Conole Command :/build move
    if(argument[1]=="moveobjectui")
    {
        /// Get character node
        if(Node* objectNode = scenePlayerUI_->GetChild(argument[2].c_str(),true))
        {
            /// Get node position
            Vector3 position = Vector3(StringToFloat(argument[3]),StringToFloat(argument[4]),StringToFloat(argument[5]));

            objectNode->SetPosition(position);

            if(argument[2].c_str()=="uidirectionallight")
            {
                /// get the character position
                Node * uicharacterNode = scenePlayerUI_ -> GetChild("Character",true);

                /// lookat the chracter
                objectNode->LookAt(uicharacterNode->GetPosition());
            }

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
    if(argument[1]=="rotateobjectui")
    {
        /// Get character node
        if(Node* objectNode = scenePlayerUI_ ->GetChild(argument[2].c_str(),false))
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

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();


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

        gamestatehandlercomponent_ ->SetCameraMode(CAMERAMODE_FIRSTPERSON);
    }


    /// parameters for zone related command
    if(argument[1]=="flymode")
    {
        Node * cameraNode = scene_->GetChild("Camera",true);
        Camera* cameraObject = cameraNode->GetComponent<Camera>();

        /// create a new viewport
        SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraObject));
        renderer->SetViewport(0, viewport);

        gamestatehandlercomponent_ ->SetCameraMode(CAMERAMODE_FLY);
    }


    return 1;
}

/// Routine for Console Debug related actions
int ExistenceClient::ConsoleActionDebug(const char * lineinput)
{
    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();


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
    if(argument[1]=="saveplayer")
    {

        playerlevels Templevels= character_ -> GetLevels();

        Templevels.experience=1000;

        character_ ->SetLevels(Templevels);

        SavePlayer(1);

    }
    /*

        /// parameters for debug related command
        if(argument[1]=="hud")
        {
            /// toggle debug hud
            if(ExistenceGameState->GetDebugHudMode()==false)
            {
                ExistenceGameState->SetDebugHudMode(true);
                GetSubsystem<DebugHud>()->ToggleAll();
            }
            else
            {
                ExistenceGameState->SetDebugHudMode(false);
                GetSubsystem<DebugHud>()->ToggleAll();
            }
        }
    */
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
