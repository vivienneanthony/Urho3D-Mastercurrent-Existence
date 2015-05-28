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

#include "../../../Urho3D/Procedural/Rules.h"

#include <time.h>

#include "../ExistenceApps.h"
#include "../Account.h"
#include "../factions.h"
#include "../Player.h"
#include "../PlayerLevels.h"


#include "GameStateHandler.h"

string ConvertUIntToString(unsigned int val);

/// This first example, maintaining tradition, prints a "Hello World" message.
/// Furthermore it shows:
///     - Using the Sample / Application classes, which initialize the Urho3D engine and run the main loop
///     - Adding a Text element to the graphical user interface
///     - Subscribing to and handling of update events
class ExistenceClient : public ExistenceApp
{
    OBJECT(ExistenceClient);


public:
    /// Construct.
    ExistenceClient(Context* context);

    /// Setup after engine initialization and before running the main loop.
    virtual void Start();

protected:
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

private:

    /// Diaplay login screen
    void SetupScreenViewport(void);
    void SetupScreenUI(void);

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
    int LoadPlayer(int player);
    int LoadTemporaryPlayer(int player);
    int GenerateSceneLoadDifferential(const char *filename=NULL);
    int LoadEnvironmentSettings(const char *environment);

    /// Console related functions
    void InitializeConsole(void);
    void HandleConsoleCommand(StringHash eventType, VariantMap& eventData);

    int ConsoleActionEnvironment(const char * lineinput);
    int ConsoleActionCamera(const char * lineinput);
    int ConsoleActionDebug(const char * lineinput);
    int ConsoleActionCharacter(const char * lineinput);
    int ConsoleActionRenderer(const char * lineinput);
    int ConsoleActionBuild(const char * lineinput);

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

    /// Window shared pointers
    SharedPtr<Window> window_;
    SharedPtr<Window> window2_;

    /// The UI's root UIElement.
    SharedPtr<UIElement> uiRoot_;
    SharedPtr<Viewport> viewport;
    SharedPtr<RenderPath> effectRenderPath;

    /// Remembered drag begin position.
    IntVector2 dragBeginPosition_;

    /// Shared pointer for input
    SharedPtr<Input> input_;

    /// This is temoporarily the necessary code
    bool accountexist;

    /// Shared pointer for a single character
    WeakPtr<Character> character_;

    /// Class and variable declation for character/player related information
    Player  TemporaryPlayer;
    Player  * TemporaryAccountPlayerList;
    unsigned int TemporaryAccountPlayerSelected;
    unsigned int TemporaryAccountPlayerListLimit;

    /// Class and variable declaration for alien race alliance information
    vector<string> aliensarray;
    vector<string> tempaliensarray;
};

/// Login State
class ExistenceClientStateLogin : public ExistenceClient
{
    OBJECT(ExistenceClientStateLogin);
public:
    ExistenceClientStateLogin(Urho3D::Context * context);
    virtual ~ExistenceClientStateLogin();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
private:
    void LoginUI(bool exist);
    void LoginScreenUINewAccountHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void LoginScreenUILoginHandleClosePressed(StringHash eventType, VariantMap& eventData);
protected:

};

/// Account State
class ExistenceClientStateAccount: public ExistenceClient
{
    OBJECT(ExistenceClientStateAccount);
public:
    ExistenceClientStateAccount(Urho3D::Context * context);
    virtual ~ ExistenceClientStateAccount();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
private:
    void CreateAccountScreenUI(void);
    void CreateAccountUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
protected:

};

/// Main Screen State
class ExistenceClientStateMainScreen: public ExistenceClient
{
    OBJECT(ExistenceClientStateMainScreen);
public:
    ExistenceClientStateMainScreen(Urho3D::Context * context);
    virtual ~ExistenceClientStateMainScreen();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
private:
    void mainScreenUI(void);
    void MainScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void HandleCharacterStartButtonReleased(StringHash eventType, VariantMap& eventData);
    void HandleCharacterSelectedReleased(StringHash eventType, VariantMap& eventData);
    void HandleCharacterSelectedInfoButtonReleased(StringHash eventType, VariantMap& eventData);
protected:

};

/// Player Create Login State
class ExistenceClientStatePlayer: public ExistenceClient
{
    OBJECT(ExistenceClientStatePlayer);
public:
    ExistenceClientStatePlayer(Urho3D::Context * context);
    virtual ~ExistenceClientStatePlayer();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
private:
    void CreatePlayerScreenUI(void);
    void HandlerCameraOrientation(StringHash eventType, VariantMap& eventData);
    void CameraOrientationRotateMove (float degrees, int movement);
    void HandleMouseReleased(StringHash eventType, VariantMap& eventData);
    void CreatePlayerUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void HandlePersonalitySelectionItemClick(StringHash eventType, VariantMap& eventData);
protected:

};

/// Main Screen State
class ExistenceClientStateProgress :public ExistenceClient
{
    OBJECT(ExistenceClientStateProgress);
public:
    ExistenceClientStateProgress(Urho3D::Context * context);
    virtual ~ExistenceClientStateProgress();
    virtual void Enter();
    virtual void Exit();
    virtual void OnUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData );
private:
    void ProgressScreenUI(void);
    void ProgressScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData);
    void CreateCharacter(void);
    void GenerateScene(terrain_rule terrainrule,const char *differentialfilename);
    void loadSceneCreationCreation(const char * lineinput);
    int GenerateSceneBuildWorld(terrain_rule terrainrule);
    void loadDummyScene(void);
    void LoadScene(const int mode, const char * lineinput);
protected:

};

/// Player Create Login State
class ExistenceClientStateGameMode: public ExistenceClient
{
    OBJECT(ExistenceClientStateGameMode);
public:
    ExistenceClientStateGameMode(Urho3D::Context * context);
    virtual ~ExistenceClientStateGameMode();
    virtual void Enter();
    virtual void Exit();
private:
    void GameMode(void);
protected:

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
