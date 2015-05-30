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
class Existence;

class GameStateHandler : public Urho3D::Object
{
    OBJECT(GameStateHandler);
public:
    ///costructor
    GameStateHandler(Context * context);
    /// Destruct.
    virtual  ~GameStateHandler();
    /// start point
    void Start(Urho3D::Scene * scene_);
    // handler events
    void onStateChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    /// Get last state
    int getCurrentState(void);
    // Register object factory and attributes.
    static void RegisterObject(Context* context);

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

    /// register all states
    void RegisterGameStates();
    /// create  state  classname
    void createState( Urho3D::String newState );
    /// change state
    void changeState(GameStateComponent * state);
    /// exit and remove last state.
    void RemoveLastState();

    /// scene
    Urho3D::Scene * scene;

    /// states container
    Urho3D::Vector<GameStateComponent*> mStates;

    /// Kept node just in case
    Urho3D::SharedPtr<Urho3D::Node> mainNode;


/// Added flags
    int consolestate;
    int uistate;
    int cameramode;
    int debughud;


};

#endif // GAMESTATEHANDLER_H
