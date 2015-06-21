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

#include "GameStateHandlerComponent.h"
#include "GameStateEvents.h"
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
#include "ExistenceClientStateProgress.h"
#include "ExistenceClientUI.h"


#include "../../Urho3D/Engine/DebugHud.h"

#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;
using namespace Update;

/// Constructor Destror
ExistenceClientStateProgress::ExistenceClientStateProgress(Urho3D::Context* context)
    :ExistenceClientStateSingleton (context)
    ,Existence(baseclass)
{

    /// Debug
    cout << "Debug: State Main Progress Constructor" << endl;

    /// int
    progressload_ = PROGRESSDEFAULT;
    progressloadingstate = PROGRESSSCENELOAD;

    /// Get component
    GameStateHandlerComponent * gamestatehandlercomponent_ = GetSubsystem<GameStateHandlerComponent>();

    /// Set aApplication
    Existence = gamestatehandlercomponent_->GetApplication();


    return;
}

ExistenceClientStateProgress::~ExistenceClientStateProgress()
{

    /// Debug
    cout << "Debug: State Main Progress Deconstructor" << endl;

    return;
}

void ExistenceClientStateProgress::SetParameter(String parameter_)
{
    /// Copy
    progressloadparameters_= parameter_;

    /// Debug
    cout << "Debug:State Main Progress SetParameter " << progressloadparameters_.CString() << endl;


    return;
}



void ExistenceClientStateProgress::Enter()
{

    /// Debug
    cout << "Debug: State Main Progress Enter" << endl;

    /// Subscribe to event (Listen to Load Change)
    SubscribeToEvent(P_LOAD_CHANGE, HANDLER(ExistenceClientStateProgress, ProgressLoadOnStateChange));

    /// Debug
    cout << "Debug:State Main Progress SubscribeToEvent(P_LOAD_CHANGE, HANDLER(ExistenceClientStateProgress, ProgressLoadOnStateChange))" << endl;

    /// Start progress
    Progress();

    return;
}

void ExistenceClientStateProgress::Exit()
{

    /// Debug
    cout << "Debug: State Main Progress Exit" << endl;

    return;
}

void ExistenceClientStateProgress::OnUpdate(StringHash eventType, VariantMap& eventData)
{
    return;
}


void ExistenceClientStateProgress::Progress(void)
{
    /// Temporary for use flag of states
    bool CurrentStateIsProgress = true;

    /// Setup screen
    ProgressScreenViewport();

    /// Load the UI
    ProgressScreenUI();

    /// Set Timer
    ProgressTimer.Reset();

    ///  Handle Post Updates
    SubscribeToEvent(E_POSTUPDATE,HANDLER(ExistenceClientStateProgress,ProgessionHandleUpdate));

    /// Load Scene
    loadScene(1, progressloadparameters_);

    return;
}


// Log UI Code
/// Create progress screen UI
void ExistenceClientStateProgress::ProgressScreenUI(void)
{

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    /// Clear the UI
    ui_->Clear();

    /// Get root
    UIElement * progressuiRoot_ = ui_-> GetRoot();

    /// set ui state to none
    ///ExistenceGameState->SetUIState(UI_PROGRESSINTERFACE);

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// Create the Window and add it to the UI's root node
    progressWindow_= new Window(context_);

    UIElement* titleBar = new UIElement(context_);
    Text* windowTitle = new Text(context_);
    Text* progressText=new Text(context_);
    Button* continueButton = new Button(context_);

    /// Set Window size and layout settings
    progressWindow_->SetMinSize(384, 192);
    progressWindow_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    progressWindow_->SetAlignment(HA_CENTER, VA_CENTER);
    progressWindow_->SetName("LoginWindow");
    progressWindow_->SetMovable(false);
    progressWindow_->SetOpacity(.6);

    /// Create Window 'titlebar' container
    titleBar->SetMinSize(0,32);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    windowTitle->SetName("Login");
    windowTitle->SetText("Accessing Server");

    progressText->SetName("progressText");
    progressText->SetText("Loading ghost characters states... ");

    /// addlones
    continueButton->SetName("Login");
    continueButton->SetStyle("continueButton");

    /// Add the controls to the title bar
    titleBar->AddChild(windowTitle);
    progressWindow_->AddChild(titleBar);
    progressWindow_->AddChild(progressText);
    progressWindow_->AddChild(continueButton);

    progressuiRoot_ -> AddChild(progressWindow_);

    /// Apply styles
    progressWindow_->SetStyleAuto();
    progressText->SetStyleAuto();
    windowTitle->SetStyleAuto();

    ///SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClientStateProgress, ProgressScreenUIHandleClosePressed));

    return;
}

// Progress screen UI function handler
void ExistenceClientStateProgress::ProgressScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ///ExistenceGameState->SetUIState(UI_PROGRESSINTERFACE);

    ///mainScreenUI();
    ///ExistenceGameState -> SendEvent("GAME_STATE_MAINSCREEN");

    return;
}



/// Create a chacter
void ExistenceClientStateProgress::CreateCharacter(void)
{
    /// Get all Revelant resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();


    Node* objectNode = Existence->scene_->GetChild("Character",true);

    /// Register Character component
    Existence -> character_ = objectNode->CreateComponent<Character>();

    Existence -> LoadPlayer(Existence->TemporaryAccountPlayerSelected);

    /// Create Character
    ProgressSendEvent(0, String("Creating and updating character ...  "));

    /// Load a Character Mesh
    Existence->LoadCharacterMesh(CHARACTERMAINSCENE, "Character",Existence->character_->GetAlliance().alienrace,Existence->character_->GetCharacteristics().gender);

    GameObject* charaterGameObject = objectNode -> CreateComponent<GameObject>();

    /// Create rigidbody, and set non-zero mass so that the body becomes dynamic
    RigidBody* body = objectNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);

    body->SetMass(.2f);

    /// Get static model and bounding box, calculate offset
    AnimatedModel * staticmodelreference = objectNode->GetComponent<AnimatedModel>();
    Model * staticmodel=staticmodelreference->GetModel();

    BoundingBox   staticmodelbox = staticmodel->GetBoundingBox();
    Vector3  staticmodelboxcenter= Vector3(staticmodelbox.max_-staticmodelbox.min_)/2;

    /// Set zero angular factor so that physics doesn't turn the character on its own.
    /// Instead we will control the character yaw manually
    body->SetAngularFactor(Vector3::ZERO);

    /// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    body->SetCollisionEventMode(COLLISION_ALWAYS);

    /// Set a capsule shape for collision
    CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();

    /// Set shape collision
    shape->SetBox(Vector3::ONE);
    shape->SetPosition(Vector3(staticmodelboxcenter));
    shape->SetLodLevel(1);

    Node * crossboxNode = objectNode ->CreateChild("CrossBox");

    /// Set an initial position for the camera scene node above the plane
    crossboxNode->SetPosition(Vector3(0.0f,staticmodelboxcenter.y_+.75f,1.0f));
    crossboxNode->SetRotation(Quaternion(0.0,0.0,0.0));

    /// Get static model and bounding box, calculate offset
    StaticModel * crossboxModel = crossboxNode->CreateComponent<StaticModel>();

    crossboxModel ->SetModel(cache->GetResource<Model>("Resources/Models/CrossBox.mdl"));
    crossboxModel ->ApplyMaterialList("Resources/Models/CrossBox.txt");

    return;
}

/// Generate Scene
void ExistenceClientStateProgress::GenerateScene(terrain_rule terrainrule,const char *differentialfilename)
{
    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_= GetSubsystem<EnvironmentBuild>();
    /// create variables (urho3d)
    String InputDataFile;

    /// Create Scene components
    Existence->scene_-> CreateComponent<Octree>();
    Existence->scene_-> CreateComponent<PhysicsWorld>();
    Existence->scene_-> CreateComponent<DebugRenderer>();

    /// clear api
    manager_->SetScene(Existence->scene_);
    environmentbuild_->SetScene(Existence->scene_);

    /// test creation
    if(terrainrule.creationtime==0)
    {
        int pick1=rand() % 1024;
        int pick2=rand() % 1024;

        terrainrule.creationtime=(unsigned long long int)pick1*pick2;
    }


    /// Create skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
    /// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
    /// generate the necessary 3D texture coordinates for cube mapping
    Node* skyNode = Existence->scene_->CreateChild("Sky");

    Skybox* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

    skyNode->SetScale(500.0f); /// The scale actually does not matter
    skyNode->SetName("GeneratedSkybox_Skybox1");


    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = Existence->scene_->CreateChild("DirectionalLight1");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(false);
    light->SetSpecularIntensity(0.4f);
    light->SetBrightness(.8);
    light->SetColor(Color(0.251f, 0.612f, 1.0f));

    lightNode->SetPosition(Vector3(0.0f,3.0f,0.0f));
    lightNode->SetName("GeneratedLight_Light1");

    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode2 = Existence->scene_->CreateChild("DirectionalLight2");
    Light* light2 = lightNode2->CreateComponent<Light>();
    light2->SetLightType(LIGHT_DIRECTIONAL);
    light2->SetCastShadows(true);
    light2->SetSpecularIntensity(.3f);
    light2->SetBrightness(.6);
    light2->SetColor(Color(1.0f, 1.0f,.95f));

    lightNode2->SetRotation(Quaternion(55.7392,0,0));
    lightNode2->SetPosition(Vector3(0.0f,3.0f,0.0f));
    lightNode2->SetName("GeneratedLight_Light2");

    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode3 = Existence->scene_->CreateChild("DirectionalLight3");
    Light* light3 = lightNode3->CreateComponent<Light>();
    light3->SetLightType(LIGHT_DIRECTIONAL);
    light3->SetCastShadows(true);
    light3->SetSpecularIntensity(0.2f);
    light3->SetBrightness(.4);
    light3->SetColor(Color(1.0f, 1.0f,.95f));

    lightNode3->SetRotation(Quaternion(39.1376,-180,-180));
    lightNode3->SetPosition(Vector3(0.0f,3.0f,0.0f));
    lightNode3->SetName("GeneratedLight_Light3");

    /// Define Terrain component information
    Node* terrainNode = Existence->scene_->CreateChild("Terrain");
    terrainNode ->SetName("GeneratedTerrainRule_TerrainRoot");

    Terrain* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(64);
    terrain->SetSpacing(Vector3((float)1/(4096/2048), 0.5f, (float)1/(4096/2048))); /// Spacing between vertices and vertical resolution of the height map
    terrain->SetSmoothing(true);
    terrain->SetCastShadows(true);

    /// Addcomponent
    ProceduralTerrain * terrainProcedural = terrainNode->CreateComponent<ProceduralTerrain>();

    /// Replacement random generator here
    RandomNumberGenerator RandomGenerator;

    RandomGenerator.SetRandomSeed(terrainrule.creationtime);

    /// Temporary use rule seed to choose octaves - set base values
    int octaves= RandomGenerator.RandRange(2)+6;

    int baseoctave=RandomGenerator.RandRange(30)+10;
    int basepersistencerandom = RandomGenerator.RandRange(50)+10;
    basepersistencerandom = (basepersistencerandom/(4096/2048));
    float basepersistence=(float)basepersistencerandom/100;

    /// Set base values of Octaves
    float octave1 = (float)(50+baseoctave)/100;
    float octave2 = (float) octave1*basepersistence;
    float octave3 = (float) octave2*basepersistence;
    float octave4 = (float) octave3*basepersistence;
    float octave5 = (float) octave4*basepersistence;
    float octave6 = (float) octave5*basepersistence;
    float octave7 = (float) octave6*basepersistence;
    float octave8 = (float) octave7*basepersistence;
    bool override=false;
    float persistence=basepersistence;


    /// Set component
    terrainProcedural -> Initialize();
    terrainProcedural -> SetDimensions(DEFAULTSIZE,DEFAULTSIZE);
    terrainProcedural -> SetWorldType(terrainrule.worldtype, terrainrule.subworldtype, terrainrule.sealevel, terrainrule.creationtime);
    terrainProcedural -> SetOctaves(override, octaves,  persistence, octave1,octave2,octave3,octave4,octave5,octave6,octave7,octave8);

    /// Generate Produracel map
    terrain->GenerateProceduralHeightMap(terrainrule);

    Image * producedHeightMapImage = new Image(context_);
    producedHeightMapImage -> SetSize(DEFAULTSIZE+1,DEFAULTSIZE+1, 1, 4);
    producedHeightMapImage -> SetData(terrain->GetData());

    terrain->SetMaterial(cache->GetResource<Material>("Materials/TerrainEdit.xml"));

    /// Get heightmap for texture blend
    Image * terrainHeightMap= new Image(context_);

    terrainHeightMap->SetSize(DEFAULTSIZE+1,DEFAULTSIZE+1,1,4);
    terrainHeightMap ->SetData(terrain -> GetHeightMap () -> GetData());

    terrainHeightMap -> FlipVertical();

    /// Generte image

    /// Define heightmap texture
    int bw=DEFAULTSIZE+1,bh=DEFAULTSIZE+1;

    Texture2D * blendtex=new Texture2D(context_);
    blendtex -> SetNumLevels(1);
    blendtex -> SetSize(0,0,0,TEXTURE_DYNAMIC);
    terrain-> GetMaterial() -> SetTexture(TU_DIFFUSE ,blendtex);

    /// Shared pointer for blend texture
    SharedPtr<Image> blend;
    SharedPtr<Image> blendMap;

    blend = new Image(context_);
    blend -> SetSize(bw,bh,1,4);
    blend -> Clear(Color(1,0,0,0));

    blendMap = new Image(context_);
    blendMap -> SetSize(bw,bh,1,4);
    blendMap -> Clear(Color(0,0,0,0));


    float steep=0.0f;
    float steepforlerp=0.0f;

    /// create blend here
    for(unsigned int x=0; x<bw; x++)
    {
        for(unsigned int y=0; y<bh; y++)
        {

            Color terrainHeightvalue=terrainHeightMap->GetPixel(x,y);

            switch(terrainrule.worldtype)
            {
            case WORLD_DESERT:
            {
                Color currentcolor = blend -> GetPixel(x,y);
                Color resultcolor=currentcolor.Lerp(Color(0.0f,1.0f,0.0f,0.0f), 1.0f);
                blend-> SetPixel(x,y,resultcolor);
            }
            break;
            default:
                /// Compare to sealavel
                if(terrainHeightvalue.r_<terrainrule.sealevel)
                {

                    Color currentcolor = blend -> GetPixel(x,y);

                    //               float mix=1.0f-((float)terrainHeightvalue.r_/terrainrule.sealevel);
                    float mix=(float)terrainHeightvalue.r_/terrainrule.sealevel;

                    float sterpforlerp=cutoff(mix,0.05f,0.040f,false);

                    Color resultcolor=currentcolor.Lerp(Color(0.0f,1.0f,0.0f,0.0f), sterpforlerp);

                    blend-> SetPixel(x,y,resultcolor);

                }
                break;
            }

            /// blend cliff
            Vector2 nworld=Vector2(x/(float)bw, y/(float)bh);
            Vector3 worldvalue=NormalizedToWorld( producedHeightMapImage,terrain,nworld);
            Vector3 normalvalue=terrain->GetNormal(worldvalue);

            steep=1.0f-normalvalue.y_;
            steepforlerp=cutoff(steep,0.05f,0.040f,false);

            Color currentcolor = blend -> GetPixel(x,y);

            int mixfactor=rand()%99;

            float mix=(float)(mixfactor+1)/100;

            // Color resultcolor=currentcolor.Lerp(Color(0,0,mix,1.0f-mix), steepforlerp);
            Color resultcolor=currentcolor.Lerp(Color(0,0,mix,1.0f-mix), steepforlerp);

            blend-> SetPixel(x,y,resultcolor);

        }
    }

    /// Rotate image and assign texture
    blend -> 	FlipVertical ();

    environmentbuild_ -> SetTextureMap(blend);

    blendtex ->SetData(blend, true);

    RigidBody* terrainbody = terrainNode->CreateComponent<RigidBody>();

    CollisionShape* terrainshape = terrainNode->CreateComponent<CollisionShape>();

    terrainbody->SetCollisionLayer(1);
    terrainshape->SetTerrain();

    Vector3 position(0.0f,0.0f);
    position.y_ = terrain->GetHeight(position) + 1.0f;

    /// Add node
    manager_->AddGeneratedObject(terrainNode);


    /// Position character
    Node * characternode_ = Existence->scene_->CreateChild("Character");
    characternode_->SetPosition(Vector3(0.0f, position.y_ , 0.0f));

    /// Get the materials
    Material * skyboxMaterial = skybox->GetMaterial();

    /// Change environment
    Existence->GenerateSceneUpdateEnvironment(terrainrule);

    /// Add objects functions
    GenerateSceneBuildWorld(terrainrule);

    /// Load differiental
    if(differentialfilename)
    {
        Existence->GenerateSceneLoadDifferential(differentialfilename);

    }

    return;
}

/// Build world for generated scene
int ExistenceClientStateProgress::GenerateSceneBuildWorld(terrain_rule terrainrule)
{
    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    EnvironmentBuild * environmentbuild_ = GetSubsystem<EnvironmentBuild>();

    /// Get Terrain Child
    Node* terrainNode = Existence->scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    /// Initialize
    environmentbuild_-> Initialize();

    environmentbuild_ -> GenerateWorldObjects(0, terrainrule);

    return 1;
}

/// Load a dummy scene
void ExistenceClientStateProgress::loadDummyScene(void)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    Existence->scene_-> CreateComponent<Octree>();
    Existence->scene_-> CreateComponent<DebugRenderer>();

    /// Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
    /// show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
    /// is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
    /// optimizing manner

    /// Create a child scene node (at world origin) and a StaticModel component into it. Set the StaticModel to show a simple
    /// plane mesh with a "stone" material. Note that naming the scene nodes is optional. Scale the scene node larger
    /// (100 x 100 world units)
    Node* planeNode = Existence->scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    /// Create rigidbody, and set non-zero mass so that the body becomes dynamic
    RigidBody* body = planeNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(1);

    /// Set a capsule shape for collision
    CollisionShape* shape = planeNode->CreateComponent<CollisionShape>();

    /// Set shape collision
    shape->SetBox(Vector3::ONE);
    shape->SetLodLevel(1);

    /// Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    /// light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    /// The light will use default settings (white light, no shadows)
    Node* lightNode = Existence->scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); /// The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    /// Create more StaticModel objects to the scene, randomly positioned, rotated and scaled. For rotation, we construct a
    /// quaternion from Euler angles where the Y angle (rotation about the Y axis) is randomized. The mushroom model contains
    /// LOD levels, so the StaticModel component will automatically select the LOD level according to the view distance (you'll
    /// see the model get simpler as it moves further away). Finally, rendering a large number of the same object with the
    /// same material allows instancing to be used, if the GPU supports it. This reduces the amount of CPU work in rendering the
    /// scene.
    const unsigned NUM_OBJECTS = 200;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* mushroomNode = Existence->scene_->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(0.5f + Random(2.0f));
        StaticModel* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
        mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
    }

    Node* characterNode = Existence->scene_->CreateChild("Character");
    characterNode->SetName("Character");

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    Existence->cameraNode_ = Existence->scene_->CreateChild("Camera");
    Existence->cameraNode_->CreateComponent<Camera>();

    /// Set an initial position for the camera scene node above the plane
    Existence->cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

    return;
}

/// Load a scene
bool ExistenceClientStateProgress::loadScene(const int mode, String lineinput)
{
    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Set Scene
    Manager * manager_ = GetSubsystem<Manager>();
    EnvironmentBuild * environmentbuild_= GetSubsystem<EnvironmentBuild>();

    /// clear api
    manager_->SetScene(Existence->scene_);
    environmentbuild_->SetScene(Existence->scene_);

    Input* input = GetSubsystem<Input>();

    /// string string leaving something comparable
    string argumentsstring = string(lineinput.CString());
    string argument[80];

    cout << "string" << argumentsstring.c_str()<< endl;

    /// create a idx
    int idx = 0;
    bool success = false;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 80)
    {
        ssin >> argument[idx];
        ++idx;
    }

    int testgenerate=1;
    int padded=0;

    GetSubsystem<Input>()->SetMouseVisible(false);

    unsigned long long int creationtime = 0; /// Default

    cout << "Argument1 " << argument[1].c_str() << endl;


    /// Run trhrough arugments - first check if GENERATE
    if(argument[0]=="generate")
    {
        ProgressSendEvent(0, String("Generating scene .... "));

        if(argument[1]=="random")
        {
            /// get current time
            time_t tempseed = time(NULL);

            creationtime = static_cast<unsigned long int> (tempseed);
        }
        else if(argument[1] =="datetime")
        {

            /// get current time
            time_t tempseed = time(NULL);

            /// Convert time and change timespeed if valid
            if(tempseed = ConvertStringToTime(argument[3].c_str(), tempseed))
            {
                creationtime = static_cast<unsigned long long int > (tempseed);
            }
            else
            {

                creationtime = static_cast<unsigned long long int > (tempseed);
            }
            padded=1;
        }
        else if(argument[1]=="seed")
        {
            std::string::size_type sz;   // alias of size_t

            creationtime=atoi(argument[3].c_str());
            padded=1;
        }
        else
        {
            /// get current time
            creationtime = 0;
        }

        /// Copy rule information
        terrain_rule terrainrule;

        /// Copy rule
        terrainrule.worldtype=(int)atoi(argument[3+padded].c_str());
        terrainrule.subworldtype=(int)atoi(argument[4+padded].c_str());
        terrainrule.sealevel=(float)atof(argument[5+padded].c_str());

        /// Set timeseed
        /// terrainrule.timeseed=timeseed;   /// temporary
        terrainrule.creationtime=creationtime; /// new for randomizing

        /// generate a seen
        GenerateScene(terrainrule, argument[6+padded].c_str());

        success=true;
    }
/// Run trhrough arugments - first check if FILE
    else if (argument[0]=="file")
    {
        ProgressSendEvent(0, String("Locating file scene ... "));

        /// Create variables (urho3d)
        String InputDataFile;

        InputDataFile.Append(filesystem->GetProgramDir().CString());
        InputDataFile.Append("Resources/Scenes/");
        InputDataFile.Append(argument[1].c_str());

        /// Debug Info
        cout << "Debug : Scene load file " << InputDataFile.CString() << endl;

        /// Check if the input data file exist
        if(filesystem->FileExists(InputDataFile))
        {
            /// Open file as a Urho3d Datafile
            dataFile = new File(context_, InputDataFile, FILE_READ);

            if (dataFile -> IsOpen())
            {

                /// Get File Extension
                String extension = GetExtension(InputDataFile);

                /// Load File based on Extension
                ProgressSendEvent(0, String("Opening file scene  ... "));

                /// Determine file extension
                if (extension != ".xml")
                {
                    ///success= Existence-> scene_ -> Load(dataFile);
                    success = Existence-> scene_ -> LoadAsync(dataFile);
                }
                else
                {
                    success= Existence-> scene_ ->LoadAsyncXML(dataFile);
                }
            }
            else
            {
                /// set is error
                success=false;
            }
        }
        else
        {
            /// Send event loading dummy scene automatic success
            ProgressSendEvent(0, String("Loading a dummy scene ... "));

            /// Load dummy scene
            loadDummyScene();

            success= true;
        }
    }
    else
    {
        /// Send event loading dummy scene automatic success
        ProgressSendEvent(0, String("Loading a dummy scene ... "));

        /// load dummy scene
        loadDummyScene();

        success= true;
    }

    /// Get the Camera Node and setup the viewport
    if(!(Existence->cameraNode_ = Existence->scene_->GetChild("Camera")))
    {
        /// Create a scene node for the camera, which we will move around
        /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
        Existence->cameraNode_ = new Node(context_);

        Existence->cameraNode_ = Existence->scene_->CreateChild("Camera");
        Existence->cameraNode_->CreateComponent<Camera>();

        Camera* camera = Existence->cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(1000.0f);

        ///Set an initial position for the camera scene node above the ground
        Existence->cameraNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

    }
    else
    {
        /// Cannot locate camera so return false
        success = false;
    }


    return success;
}

/// Setup the viewport
void ExistenceClientStateProgress::ProgressScreenViewport(void)
{
    /// Get resources and clear everything
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// CLEAR_COLOR
    Existence -> EraseScene();
    Existence -> EraseUI();

    /// Create a new scene
    progressScene_ = new Scene(context_);
    progressUI_ = new UI(context_);

    /// Add Octree
    progressScene_ ->CreateComponent<Octree>();

    /// Add a camera node
    Node * cameraNode_ = progressScene_ -> CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();

    /// Setup Current Viewport
    SharedPtr<Viewport> progressViewport_(new Viewport(context_, progressScene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, progressViewport_);

    return;
}

/// handler for progress load
void ExistenceClientStateProgress::ProgressLoadOnStateChange(StringHash eventType, VariantMap& eventData)
{
    /// Resources
    UI* ui_ = GetSubsystem<UI>();

    /// Get Datas
    int  Status=eventData[LoadState::P_CMD].GetInt();
    String Arguments=eventData[LoadState::P_ARG].GetString();

    /// Get UI root
    UIElement * progressRootUI = ui_ -> GetRoot();

    cout << "Progress OnStateChangeCalled " << Status << " Data " << Arguments.CString() << endl;

    /// If bar exist
/// Text * progressText = (Text *)progressRootUI->GetChild("progressText",true);

    Text * progressText = (Text *)progressRootUI->GetChild("progressText",true);

    /// Just update the status bar with data
    if(Status==0)
    {

        /// If the Progress Text exist
        if(progressText)
        {
            /// Set Text
            cout << "Set Text" << endl;
            progressText -> SetText(Arguments);
        }
    }

    /// Loading completed call Load Success
    if(Status==1)
    {
        /// go to Handler Progress Load
        HandlerProgressLoadSuccess(eventType, eventData);

        return;
    }

    /// Status there was a problem doing loading
    if(Status==9999)
    {
        /// Create Popup
        PopupWindowConfirm("Progress","Progress Alert","Time out loading");

        /// Get UI
        UI* ui_ = GetSubsystem<UI>();

        /// Get the root
        UIElement * progressUIRoot_ = ui_-> GetRoot();

        Button * continueButton = (Button *) progressUIRoot_ -> GetChild("ProgressAlertContinueButton",true);

        /// Subscribe
        SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClientStateProgress,HandlerProgressLoadFailed));
    }

    return;
}

/// Broadcast a event
void ExistenceClientStateProgress::ProgressSendEvent(int commandstatus, String message)
{
    /// Set Variant Map
    VariantMap eventData;

    /// Send Load Event
    eventData[LoadState::P_CMD] = commandstatus;
    eventData[LoadState::P_ARG] = message;
    eventData[LoadState::P_OBJ] = this;

    /// Send Event text
    cout << "Send Event " << commandstatus << " message " << message.CString() << endl;

    /// Send event
    SendEvent(P_LOAD_CHANGE, eventData);

    return;
}

/// Handle updates check for a ssucessfule load
void ExistenceClientStateProgress::ProgessionHandleUpdate(StringHash eventType, VariantMap& eventData)
{
    /// Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    /// Check scene loading state
    if(progressloadingstate == PROGRESSSCENELOAD)
    {

        /// Check async load of a scene
        if(Existence -> scene_-> GetAsyncProgress()>=1)
        {
            /// set progress loadign state
            progressloadingstate = PROGRESSCHARACTERLOAD;

            Existence -> scene_ ->StopAsyncLoading ();

            /// Add Lifetimes
            AddLife();

            /// Create character
            CreateCharacter();

            /// Everything loaded turn off async and send status completed
            ProgressSendEvent(1,String("Completed"));
            /// Send Load Event
        }
    }

    /// If timer exceeds 6000 milliseconds
    if(ProgressTimer.GetMSec(false)>60000&&progressload_!=PROGRESSTIMEOUT)
    {

        ///  Send Time out
        ProgressSendEvent(9999,String("Timeout occurred ... "));
        /// Send Load Event

        progressload_ = PROGRESSTIMEOUT;
    }

    /// If timer exceeds 9000 milliseconds
    if(ProgressTimer.GetMSec(false)>240000)
    {
        ///  Handle Post Updates
        UnsubscribeFromAllEvents();

        /// code maybe to create a status to exist out here

    }
    return;
}
/// Create progress screen UI
void ExistenceClientStateProgress::PopupWindowConfirm(const String &WindowName, const String &Title, const String &Message)
{

    /// Get Needed SubSystems
    Graphics* graphics_ = GetSubsystem<Graphics>();
    UI* ui_ = GetSubsystem<UI>();

    /// Get root
    UIElement * progressuiRoot_ = ui_-> GetRoot();

    /// Get rendering window size as floats
    float width = (float)graphics_->GetWidth();
    float height = (float)graphics_->GetHeight();

    /// Create the Window and add it to the UI's root node
    progressWindow_= new Window(context_);

    UIElement* titleBar = new UIElement(context_);
    Text* windowTitle = new Text(context_);
    Text* progressText=new Text(context_);
    Button* continueButton = new Button(context_);

    /// Set Window size and layout settings
    progressWindow_->SetMinSize(384, 248);
    progressWindow_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    progressWindow_->SetAlignment(HA_CENTER, VA_CENTER);
    progressWindow_->SetName(WindowName+"AlertWindow");
    progressWindow_->SetMovable(true);
    progressWindow_->SetOpacity(1);

    /// Create Window 'titlebar' container
    titleBar->SetMinSize(0,32);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    windowTitle->SetName(WindowName+"AlertWindowTitle");
    windowTitle->SetText(Title);

    progressText->SetName(WindowName+"AlertWindowMessage");
    progressText->SetText(Message);

    /// addlones
    continueButton->SetName(WindowName+String("AlertContinueButton"));
    continueButton->SetPosition(300,212);
    continueButton -> SetHorizontalAlignment(HA_RIGHT);

    /// Add the controls to the title bar
    titleBar->AddChild(windowTitle);
    progressWindow_->AddChild(titleBar);
    progressWindow_->AddChild(progressText);
    progressWindow_->AddChild(continueButton);

    progressuiRoot_ -> AddChild(progressWindow_);

    /// Apply styles
    progressWindow_->SetStyleAuto();
    progressText->SetStyleAuto();
    windowTitle->SetStyleAuto();
    continueButton->SetStyle("continueButton");

    return;
}

void ExistenceClientStateProgress::HandlerProgressLoadSuccess(StringHash eventType, VariantMap& eventData)
{
    /// Unsubscribe
    UnsubscribeFromAllEvents();

    /// Clear
    UI* ui_ = GetSubsystem<UI>();

    /// Clear UI
    ui_ -> Clear();
    progressScene_->Clear();

    /// Create a event
    VariantMap gamestatechange;
    gamestatechange[GameState::P_CMD] = GAME_STATE_GAMEMODE;

    cout << "Debug: Attempt to send a state change" << endl;

    SendEvent(G_STATES_CHANGE,gamestatechange);


    return;
}

void ExistenceClientStateProgress::HandlerProgressLoadFailed(StringHash eventType, VariantMap& eventData)
{

    /// Unsubscribe
    UnsubscribeFromAllEvents();

    /// need to go to function
    /// Exit
    Existence -> Exit();

    return;
}

/// Add lifetime to nodes  if needed
void ExistenceClientStateProgress::AddLife(void)
{
    /// Send event loading dummy scene automatic success
    ProgressSendEvent(0, String("Adding game life to null life ... "));

    /// Loop through the whole scene and get the root Node
    Node * RootNode = Existence->scene_ -> GetParent();

    /// Get node list
    PODVector <Node *> NodesVector;
    Existence->scene_ -> GetChildren (NodesVector, true);

    /// Set necessary objects
    Node * OrphanNode;
    String Nodename;

    ProgressSendEvent(0, String("Adding lifetime to nodes ... "));

    /// loop nodes
    for(int i=0; i < NodesVector.Size(); i++)
    {
        /// Do nothing like copy the node vector to a node
        OrphanNode = NodesVector[i];

        /// Prevent double game objects
        /// Add a component
        if (!OrphanNode -> HasComponent<GameObject>())
        {
            GameObject * OrphanNodeGameObject = OrphanNode-> CreateComponent<GameObject>();

            OrphanNodeGameObject -> SetLifetime(-1);
        }

    }
}


void ExistenceClientStateProgress::AlternativeSendEvent(int event)
{
    ProgressSendEvent(event,"");

    return;
}
