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
#include "GameStateEvents.h"
#include "GameStateComponent.h"
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


///using namespace std;
using namespace Urho3D;

GameStateHandler::GameStateHandler(Context * context):
    Object(context)
    ,consolestate(0)
    ,uistate(0)
    ,debughud(0)
    ,cameramode(0)
{

    /// Set defaults
    uistate=UI_NONE;
    consolestate=UI_CONSOLEOFF;
    cameramode=CAMERAMODE_DEFAULT;
    debughud=false;

    context_ = context;

    /// Subscribe to event state change
    SubscribeToEvent(G_STATES_CHANGE, HANDLER(GameStateHandler, onStateChange));

    cout << "Debug: Game State Constructor context_ " << &context_ << " context " << &context << endl;

}

GameStateHandler::~GameStateHandler()
{
    /// Remove last state
    RemoveLastState();

    /// Destroy component
    LOGINFO("Destroyng GameComponent" );
}


void GameStateHandler::RegisterGameStates(Context* context)
{
    /// .... all states here
    context->RegisterFactory<ExistenceClientStateSingleton>();
    context->RegisterFactory<ExistenceClientStateAccount>();
    context->RegisterFactory<ExistenceClientStateGameMode>();
    context->RegisterFactory<ExistenceClientStateLogin>();
    context->RegisterFactory<ExistenceClientStatePlayer>();
    context->RegisterFactory<ExistenceClientStateProgress>();
    context->RegisterFactory<ExistenceClientStateMainScreen>();


    cout << "Debug: Game State Register States context_ " << &context << endl;


}


/// Register Object
void GameStateHandler::RegisterObject(Context* context)
{
    context->RegisterFactory<GameStateHandler>();

    cout << "Debug: Game State Register Game Handler context " << &context << endl;
}

void GameStateHandler::Start(void)
{

    ///mainNode = scene->CreateChild("Main");
    createState(ExistenceClientStateLogin::GetTypeNameStatic());

}

/// Temporary
String  GameStateHandler::getCurrentState(void)
{
    /// Not Used
    String stateString = String("test") ;

    //String stateString = mStates.Back().GetType();
    return stateString;
}

void GameStateHandler::onStateChange( Urho3D::StringHash eventType, Urho3D::VariantMap& eventData )
{
    /// intercept state event
    GameStates newState=  static_cast<GameStates>(eventData[GameState::P_CMD].GetInt());

    LOGINFO("New State " +  (String)((int)newState)) ;


    switch (newState)
    {
    case GAME_STATE_SINGLETON:
        createState(ExistenceClientStateSingleton::GetTypeNameStatic());
        break;
    case GAME_STATE_LOGIN:
        createState(ExistenceClientStateLogin::GetTypeNameStatic());
        break;
    case  GAME_STATE_ACCOUNTCREATE: //called from button on GameMainMenuSample form
        createState(ExistenceClientStateAccount::GetTypeNameStatic());
        break;
    case  GAME_STATE_PROGRESS:
        createState(ExistenceClientStateProgress::GetTypeNameStatic());
        break;
    case GAME_STATE_PLAYERCREATE:
        createState(ExistenceClientStatePlayer::GetTypeNameStatic());
        break;
    case GAME_STATE_MAINMENU:
        createState(ExistenceClientStateMainScreen::GetTypeNameStatic());
        break;
    case GAME_STATE_GAMEMODE:
        createState(ExistenceClientStateGameMode::GetTypeNameStatic());
        break;
    default:
        ErrorExit("Unkown State " + (String)(int) newState );
        break;
    }
}


void GameStateHandler::createState( String newState )
{
    /// switch states
    if(newState=="ExistenceClientStateSingleton")
    {


        myDerivedStates.push_back(new ExistenceClientStateSingleton(context_));
    }
    else if(newState=="ExistenceClientStateLogin")
    {
        cout << "Debug: Create State Login context_ " << &context_ << endl;

        myDerivedStates.push_back(new ExistenceClientStateLogin(context_));
    }
    else
    {
        ErrorExit("Unkown GameState ");
    }
}


/// Not used at the moment
void GameStateHandler::changeState( GameStateComponent* state )
{
    /*LOGINFO("Adding state" + state->GetTypeName());
    //exit from old state
    RemoveLastState();
    //enter  new state
    mStates.Push(state);
    mStates.Back()->Enter();*/

}

/// Not used at the moment
void GameStateHandler::changeState2(ExistenceClientStateSingleton * State)
{
    LOGINFO("Adding state" + State->GetTypeName());

    //exit from old state
    //RemoveLastState();
    //enter  new state
    //mStates.Push(state);
    //mStates.Back()->Enter();

}

/// Not used at the moment
void GameStateHandler::RemoveLastState()
{
    /*  if ( !mStates.Empty() )
      {
          mStates.Back()->Exit();
          //remove component's node  remove node and component
          mStates.Back()->GetNode()->Remove();
          mStates.Pop();
      }*/
}

int GameStateHandler::GetConsoleState(void)
{
    int flag;

    flag = consolestate;

    return flag;
}


int GameStateHandler::SetConsoleState(int flag)
{

    //consolestate=flag;

    return 1;
}

int GameStateHandler::GetUIState(void)
{
    int flag;
    flag=uistate;

    return flag;
}

int GameStateHandler::SetUIState(int flag)
{

    uistate = flag;

    return 1;
}

int GameStateHandler::GetCameraMode(void)
{
    int flag;

    flag = cameramode;

    return flag;;
}

int GameStateHandler::SetCameraMode(int flag)
{

    cameramode = flag;

    return 1;
}

int GameStateHandler::GetDebugHudMode(void)
{
    int flag;

    flag = debughud;

    return flag;;
}

int GameStateHandler::SetDebugHudMode(int flag)
{

    debughud = flag;

    return 1;
}

//}




