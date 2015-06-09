#ifndef GAMESTATEHANDLERCOMPONENT_H
#define GAMESTATEHANDLERCOMPONENT_H


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

#include "GameStateHandlerComponent.h"

using namespace Urho3D;
using namespace std;


class URHO3D_API  GameStateHandlerComponent : public LogicComponent
{
    OBJECT(GameStateHandlerComponent);
public:
    /// Constructor
    GameStateHandlerComponent(Context * context);
    /// Destruct.
    virtual  ~GameStateHandlerComponent();
    /// Register object factory and attributes.
    static void RegisterNewSubsystem(Context* context);

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

    /// Added flags
    int consolestate;
    int uistate;
    int cameramode;
    int debughud;


};

#endif // GAMESTATEHANDLER_H
