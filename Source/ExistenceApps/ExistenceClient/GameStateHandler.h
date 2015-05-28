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




namespace ExistenceClientStates
{

/// fw declaration
class ExistenceClient;

class GameStateHandler : public Urho3D::Object
{
    OBJECT(GameStateHandler);
public:
    ///costructor
    GameStateHandler(Urho3D::Context * context );
    /// Destruct.
    ~GameStateHandler();
    /// start point
    void start(Urho3D::Scene * scene_);
    // handler events
    void onStateChange(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    /// Get last state
    int getCurrentState(void);

private:
    /// create  state  classname
    void createState( Urho3D::String newState );
    /// register all states
    void RegisterGameStates();
    /// change state
    void changeState(ExistenceClient * state);
    /// exit and remove last state.
    void RemoveLastState();

    /// scene
    Urho3D::Scene * scene;

    /// states container
    Urho3D::Vector<ExistenceClient*> mStates;

};

}
#endif // GAMESTATEHANDLER_H
