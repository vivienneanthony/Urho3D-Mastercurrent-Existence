#ifndef GAMESTATEHANDLER_H
#define GAMESTATEHANDLER_H


#define         UI_NONE                             0
#define         UI_LOGININTERFACE                   1
#define         UI_ACCOUNTCREATIONINTERFACE         2
#define         UI_CHARACTERCREATIONINTERFACE       3
#define         UI_CHARACTERSELECTIONINTERFACE      4
#define         UI_PROGRESSINTERFACE                5
#define         UI_GAMECONSOLE                      6

#define         STATE_NONE                          10
#define         STATE_MAIN                          11
#define         STATE_GAME                          12

#define         UI_CONSOLEOFF                     0
#define         UI_CONSOLEON                      1

#define         CAMERAMODE_DEFAULT                  0
#define         CAMERAMODE_FIRSTPERSON              1
#define         CAMERAMODE_FLY                      2

#include "GameStateComponent.h"


using namespace Urho3D;

/// fw declaration
class ExistenceClient;
class ExistenceClientStateSingleton;

class GameStateHandler : public Object
{
    OBJECT(GameStateHandler);
public:
    ///costructor
    GameStateHandler(Context * context);
    /// Destruct.
    virtual  ~GameStateHandler();
    /// start point
    void Start(void);
    // handler events
    void onStateChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    /// Get last state
    String getCurrentState(void);
    // Register object factory and attributes.
    static void RegisterObject(Context* context);
    /// register all states
    void RegisterGameStates();

    /// Function to access other states
    int GetConsoleState(void);
    int SetConsoleState(int flag);

    int GetUIState(void);
    int SetUIState(int flag);

    int GetCameraMode(void);
    int SetCameraMode(int flag);

    int GetDebugHudMode(void);
    int SetDebugHudMode(int flag);


private:


    /// create  state  classname
    void createState( Urho3D::String newState );
    /// change state
    void changeState(GameStateComponent * state);
    void changeState2(ExistenceClientStateSingleton * State);
    /// exit and remove last state.
    void RemoveLastState();

    /// Not used at the moment
    /// holder
    ExistenceClientStateSingleton * GameState;

    /// Not used at the moment
    /// states container
    Urho3D::Vector<GameStateComponent*> mStates;

    /// Not used at the moment
    /// Kept node just in case
    Urho3D::SharedPtr<Urho3D::Node> mainNode;

/// Added flags
    int consolestate;
    int uistate;
    int cameramode;
    int debughud;


    /// Vector Array - Derived States
    std::vector< ExistenceClientStateSingleton *> myDerivedStates;


};

#endif // GAMESTATEHANDLER_H
