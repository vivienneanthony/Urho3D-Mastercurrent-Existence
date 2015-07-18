#ifndef EXISTENCECLIENT_H
#define EXISTENCECLIENT_H

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

#define DISPLAYMESH_MUILTIPLECHARACTER 1
#define DISPLAYMESH_SINGLECHARACTER 2

#define UIBLANK 0
#define UIMAIN  1

#define CHARACTERMAINSCENE          0
#define CHARACTERUISCENE            1

#define CAMERAORIENTATIONROTATEYAW 1
#define CAMERAORIENTATIONROTATEPITCH 1


#include <vector>
#include <time.h>

#include "../../../Urho3D/Procedural/Rules.h"

#include "../ExistenceApps.h"
#include "../Account.h"
#include "../factions.h"
#include "../Player.h"
#include "../PlayerLevels.h"

#include "../ExistenceComponents/GameStateEvents.h"
#include "../ExistenceComponents/GameStateHandlerComponent.h"


string ConvertUIntToString(unsigned int val);

/// This first example, maintaining tradition, prints a "Hello World" message.
/// Furthermore it shows:
///     - Using the Sample / Application classes, which initialize the Urho3D engine and run the main loop
///     - Adding a Text element to the graphical user interface
///     - Subscribing to and handling of update events


using namespace std;


class ExistenceClient : public ExistenceApp
{
    OBJECT(ExistenceClient);

public:
    /// Mention friend classes
    friend class ExistenceClientStateSingleton;
    friend class ExistenceClientStateMainScreen;
    friend class ExistenceClientStatePlayer;
    friend class ExistenceClientStateAccount;
    friend class ExistenceClientStateLogin;
    friend class ExistenceClientStateProgress;
    friend class ExistenceClientStateGameMode;
    friend class ExistenceClientStateSplash;

    /// Construct.
    ExistenceClient(Context* context);
    virtual ~ExistenceClient();

    /// Setup after engine initialization and before running the main loop.
    virtual void Start();

    /// Return XML patch instructions for screen joystick layout for a specific sample app, if any.https://github.com/urho3d/Urho3D/tree/master/Source/Samples
    virtual String GetScreenJoystickPatchString() const
    {
        return
            "<patch>"
            "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]\">"
            "        <attribute name=\"Is Visible\" value=\"false\" />"
            "    </add>"
            "</patch>";
    }

    void Init(Context * context);

    /// Diaplay login screen
    void SetupScreenViewport(void);
    void SetupScreenUI(void);

    void Exit(void);
    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    /// Events Keyboard
    void HandleKeyDown(StringHash eventType, VariantMap& eventData);

    void HandleInput(const String& input);
    void eraseScene(void);

    void AddLogoViewport(void);

    int CreateCursor(void);

    void MoveCamera(float timeStep);
    void Print(const String& output);

    void HandlePostUpdates(StringHash eventType, VariantMap& eventData);

    /// Render related functions
    int LoadCharacterMesh(int mode, String nodename, unsigned int alienrace, unsigned int gender);
    int loadplayerMesh(Node * playermeshNode, int alienrace, int gender,int mode);

    /// File related functions
    void LoadAccount(void);
    void SaveAccount(accountinformation account);
    void SavePlayer(bool activeplayer);
    int LoadAccountPlayers(void);
    int LoadPlayer(int player) ;
    int LoadTemporaryPlayer(int player);
    int GenerateSceneLoadDifferential(const char *filename=NULL);
    int LoadEnvironmentSettings(const char *environment);

    /// Console related functions
    void InitializeConsole(void);
    void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);
    void HandlerFunctionKeyDown(StringHash eventType, VariantMap& eventData);


    int ConsoleActionEnvironment(const char * lineinput);
    int ConsoleActionCamera(const char * lineinput);
    int ConsoleActionDebug(const char * lineinput);
    int ConsoleActionCharacter(const char * lineinput);
    int ConsoleActionRenderer(const char * lineinput);
    int ConsoleActionBuild(const char * lineinput);

    void EraseUI(void);
    void EraseScene(void);
    /// UI Related Functions
    void loadSceneUI(void);
    bool loadHUDFile(const char * filename, const int positionx, const int positiony);
    void loadUIXMLClosePressed(StringHash eventType, VariantMap& eventData);
    bool loadUIXML(int windowtype, const int positionx, const int positiony, int selected);
    void QuickMenuPressed(StringHash eventType, VariantMap& eventData);
    void UpdateUI(float timestep);
    void PlayerWindowUpdateUI(int selected);
    void PlayerWindowHandleDisplaySelection(StringHash eventType, VariantMap& eventData);
    int UpdateUISceneLoader(void);
    void UpdatePlayerInfoBar(void);
    void SceneLoaderHanderPress(StringHash eventType, VariantMap& eventData);
    int GenerateSceneUpdateEnvironment(terrain_rule terrainrule);

    /// Temporary online
    bool IsClientConnected(void);
    bool ClientConnect(void);
    bool SetServerSettings(void);

    /// Get subsubsystems
    Renderer * GetRenderSubsystems(void) const;
    UI * GetUISubsystems(void) const;
    Graphics * GetGraphicsSubsystems(void) const;
    ResourceCache * GetResourceCacheSubsystems(void) const;

    Window * GetSharedWindow(void) const;

    /// Debug test string
    int GetTestString(void)
    {
        return testvalue;
    }

    /// Base class get
    SharedPtr <ExistenceClient> GetApplicationPTR(void) const
    {
        return applicationPtr;
    }


    SharedPtr<ExistenceClient> applicationPtr;


protected:

    /// Scene Shared Pointer
    SharedPtr<Scene> scene_;
    SharedPtr<Scene> scenePlayerUI_;
    SharedPtr<Scene> sceneLoadingGameModeTransition_;


    /// Camera scene node.
    SharedPtr<Node> cameraNode_;
    SharedPtr<Node> cameraNodePlayerUI_;
    SharedPtr<Scene> sceneLoadingGameModeTransitionUI_;

    /// Urho3D window shared pointers
    SharedPtr<Window> window_;
    SharedPtr<Window> window2_;

    /// Urho3D UIelement root, viewport, and render path
    SharedPtr<UIElement> uiRoot_;

    /// Viewport Shared
    SharedPtr<Viewport> viewport;

    /// RenderPath shared
    SharedPtr<RenderPath> effectRenderPath;

    /// Urho3D Shared pointer for input
    SharedPtr<Input> input_;

    /// Existence Weak pointer for a single character
    WeakPtr<Character> character_;

    /// Existence player structure class and variable declation for character/player related information
    Player  TemporaryPlayer;
    Player  * TemporaryAccountPlayerList;
    unsigned int TemporaryAccountPlayerSelected;
    unsigned int TemporaryAccountPlayerListLimit;

    /// Existence class and variable declaration for alien race alliance information
    vector<string> aliensarray;
    vector<string> tempaliensarray;

    /// This is temoporarily the necessary code
    bool accountexist;

    /// Server connection related
    bool ServerConnection;

    /// Debug Testing
    int testvalue;

    /// Camera yaw angle.
    float yaw_;
    /// Camera pitch angle.
    float pitch_;



private:
    void SplashShowGameLogo(void);
    void SplashStatInit(void);
    void HandlerSplashUpdate(StringHash eventType, VariantMap& eventData);
    void SplashSetupScreenViewport(void);

    Timer SplashTimer;
};

/// Login State
class ExistenceClientStateSingleton : public LogicComponent
{
    OBJECT(ExistenceClientStateSingleton);
public:
    SharedPtr<ExistenceClient> baseclass;
    ExistenceClientStateSingleton(Context * context);
    virtual ~ExistenceClientStateSingleton();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
    void Singleton(void);
protected:


};

/// Login State
class ExistenceClientStateSplash : public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStateSplash);
public:
    ExistenceClientStateSplash(Context * context);
    virtual ~ExistenceClientStateSplash();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
protected:
    void SplashScreen(void);
    void SplashSetupScreenViewport(void);
    void SplashShowGameLogo(void);

    void SplashStatInit(void);
    void HandlerSplashUpdate(StringHash eventType, VariantMap& eventData);

    Timer SplashTimer;
    SharedPtr<ExistenceClient> Existence;
  /// Open file as a Urho3d Datafile
    SharedPtr<File> dataFile;
    bool splashcompleted;


};

/// Login State
class ExistenceClientStateLogin : public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStateLogin);
public:
    ExistenceClientStateLogin(Context * context);
    virtual ~ExistenceClientStateLogin();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
    void LoginScreen(void);
    void LoginScreenUI(void);
    void LoginScreenUINewAccountHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void LoginScreenUILoginHandleClosePressed(StringHash eventType, VariantMap& eventData);

protected:
    SharedPtr<ExistenceClient> Existence;
};



/// Account State
class ExistenceClientStateAccount: public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStateAccount);
public:
    ExistenceClientStateAccount(Context * context);
    virtual ~ ExistenceClientStateAccount();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
    void Account(void);
    void CreateAccountScreenUI(void);
    void CreateAccountUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
protected:
/// pointer
    SharedPtr<ExistenceClient> Existence;

};

/// Main Screen State
class ExistenceClientStateMainScreen: public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStateMainScreen);

public:
    ExistenceClientStateMainScreen(Context * context);
    virtual ~ExistenceClientStateMainScreen();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
    void MainScreen(void);
    void MainScreenUI(void);
    void MainScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void HandleCharacterStartButtonReleased(StringHash eventType, VariantMap& eventData);
    void HandleCharacterSelectedReleased(StringHash eventType, VariantMap& eventData);
    void HandleCharacterSelectedInfoButtonReleased(StringHash eventType, VariantMap& eventData);
    void ListenToLoad(StringHash eventType, VariantMap& eventData);
protected:
    SharedPtr<ExistenceClient> Existence;
};

/// Main Screen State
class ExistenceClientStateGameMode: public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStateGameMode);
public:
    ExistenceClientStateGameMode(Context * context);
    virtual ~ExistenceClientStateGameMode();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void OnMoveCamera(float timeStep);
    bool Raycast(float maxDistance, Vector3& hitPos, Node*& hitNode);
    void GetTargetPressed(void);
private:
    void GameMode(void);
    void LoadGameModeUI(void);
    void HandlerPostUpdates(StringHash eventType, VariantMap& eventData);
    void InteractListener(StringHash eventType, VariantMap& eventData);
protected:
/// pointer
    SharedPtr<ExistenceClient> Existence;
};

/// Player Create Login State
class ExistenceClientStatePlayer: public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStatePlayer);
public:
    ExistenceClientStatePlayer(Context * context);
    virtual ~ExistenceClientStatePlayer();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
    void Player(void);
    void CreatePlayerScreenUI(void);
    void HandlerCameraOrientation(StringHash eventType, VariantMap& eventData);
    void CameraOrientationRotateMove (float degrees, int movement);
    void HandleMouseReleased(StringHash eventType, VariantMap& eventData);
    void CreatePlayerUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void loadSceneCreationCreation(const char * lineinput);
    void CreatePlayerUIHandleControlClicked(StringHash eventType, VariantMap& eventData);
    void HandlePersonalitySelectionItemClick(StringHash eventType, VariantMap& eventData);
protected:
    /// pointer
    SharedPtr<ExistenceClient> Existence;

};

/// Main Screen State
class ExistenceClientStateProgress :public ExistenceClientStateSingleton
{
    OBJECT(ExistenceClientStateProgress);
public:
    ExistenceClientStateProgress(Context * context);
    virtual ~ExistenceClientStateProgress();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
    virtual void SetParameter(String parameters_);
private:
    void Progress(void);
    void ProgressScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void CreateCharacter(void);
    void GenerateScene(terrain_rule terrainrule,const char *differentialfilename);
    int GenerateSceneBuildWorld(terrain_rule terrainrule);
    void loadDummyScene(void);
    bool loadScene(const int mode, String lineinput);

    /// Clear Viewport
    void ProgressScreenViewport(void);
    void ProgressScreenUI(void);

    /// Add a loader state
    void ProgressLoadOnStateChange(StringHash eventType, VariantMap& eventData);
    void ProgressSendEvent(int commandstatus, String message);
    void ProgessionHandleUpdate(StringHash eventType, VariantMap& eventData);

    /// Temporary alert
    void PopupWindowConfirm(const String &WindowName, const String &Title, const String &Message);

    void HandlerProgressLoadFailed(StringHash eventType, VariantMap& eventData);
    void HandlerProgressLoadSuccess(StringHash eventType, VariantMap& eventData);

    void AddLife(void);

    void AlternativeSendEvent(int event);

    void LoadTransition(void);

protected:
    /// pointer
    SharedPtr<ExistenceClient> Existence;

    /// Progress Screen
    SharedPtr<Scene> progressScene_;
    SharedPtr<UI> progressUI_;
    SharedPtr<RenderPath> progressrendererPath_;
    /// RenderPath shared
    SharedPtr<RenderPath> effectRenderPath;

    SharedPtr<Camera> progresscameraNode_;
    SharedPtr<Viewport> progressViewport_;
    SharedPtr<Window> progressWindow_;

    Timer ProgressTimer;
    String progressloadparameters_;

    int progressload_;
    int progressloadingstate;

    /// Open file as a Urho3d Datafile
    SharedPtr<File> dataFile;
    SharedPtr<File> ProgressDataFile;

    bool loadedtransition;
    WeakPtr<VariantMap> eventDataPtr;


    unsigned int progressloadmode_;
    unsigned int progressloadgenerated_;
};


/// Miscellanous functions
vector<string> split(const string& s, const string& delim, const bool keep_empty=true);
time_t ConvertStringToTime(const char * buff, time_t timeseed);
string GenerateName(char group, char subgroup);

string ConvertUIntToString(unsigned int val);
float cutoff(float inputvalue, float pointmid, float range,bool debug);
float StringToFloat(string buffer);
Vector3 NormalizedToWorld(Image *height, Terrain *terrain, Vector2 normalized);
typedef std::pair<float,float> range ;
bool intersects( range a, range b );
range make_range( float a, float b );

#endif


