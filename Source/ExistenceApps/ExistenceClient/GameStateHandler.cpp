#include <Urho3D/Urho3D.h>


///#include "Object.h"
#include "../../../Urho3D/Core/CoreEvents.h"
#include "../../../Urho3D/Resource/ResourceCache.h"
///#include "Context.h"
#include "../../../Urho3D/IO/Log.h"
///#include "Node.h"
///#include "../../../Urho3D/Core/ProcessUtils.h"


#include "GameStateHandler.h"

#include "GameStatesEvents.h"
#include "Existence.h"

using namespace std;
using namespace Urho3D;

namespace ExistenceClient
{

GameStateHandler::GameStateHandler(Urho3D::Context * context):
    Object(context)
    ,scene(0)

{
    SubscribeToEvent(G_STATES_CHANGE, HANDLER(GameStateHandler, onStateChange));
    RegisterGameStates();
}

GameStateHandler::~GameStateHandler()
{
    RemoveLastState();
    LOGINFO("Destroyng GameComponent" );
}

void GameStateHandler::RegisterGameStates()
{
    /// .... all states here
    context_->RegsiterFactory<ExistenceClientStateAccount>();
    context_->RegisterFactory<ExistenceClientStateGameMode>();
    context_->RegisterFactory<ExistenceClientStateLogin>();
    context_->RegisterFactory<ExistenceClientStatePlayer>();
    context_->RegisterFactory<ExistenceClientStateProgress>();
    context_->RegisterFactory<ExistenceClientStateMainScreen>();
}

void GameStateHandler::start(Urho3D::Scene * scene_)
{

    // set initialize of state
    gamestate=STATE_NONE;
    uistate=UI_NONE;

    consolestate=UI_CONSOLEOFF;
    cameramode=CAMERAMODE_DEFAULT;

    debughud=false;

    scene=scene_;
    if(scene)
    {
        ///mainNode = scene->CreateChild("Main");
        createState(ExistenceClientStateLogin::GetTypeNameStatic());
    }
    else
    {
        ErrorExit("Scene null");
    }

}

int GameStateHandler::getCurrentState(void)
{
    return mStates.Back();
}

void GameStateHandler::onStateChange( Urho3D::StringHash eventType, Urho3D::VariantMap& eventData )
{
    /// intercept state event
    GameStates newState=  static_cast<GameStates>(eventData[GameState::P_CMD].GetInt());
    ///LOGINFO("New State " +  (String)((int)newState)) ;

    switch (newState)
    {
    case  GAME_STATE_LOGIN:
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
    case GAME_STATE_MAINMENU;
        createState(ExistenceClientStateMainMenu::GetTypeNameStatic());
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
    /// add a node and a component
    /// so will be possible create / remove models attached it
    Node * stateNode = scene->CreateChild(newState);
    ExistenceClient  * gameState = dynamic_cast<ExistenceClient*>(stateNode->CreateComponent(newState));
    if(gameState)
    {

        changeState(gameState);

    }
    else
    {
        ErrorExit("Unkown GameState ");

    }


}

void GameStateHandler::changeState( ExistenceClient* state )
{
    LOGINFO("Adding state" + state->GetTypeName());
    //exit from old state
    RemoveLastState();
    //enter  new state
    mStates.Push(state);
    mStates.Back()->Enter();

}

void GameStateHandler::RemoveLastState()
{
    if ( !mStates.Empty() )
    {
        mStates.Back()->Exit();
        //remove component's node  remove node and component
        mStates.Back()->GetNode()->Remove();
        mStates.Pop();
    }
}

int GameStateHandler::GetConsoleState(void)
{
    bool flag;

    flag = consolestate;

    return flag;
}

int GameStateHandler::SetConsoleState(int flag)
{

    consolestate = flag;

    return 1;
}


int GameStateHandler::GetUIState(int flag)
{
    int flag
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

void GameStateHandler::SetDebugHudMode(int flag)
{

    debughud = flag;

    return;
}

}
