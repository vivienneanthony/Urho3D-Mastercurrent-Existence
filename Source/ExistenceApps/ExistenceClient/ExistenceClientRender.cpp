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
#include "../ExistenceComponents/Character.h"
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

#include "../ExistenceComponents/GameStateHandlerComponent.h"
#include "../ExistenceComponents/GameObject.h"
#include "../ExistenceComponents/EnvironmentBuild.h"
#include "../ExistenceComponents/Manager.h"
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
#include "ExistenceClientRender.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"


#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;

/// Erase the UI
void ExistenceClient::EraseUI(void)
{
    /// Get resources
    UI * ui_ = GetSubsystem<UI>();

    /// Clear
    ui_ -> Clear();

    return;
}

/// Erase the Scene_
void ExistenceClient::EraseScene(void)
{
    /// Clear scene
    scene_->Clear();

    return;
}


/// Load Character Mesh
int ExistenceClient::LoadCharacterMesh(int mode, String nodename, unsigned int alienrace, unsigned int gender)
{

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    Node * objectNode = new Node(context_);

    if(mode==CHARACTERMAINSCENE)
    {
        objectNode = scene_->GetChild(nodename);
    }

    if(mode==CHARACTERUISCENE)
    {
        objectNode = scenePlayerUI_->GetChild(nodename);
    }


    /// Temporarily define faction information (Might make this a class)
    unsigned int factionslimit=4;
    unsigned int alienslimit=4;

    // select active
    alienraces Alien[alienslimit];
    string alienracename;

    /// Define structure
    Alien[0].uniqueid=1;
    Alien[0].name.append("humans");
    Alien[0].button.append("logohumans");

    Alien[1].uniqueid=100;
    Alien[1].name.append("romulans");
    Alien[1].button.append("logoromulans");

    Alien[2].uniqueid=200;
    Alien[2].name.append("klingons");
    Alien[2].button.append("logoklingons");

    Alien[3].uniqueid=1000;
    Alien[3].name.append("orcins");
    Alien[3].button.append("logoorcins");

    alienracename.clear();

    /// Select currentalien
    for(unsigned int idx=0; idx<alienslimit; idx++)
    {
        if(alienrace==Alien[idx].uniqueid)
        {
            alienracename.append(Alien[idx].name);

        }
    }

    AnimatedModel* objectNodemodel=objectNode->CreateComponent<AnimatedModel>();

    /// Add Animation Controller
    objectNode->CreateComponent<AnimationController>();

    /// Setup mesh and mesh details nodes and stactic models
    if(gender<51)
    {
        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("female");

        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";

        objectNodemodel->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        objectNodemodel->ApplyMaterialList(String(temporarytexturefilename.c_str()));

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel* objectNodeoutfitbottom = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".txt";

        objectNodeoutfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeoutfittop = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".txt";

        objectNodeoutfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeteeth = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeteeth->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleteeth.mdl"));
        objectNodeteeth->ApplyMaterialList("Resources/Models/standardfemaleteeth.txt");

        AnimatedModel* objectNodeeyes = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeeyes->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleeyes.mdl"));
        objectNodeeyes->ApplyMaterialList("Resources/Models/standardfemaleeyes.txt");

        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianfemaleIdleAction.ani");

        objectNodemodel -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleAnimation");

        /// Add Walking Animation
        Animation * AddAnimation = new Animation(context_);
        AddAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianFemaleWalkAction.ani");

        objectNodemodel -> AddAnimationState(AddAnimation);
        AddAnimation -> SetAnimationName ("WalkAnimation");

    }
    else
    {

        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("male");

        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";
        objectNodemodel->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        objectNodemodel->ApplyMaterialList(String(temporarytexturefilename.c_str()));

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel* objectNodeoutfitbottom = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".txt";

        objectNodeoutfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeoutfittop = objectNode ->CreateComponent<AnimatedModel>();
        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".txt";

        objectNodeoutfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        objectNodeoutfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* objectNodeteeth = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeteeth->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleteeth.mdl"));
        objectNodeteeth->ApplyMaterialList("Resources/Models/standardmaleteeth.txt");

        AnimatedModel* objectNodeeyes = objectNode ->CreateComponent<AnimatedModel>();
        objectNodeeyes->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleeyes.mdl"));
        objectNodeeyes->ApplyMaterialList("Resources/Models/standardmaleeyes.txt");


        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianmaleIdleGuardAction.ani");

        objectNodemodel -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleAnimation");


        /// Add Walking Animation
        Animation * AddAnimation = new Animation(context_);
        AddAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianMaleWalkAction.ani");

        objectNodemodel -> AddAnimationState(AddAnimation);
        AddAnimation -> SetAnimationName ("WalkAnimation");

    }

    /// Get Controller
    AnimationController * playermeshAnimationController1 = objectNode -> GetComponent<AnimationController>();

    /// Set Morph Weight
    playermeshAnimationController1-> SetWeight("IdleAnimation",1);
    playermeshAnimationController1-> SetTime("IdleAnimation",1.89753);
    playermeshAnimationController1-> SetLayer("IdleAnimation",0);
    playermeshAnimationController1-> SetLooped("IdleAnimation",true);

    playermeshAnimationController1-> SetWeight("WalkAnimation",0);
    playermeshAnimationController1-> SetTime("WalkAnimation",2.45833);
    playermeshAnimationController1-> SetLayer("WalkAnimation",1);
    playermeshAnimationController1-> SetLooped("WalkAnimation",true);

    /// playermeshAnimationController1->Play("IdleAnimation",0,true,0);

    /// Set shado
    objectNodemodel->	SetCastShadows(true);


    return 1;
}


/// Erase a scene from the UI and Scene
void ExistenceClient::eraseScene(void)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    ui_->Clear();
    scene_->Clear();

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_ -> CreateComponent<Camera>();

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0,0.0,0.0));

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;

}
