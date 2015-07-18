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
#include "../../../Urho3D/UI/BorderImage.h"
#include "../../../Urho3D/Graphics/GraphicsEvents.h"
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
#include "../../../Urho3D/Script/ScriptFile.h"
#include "../../../Urho3D/Script/Script.h"
#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/Main.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>

#include "../ExistenceComponents/GameObject.h"
#include "../ExistenceComponents/InteractObject.h"
#include "../ExistenceComponents/DroneEntity.h"
#include "../ExistenceComponents/Character.h"
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

#include "ExistenceEditor.h"

#include "../../Urho3D/Engine/DebugHud.h"

#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;


// Expands to this example's entry-point
DEFINE_APPLICATION_MAIN(ExistenceEditor)


ExistenceEditor::ExistenceEditor(Context* context) :
    Application(context)
{
    /// Register

    //context_ -> RegisterFactory<Character> ("Character");
    //context_ -> RegisterFactory<GameObject> ("GameObject");
    //context_ -> RegisterFactory<InteractObject> ("InteractObject");
    //context_ -> RegisterFactory<ProceduralTerrain> ("PoceduralTerrain");

      /// Register
    Character::RegisterObject(context);
    GameObject::RegisterObject(context);
    InteractObject::RegisterObject(context);
    ProceduralTerrain::RegisterObject(context);
    DroneEntity::RegisterObject(context);

    cout << "Debig: Existence Client  Existence " << &applicationPtr << endl;

    /// Register states
    cout << "Debug: Existence Client Base Class Constructor context" << &context << " context_"<< &context_ << endl;

    return;
}

ExistenceEditor::~ExistenceEditor()
{
    cout << "Debug: Existence Client Base Class Deconstructor" << endl;

    return;
}


void ExistenceEditor::Setup()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    // Read command line from a file if no arguments given. This is primarily intended for mobile platforms.
    // Note that the command file name uses a hardcoded path that does not utilize the resource system
    // properly (including resource path prefix), as the resource system is not yet initialized at this point
    const String commandFileName = filesystem->GetProgramDir() + "Data/CommandLine.txt";
    if (GetArguments().Empty() && filesystem->FileExists(commandFileName))
    {
        SharedPtr<File> commandFile(new File(context_, commandFileName));
        String commandLine = commandFile->ReadLine();
        commandFile->Close();
        ParseArguments(commandLine, false);
        // Reparse engine startup parameters now
        engineParameters_ = Engine::ParseParameters(GetArguments());
    }

    // Check for script file name
    const Vector<String>& arguments = GetArguments();
    String scriptFileName;
    if (arguments.Size() && arguments[0][0] != '-')
        scriptFileName_ = GetInternalPath(arguments[0]);

    // Show usage if not found
    if (scriptFileName_.Empty())
    {
// Set file
        scriptFileName_=String("Scripts/Editor.as");
    }
    else
    {
        // Use the script file name as the base name for the log file
        engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("urho3d", "logs") + GetFileNameAndExtension(scriptFileName_) + ".log";
    }
}

void ExistenceEditor::Start()
{

    String extension = GetExtension(scriptFileName_);

    if (extension != ".lua" && extension != ".luc")
    {

        // Instantiate and register the AngelScript subsystem
        context_->RegisterSubsystem(new Script(context_));

        // Hold a shared pointer to the script file to make sure it is not unloaded during runtime
        scriptFile_ = GetSubsystem<ResourceCache>()->GetResource<ScriptFile>(scriptFileName_);

        /// \hack If we are running the editor, also instantiate Lua subsystem to enable editing Lua ScriptInstances

        // If script loading is successful, proceed to main loop
        if (scriptFile_ && scriptFile_->Execute("void Start()"))
        {
            // Subscribe to script's reload event to allow live-reload of the application
            SubscribeToEvent(scriptFile_, E_RELOADSTARTED, HANDLER(ExistenceEditor, HandleScriptReloadStarted));
            SubscribeToEvent(scriptFile_, E_RELOADFINISHED, HANDLER(ExistenceEditor, HandleScriptReloadFinished));
            SubscribeToEvent(scriptFile_, E_RELOADFAILED, HANDLER(ExistenceEditor, HandleScriptReloadFailed));
            return;
        }
    }

    // The script was not successfully loaded. Show the last error message and do not run the main loop
    ErrorExit();
}

void ExistenceEditor::Stop()
{
    if (scriptFile_)
    {
        // Execute the optional stop function
        if (scriptFile_->GetFunction("void Stop()"))
            scriptFile_->Execute("void Stop()");
    }

}

void ExistenceEditor::HandleScriptReloadStarted(StringHash eventType, VariantMap& eventData)
{

    if (scriptFile_->GetFunction("void Stop()"))
        scriptFile_->Execute("void Stop()");

}

void ExistenceEditor::HandleScriptReloadFinished(StringHash eventType, VariantMap& eventData)
{

    // Restart the script application after reload
    if (!scriptFile_->Execute("void Start()"))
    {
        scriptFile_.Reset();
        ErrorExit();
    }

}

void ExistenceEditor::HandleScriptReloadFailed(StringHash eventType, VariantMap& eventData)
{

    scriptFile_.Reset();
    ErrorExit();

}
