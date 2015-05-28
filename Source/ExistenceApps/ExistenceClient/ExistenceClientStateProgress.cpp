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

#include "GameStateHandler.h"
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

/// Constructor Destror
ExistenceClientStateProgress::ExistenceClientStateProgress(Context* context)
{
    /// create UI
    ProgressScreenUI();
}

ExistenceClientStateProgress::~ExistenceClientStateProgress:()
{
    //dtor
}



// Log UI Code
/// Create progress screen UI
void ExistenceClientStateProgress::ProgressScreenUI(void)
{

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    ui->Clear();

    /// set ui state to none
    ExistenceGameState.SetUIState(UI_PROGRESSINTERFACE);

    /// Get rendering window size as floats
    float width = (float)graphics->GetWidth();
    float height = (float)graphics->GetHeight();

    /// Create the Window and add it to the UI's root node
    window_= new Window(context_);

    uiRoot_->AddChild(window_);
    UIElement* titleBar = new UIElement(context_);
    Text* windowTitle = new Text(context_);
    Text* progressText=new Text(context_);
    Button* continueButton = new Button(context_);

    /// Set Window size and layout settings
    window_->SetMinSize(384, 192);
    window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window_->SetAlignment(HA_CENTER, VA_CENTER);
    window_->SetName("LoginWindow");
    window_->SetMovable(false);
    window_->SetOpacity(.6);

    /// Create Window 'titlebar' container
    titleBar->SetMinSize(0,32);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    windowTitle->SetName("Login");
    windowTitle->SetText("Accessing Server");

    progressText->SetName("progressText");
    progressText->SetText("Loading ghost characters states... ");
    progressText->SetStyleAuto();

    /// addlones
    continueButton->SetName("Login");
    continueButton->SetStyle("continueButton");

    /// Add the controls to the title bar
    titleBar->AddChild(windowTitle);
    window_->AddChild(titleBar);
    window_->AddChild(progressText);
    window_->AddChild(continueButton);

    /// Apply styles
    window_->SetStyleAuto();
    windowTitle->SetStyleAuto();

    SubscribeToEvent(continueButton, E_RELEASED, HANDLER(ExistenceClient, ProgressScreenUIHandleClosePressed));

    return;
}

// Progress screen UI function handler
void ExistenceClientStateProgress::ProgressScreenUIHandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    /// set ui state to none
    ExistenceGameState.SetUIState(UI_PROGRESSINTERFACE);

    mainScreenUI();

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

    Node* objectNode = scene_->GetChild("Character");

    /// Register Character component
    character_ = objectNode->CreateComponent<Character>();

    LoadPlayer(TemporaryAccountPlayerSelected);

    /// Copy character information
    //character_ -> SetAlliance(TemporaryAccountPlayerList[TemporaryAccountPlayerSelected].GetAlliance());
    //character_ -> SetCharacteristics(TemporaryAccountPlayerList[TemporaryAccountPlayerSelected].GetCharacteristics());
    //character_ -> SetPlayerInfo(TemporaryAccountPlayerList[TemporaryAccountPlayerSelected].GetPlayerInfo());

    //character_ -> SetHealth(100);

    /// Load a Character Mesh
    LoadCharacterMesh(CHARACTERMAINSCENE, "Character",character_->GetAlliance().alienrace,character_->GetCharacteristics().gender);

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

    /// Gets the child position
    Node* headNode = objectNode->GetChild("head", true);

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    Node * cameraNode_ = headNode ->CreateChild("CameraFirstPerson");

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f,0.0f,0.15f));
    cameraNode_->SetRotation(Quaternion(0.0,0.0,0.0));

    /// Set first person camera Node
    Camera* cameraObject = cameraNode_->CreateComponent<Camera>();
    cameraObject->SetOrthographic(0);
    cameraObject->SetZoom(1);
    cameraObject->SetNearClip(0.0f);
    cameraObject->SetUseClipping(false);

    Node * crossboxNode = objectNode ->CreateChild("CrossBox");

    /// Set an initial position for the camera scene node above the plane
    crossboxNode->SetPosition(Vector3(0.0f,staticmodelboxcenter.y_+.75f,1.0f));
    crossboxNode->SetRotation(Quaternion(0.0,0.0,0.0));

    /// Get static model and bounding box, calculate offset
    StaticModel * crossboxModel = crossboxNode->CreateComponent<StaticModel>();

    crossboxModel ->SetModel(cache->GetResource<Model>("Resources/Models/CrossBox.mdl"));
    crossboxModel ->ApplyMaterialList("Resources/Models/CrossBox.txt");

    /// Set camera to first person
    ExistenceGameState.SetCameraMode(CAMERAMODE_FIRSTPERSON);

    /// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    /// at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    /// use, but now we just use full screen and default render path configured	SetOrthographic ( in the engine command line options
    /// viewport -> SetCamera(cameraObject);
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraObject));
    renderer->SetViewport(0, viewport);

    effectRenderPath = viewport->GetRenderPath() -> Clone();
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/Bloom.xml"));
    effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));

    /// Make the bloom mixing parameter more pronounced
    effectRenderPath->SetShaderParameter("BloomMix", Vector2(0.9f, 0.6f));
    effectRenderPath->SetEnabled("Bloom", false);
    effectRenderPath->SetEnabled("FXAA3", false);
    viewport->SetRenderPath(effectRenderPath);

    character_->controls_.pitch_ = cameraNode_->GetRotation().PitchAngle();
    character_->controls_.yaw_ = cameraNode_->GetRotation().YawAngle();

    return;
}

// code to handle actual commans

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
    scene_-> CreateComponent<Octree>();
    scene_-> CreateComponent<PhysicsWorld>();
    scene_-> CreateComponent<DebugRenderer>();

    /// clear api
    manager_->SetScene(scene_);
    environmentbuild_->SetScene(scene_);

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
    Node* skyNode = scene_->CreateChild("Sky");

    Skybox* skybox = skyNode->CreateComponent<Skybox>();
    skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox.xml"));

    skyNode->SetScale(500.0f); /// The scale actually does not matter
    skyNode->SetName("GeneratedSkybox_Skybox1");


    /// Create a directional light to the world. Enable cascaded shadows on it
    Node* lightNode = scene_->CreateChild("DirectionalLight1");
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
    Node* lightNode2 = scene_->CreateChild("DirectionalLight2");
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
    Node* lightNode3 = scene_->CreateChild("DirectionalLight3");
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
    Node* terrainNode = scene_->CreateChild("Terrain");
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

    blend -> SavePNG("testing2.png");

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
    Node * characternode_ = scene_->CreateChild("Character");
    characternode_->SetPosition(Vector3(0.0f, position.y_ , 0.0f));

    /// Get the materials
    Material * skyboxMaterial = skybox->GetMaterial();

    /// Change environment
    GenerateSceneUpdateEnvironment(terrainrule);

    /// Add objects functions
    GenerateSceneBuildWorld(terrainrule);

    /// Load differiental
    if(differentialfilename)
    {
        GenerateSceneLoadDifferential(differentialfilename);

    }

    return;
}



/// Build world
int ExistenceClientStateProgress::GenerateSceneBuildWorld(terrain_rule terrainrule)
{
    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    EnvironmentBuild * environmentbuild_ = GetSubsystem<EnvironmentBuild>();

    /// Build world
    ///Node * WorldObjectNode = scene_-> CreateChild("EnvironmentBuildNode");
    ///EnvironmentBuild * EnvironmentBuildObjects = WorldObjectNode  -> CreateComponent<EnvironmentBuild>();

    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    /// Initialize
    environmentbuild_-> Initialize();

    environmentbuild_ -> GenerateWorldObjects(0, terrainrule);


    return 1;
}

/// Change environment
int ExistenceClientStateProgress::GenerateSceneUpdateEnvironment(terrain_rule terrainrule)
{
    /// Define Resouces
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Get skybox. The Skybox component is used like StaticModel, but it will be always located at the camera, giving the
    /// illusion of the box planes being far away. Use just the ordinary Box model and a suitable material, whose shader will
    /// generate the necessary 3D texture coordinates for cube mapping
    Node* skyNode = scene_->GetChild("GeneratedSkybox_Skybox1",true);
    Skybox* skybox = skyNode->GetComponent<Skybox>();

    /*    /// Get a Zone component for ambient lighting & fog control
        Node* zoneNode = scene_->GetChild("GeneratedZone_Zone1",true);
        Zone* zone = zoneNode->GetComponent<Zone>();*/

    /// Get a directional light to the world. Enable cascaded shadows on it
    Node* lightNode1 = scene_->GetChild("GeneratedLight_Light1",true);
    Light* light1 = lightNode1->GetComponent<Light>();

    /// Get a directional light to the world. Enable cascaded shadows on it
    Node* lightNode2 = scene_->GetChild("GeneratedLight_Light2",true);
    Light* light2 = lightNode2->GetComponent<Light>();

    /// Get a directional light to the world. Enable cascaded shadows on it
    Node* lightNode3 = scene_->GetChild("GeneratedLight_Light3",true);
    Light* light3 = lightNode3->GetComponent<Light>();

    /// Generate Terrain
    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);
    Terrain* terrain = terrainNode->GetComponent<Terrain>();

    /// Change texture
    switch (terrainrule.worldtype)
    {
    case WORLD_DESERT:
        /// Set light and skybox material
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox_Desert.xml"));
        LoadEnvironmentSettings("Skybox_Desert.xml");
        break;
    case WORLD_ICE:
        skybox->SetMaterial(cache->GetResource<Material>("Materials/Skybox_Ice.xml"));
        terrain->SetMaterial(cache->GetResource<Material>("Materials/TerrainTriPlanar-Ice.xml"));
        break;
    default:
        break;
    }

    return 1;
}



/// Load a dummy scene
void ExistenceClientStateProgress::loadDummyScene(void)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    scene_-> CreateComponent<Octree>();
    scene_-> CreateComponent<DebugRenderer>();

    /// Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
    /// show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
    /// is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
    /// optimizing manner

    /// Create a child scene node (at world origin) and a StaticModel component into it. Set the StaticModel to show a simple
    /// plane mesh with a "stone" material. Note that naming the scene nodes is optional. Scale the scene node larger
    /// (100 x 100 world units)
    Node* planeNode = scene_->CreateChild("Plane");
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
    Node* lightNode = scene_->CreateChild("DirectionalLight");
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
        Node* mushroomNode = scene_->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(0.5f + Random(2.0f));
        StaticModel* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
        mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
    }

    Node* characterNode = scene_->CreateChild("Character");
    characterNode->SetName("Character");

    /// Create a scene node for the camera, which we will move around
    /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();

    /// Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}






/// Load a scene
void ExistenceClientStateProgress::loadScene(const int mode, const char * lineinput)
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
    manager_->SetScene(scene_);
    environmentbuild_->SetScene(scene_);

    Input* input = GetSubsystem<Input>();

    /// string string leaving something comparable
    string argumentsstring = lineinput;
    string argument[80];

    /// create a idx
    int idx = 0;

    /// transfer to lowercase
    std::transform(argumentsstring.begin(), argumentsstring.end(), argumentsstring.begin(), ::tolower);

    /// copy string to stream
    stringstream ssin(argumentsstring);

    /// loop through arguments
    while (ssin.good() && idx < 40)
    {
        ssin >> argument[idx];
        ++idx;
    }

    int testgenerate=1;
    int padded=0;

    GetSubsystem<Input>()->SetMouseVisible(false);

    unsigned long long int creationtime = 0; /// Default

    /// Run trhrough arugments - first check if GENERATE
    if(argument[1]=="generate")
    {

        if(argument[2]=="random")
        {
            /// get current time
            creationtime = 0;
        }
        else if(argument[2]=="reset")
        {
            /// get current time
            time_t tempseed = time(NULL);

            creationtime = static_cast<unsigned long int> (tempseed);

            GetSubsystem<Input>()->SetMouseVisible(true);

            return;
        }
        else if(argument[2] =="datetime")
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
        else if(argument[2]=="seed")
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
    }
/// Run trhrough arugments - first check if FILE
    else if (argument[1]=="file")
    {
        /// Create variables (urho3d)
        String InputDataFile;

        InputDataFile.Append(filesystem->GetProgramDir().CString());
        InputDataFile.Append("Resources/Scenes/");
        InputDataFile.Append(argument[2].c_str());

        /// Check if the input data file exist
        if(filesystem->FileExists(InputDataFile))
        {
            /// Open file as a Urho3d Datafile
            File dataFile(context_, InputDataFile, FILE_READ);

            if (dataFile.IsOpen())
            {

                /// Get File Extension
                String extension = GetExtension(InputDataFile);

                /// Load File based on Extension

                if (extension != ".xml")
                {
                    scene_ -> Load(dataFile);
                }
                else
                {
                    scene_ ->LoadXML(dataFile);
                }

            }

        }
        else
        {
            /// Load dummy scene
            loadDummyScene();
        }

    }
    else
    {
        /// load dummy scene
        loadDummyScene();
    }



/// Get the Camera Node and setup the viewport
    if((cameraNode_ = scene_->GetChild("Camera")))
    {

        /*SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
        renderer->SetViewport(0, viewport);*/
    }
    else
    {
        /// Create a scene node for the camera, which we will move around
        /// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
        cameraNode_ = new Node(context_);

        cameraNode_ = scene_->CreateChild("Camera");
        cameraNode_->CreateComponent<Camera>();

        Camera* camera = cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(1000.0f);

        ///Set an initial position for the camera scene node above the ground
        cameraNode_->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

    }

/// Setup viewport
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

/// Loop through the whole scene and get the root Node
    Node * RootNode = scene_ -> GetParent();

/// Get node list
    PODVector <Node *> NodesVector;
    scene_ -> GetChildren (NodesVector, true);

/// Set necessary objects
    Node * OrphanNode;
    String Nodename;

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

/// Create a character
/// Copy character information to player
    CreateCharacter();

/// Load main UI area
    loadSceneUI();

    /// rest of UI
    loadHUDFile("Resources/UI/MainTopBarWindow.xml",0,0);
    loadHUDFile("Resources/UI/PlayerInfoWindow.xml",0,34);

    /// load hud
    loadUIXML(UIQUICKMENU,0,0,0);

    /// Get player info  name from temporary list and put it into the character object
    Text* PlayerNameText = (Text*)ui->GetRoot()->GetChild("PlayerNameText", true);

    /// Get player level and level text
    unsigned int level=floor(character_->GetLevels().level/10);

    string levelstring=ConvertUIntToString(level);

    string levelstext=levels[level];

    /// Set hud sting to level and character name
    string playername=character_->GetPlayerInfo().lastname+" "+character_->GetPlayerInfo().firstname+" ("+levelstring+") "+ levelstext;


    PlayerNameText -> SetText(playername.c_str());

    UpdatePlayerInfoBar();

/// use UI cursor
/// Enable OS cursor
    ui->GetCursor()->SetVisible(true);

    if(ui->GetCursor()->IsVisible())
    {
        Print ("Cursor Exist");

        Print(ui->GetCursor()->GetAppliedStyle());
    }
    else
    {
        Print ("Cursor Does Not Exist");
    }

    return;
}


// Load a scene
void ExistenceClientStateProgress::loadSceneCreationCreation( const char * lineinput)
{
    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();


    /// Create variables (urho3d)
    String InputDataFile;

    InputDataFile.Append(filesystem->GetProgramDir().CString());
    InputDataFile.Append("Resources/Scenes/");
    InputDataFile.Append(lineinput);

    /// Check if the input data file exist
    if(filesystem->FileExists(InputDataFile))
    {
        /// Open file as a Urho3d Datafile
        File dataFile(context_, InputDataFile, FILE_READ);

        if (dataFile.IsOpen())
        {

            /// Get File Extension
            String extension = GetExtension(InputDataFile);

            /// Load File based on Extension

            if (extension != ".xml")
            {
                scene_ -> Load(dataFile);
            }
            else
            {
                scene_ ->LoadXML(dataFile);
            }

        }

    }
    else
    {
        /// Load dummy scene
        loadDummyScene();
    }


    /// Get the Camera Node and setup the viewport
    Node * cameraNode_ = scene_->GetChild("Camera");

    /// Change viewport to camera node
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);

    return;
}


