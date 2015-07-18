/// Headers and etc


#include <Urho3D/Urho3D.h>


#include "../../../Urho3D/Core/CoreEvents.h"
#include "../../../Urho3D/Engine/Engine.h"
#include "../../../Urho3D/Core/ProcessUtils.h"
#include "../../../Urho3D/Graphics/Octree.h"
#include "../../../Urho3D/Graphics/Model.h"
#include "../../../Urho3D/Graphics/Material.h"
#include "../../../Urho3D/Resource/ResourceCache.h"
#include "../../../Urho3D/Graphics/Graphics.h"
#include "../../../Urho3D/Graphics/Texture2D.h"
#include "../../../Urho3D/Graphics/AnimationController.h"
#include "../../../Urho3D/Core/Context.h"
#include "../../../Urho3D/IO/MemoryBuffer.h"
#include "../../../Urho3D/Physics/PhysicsEvents.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"
#include "../../../Urho3D/Physics/RigidBody.h"
#include "../../../Urho3D/Scene/Scene.h"
#include "../../../Urho3D/Scene/SceneEvents.h"
#include "../../../Urho3D/Graphics/Renderer.h"
#include "../../../Urho3D/UI/UI.h"
#include "../../../Urho3D/Scene/Node.h"
#include "../../../Urho3D/Physics/CollisionShape.h"
#include "../../../Urho3D/Graphics/StaticModel.h"
#include "../../../Urho3D/Graphics/Terrain.h"
#include "../../../Urho3D/Graphics/BillboardSet.h"
#include "../../../Urho3D/Procedural/Procedural.h"
#include "../../../Urho3D/Procedural/ProceduralTerrain.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <utility>


#include "Character.h"
#include "../Player.h"


#include "EnvironmentBuild.h"


using namespace std;

#define NORTH 1
#define NORTHEAST 2
#define EAST 3
#define SOUTHEAST 4
#define SOUTH  5
#define SOUTHWEST 6
#define WEST 7
#define NORTHWEST 8

EnvironmentBuild::EnvironmentBuild(Context* context) :
    LogicComponent (context)
{
    //ctor
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);

    scene_ = NULL;
}

EnvironmentBuild::~EnvironmentBuild()
{
    //dtor
}


/// Register Subsystem
void EnvironmentBuild::RegisterNewSubsystem(Context* context)
{
    context -> RegisterSubsystem(new EnvironmentBuild(context));
}

/// Set Scene
void EnvironmentBuild::SetScene(Scene *scene)
{
    /// point
    scene_ = scene;

}

/// Initialize a new environment
int EnvironmentBuild::Initialize(void)
{
    /// Allocate memory
    CollisionBounds.clear();
    SaveCollisionObjects=0;

    /// Clear rules
    terrainrules.creationtime=0;
    terrainrules.sealevel=0;
    terrainrules.worldtype=0;
    terrainrules.subworldtype=0;

}

/// Computer distance
float  EnvironmentBuild::ComputeDistance(int x1, int y1, int x2, int y2)
{
    float  xrange= x1-x2;
    float  yrange= y1-y2;

    return sqrt((xrange*xrange)+(yrange*yrange));
}

bool EnvironmentBuild::SetTextureMap(Image * InputBlendMap)
{
    blendMap = InputBlendMap;
    return 1;
}


/// Move world building here
int EnvironmentBuild::GenerateWorldObjects(const time_t &timeseed,  terrain_rule terrainrule)
{
    /// Get resources
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Check if a scene exist
    if(!scene_)
    {
        return 0;
    }

    /// Get scene and terrain node
    Node* terrainNode = scene_->GetChild("GeneratedTerrainRule_TerrainRoot",true);

    Terrain * terrain = terrainNode -> GetComponent<Terrain>();

    /// Get terrain size
    Vector3 spacing=terrain->GetSpacing();
    int patchSize=terrain->GetPatchSize();
    IntVector2 numPatches= terrain -> GetNumPatches ();

    /// Get actual size
    Vector2 patchWorldSize=Vector2(spacing.x_*(float)(patchSize*numPatches.x_), spacing.z_*(float)(patchSize*numPatches.y_));

    patchWorldSize.x_=patchWorldSize.x_*2;
    patchWorldSize.y_=patchWorldSize.y_*2;

    bool success = false;
    bool hasComponent = false;

    /// Check component
    proceduralterrain proceduralterraindefaults;

    /// Check for the current node
    if(terrainNode)
    {
        ProceduralTerrain * proceduralComponent = terrainNode-> GetComponent<ProceduralTerrain>();

        /// Set hasComponent to true
        if(proceduralComponent)
        {
            /// Set has components
            hasComponent=true;
            /// Get component values
            proceduralterraindefaults = proceduralComponent -> Get();
        }
    }

    /// if a component exist use the terrain rule imation
    if(hasComponent)
    {
        terrainrules.sealevel=proceduralterraindefaults.sealevel;
        terrainrules.subworldtype=proceduralterraindefaults.subworldtype;
        terrainrules.worldtype=proceduralterraindefaults.worldtype;
        terrainrules.creationtime=proceduralterraindefaults.creationtime;
    }
    else
    {
        terrainrules.sealevel=terrainrule.sealevel;
        terrainrules.subworldtype=terrainrule.subworldtype;
        terrainrules.worldtype=terrainrule.worldtype;
        terrainrules.creationtime=terrainrule.creationtime;
    }

    /// Size to match
    patchWorldSize.x_=patchWorldSize.x_*2;
    patchWorldSize.y_=patchWorldSize.y_*2;

    /// Create a weightmap image
    Image * terrainHeightMap= new Image(context_);

    terrainHeightMap->SetSize(patchWorldSize.x_+1, patchWorldSize.y_+1,1,4);

    int Spotx=0;
    int Spotz=0;

    float randomSpotx=0.0f;
    float randomSpotz=0.0f;

    /// initialize rule on seed
    RandomRule.SetRandomSeed(terrainrules.creationtime+2);

    blendMap -> SavePNG("testing.png");

    //Debug
    //cout << "\r\nEnvironmentBuild Seed" << terrainrules.creationtime+2<<endl;
    switch (terrainrules.worldtype)
    {
    case WORLD_DESERT:
        // Plant rocks
        for(unsigned int i=0; i<600; i++)
        {

            /// Pick a random spotskx
            Spotx=RandomRule.RandRange(2048);
            Spotz=RandomRule.RandRange(2048);

            randomSpotx=(float)Spotx-1024.0f;
            randomSpotz=(float)Spotz-1024.0f;

            /// Create rocks on paths
            CreateObjectsAlongPath(terrain, EnvironmentBuild_ROCKSSMALL, patchWorldSize.x_, patchWorldSize.y_,randomSpotx,randomSpotz, 6, 100.0f, terrainHeightMap);
        }
        break;

        /// world type terrain
    case WORLD_TERRAIN:
        /// Generate grass
        GenerateGrass2(terrain, patchWorldSize.x_, patchWorldSize.y_,terrainHeightMap);

        // Plant rocks
        for(unsigned int i=0; i<600; i++)
        {

            /// Pick a random spotskx
            Spotx=RandomRule.RandRange(1024);
            Spotz=RandomRule.RandRange(1024);

            randomSpotx=(float)Spotx-512.0f;
            randomSpotz=(float)Spotz-512.0f;

            /// Create rocks on paths
            CreateObjectsAlongPath(terrain,EnvironmentBuild_TREES, patchWorldSize.x_, patchWorldSize.y_,randomSpotx,randomSpotz, 6, 100.0f, terrainHeightMap);
        }
        break;
    default:
        break;
    }



    return 0;

}

int EnvironmentBuild::CreateObjectsAlongPath(Terrain * terrain, int objecttypes, float worldsize_x, float worldsize_y, float x, float z, float numberofobjects, float length, Image * terrainHeightMap)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

/// Check if a scene exist
    if(!scene_)
    {
        return 0;
    }


    if(!terrain)
    {
        return 0;
    }



    /// Need variables
    float lengthlimitdistance= length;

    float objectsalongpath=numberofobjects;
    float objectsdistance=lengthlimitdistance/objectsalongpath;
    float objectincrement=1;

    float origin_x=x;
    float origin_z=z;

    float difference_z=0.0f;
    float difference_x=0.0f;

    float position_x=0.0f;
    float position_z=0.0f;

    float newposition_x=0.0f;
    float newposition_z=0.0f;
    float olddistance=0.0f;

    position_x=origin_x;
    position_z=origin_z;

    float worldsize_xlow=-(worldsize_x/2);
    float worldsize_ylow=-(worldsize_y/2);

    float worldsize_xhigh=worldsize_x/2;
    float worldsize_yhigh=worldsize_y/2;


    float steep;

    /// blend cliff
    Vector2 nworld;
    Vector3 worldvalue;
    Vector3 normalvalue;


    do
    {
        /// Pick a random directoin
        int direction=RandomRule.RandRange(8)+1;

        /// Select coordinate change based on random direction
        switch (direction)
        {
        case NORTH:
            difference_x=0;
            difference_z=1;
            break;
        case NORTHEAST:
            difference_x=1;
            difference_z=1;
            break;
        case EAST:
            difference_x=+1;
            difference_z=0;
            break;
        case SOUTHEAST:
            difference_x=1;
            difference_z=-1;
            break;
        case SOUTH:
            difference_x=0;
            difference_z=-1;
            break;
        case SOUTHWEST:
            difference_x=-1;
            difference_z=-1;
            break;
        case WEST:
            difference_x=-1;
            difference_z=0;
            break;
        case NORTHWEST:
            difference_x=-1;
            difference_z=1;
            break;
        }

        /// If distance less then current distance then while continue loop
        if(ComputeDistance(position_x+difference_x, origin_x, position_z+difference_z,origin_z)<olddistance)
        {
            continue;
        }
        else
        {
            /// Create a new position
            newposition_x=position_x+difference_x;
            newposition_z=position_z+difference_z;

            ///  Copy newposition to current positon
            position_x=newposition_x;
            position_z=newposition_z;

            /// Get distance
            olddistance=ComputeDistance(position_x, origin_x, position_z, origin_z);

            nworld=Vector2(newposition_x/(float)4097, newposition_z/(float)4097);
            worldvalue=NormalizedToWorld( terrainHeightMap,terrain,nworld);
            normalvalue=terrain->GetNormal(worldvalue);

            steep=1.0f-normalvalue.y_;

            if (steep>0.01f)
            {
                continue;
            }

            /// Try this method to use percentange
            if(olddistance/lengthlimitdistance>(objectsdistance*objectincrement)/lengthlimitdistance)
            {
                /// Set next pointer
                objectincrement++;

                /// Add a object node
                Node * ObjectStaticNode= scene_ -> CreateChild("RockNode");

                /// Create nodes
                StaticModel * ObjectStaticModelBase = ObjectStaticNode->CreateComponent<StaticModel>();
                StaticModel * ObjectStaticModelBaseLeaves = ObjectStaticNode->CreateComponent<StaticModel>();

                /// Remove the second component
                if(objecttypes==EnvironmentBuild_ROCKSSMALL)
                {
                    ObjectStaticModelBaseLeaves -> Remove();
                }


                unsigned int pick=0;

                switch (objecttypes)
                {
                    /// Add rocks
                case EnvironmentBuild_ROCKSSMALL:
                    if(terrainrules.worldtype==WORLD_DESERT)
                    {
                        /// Pick random
                        pick= RandomRule.RandRange(5)+1;
                        String filenameBaseModel = String("Resources/Models/AlienRockDesert");
                        filenameBaseModel.Append(String(pick));
                        filenameBaseModel.Append(".mdl");
                        String filenameBaseTexture = String("Resources/Models/AlienRockDesert1.txt");

                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>(filenameBaseModel));
                        ObjectStaticModelBase->ApplyMaterialList(filenameBaseTexture);
                    }
                    break;
                    /// Add Trees
                case EnvironmentBuild_TREES:
                {

                    /// Pick Random
                    pick= RandomRule.RandRange(3)+1;

                    switch (pick)
                    {

                    case 1:
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree1.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree1.txt");

                        ObjectStaticModelBaseLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree1Leaves.mdl"));
                        ObjectStaticModelBaseLeaves->ApplyMaterialList("Resources/Models/AlienTree1Leaves.txt");
                        break;

                    case 2:
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree2.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree2.txt");

                        ObjectStaticModelBaseLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree2Leaves.mdl"));
                        ObjectStaticModelBaseLeaves->ApplyMaterialList("Resources/Models/AlienTree2Leaves.txt");

                        break;
                    case 3:
                        ObjectStaticModelBase->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree3.mdl"));
                        ObjectStaticModelBase->ApplyMaterialList("Resources/Models/AlienTree3.txt");

                        ObjectStaticModelBaseLeaves->SetModel(cache->GetResource<Model>("Resources/Models/AlienTree3Leaves.mdl"));
                        ObjectStaticModelBaseLeaves->ApplyMaterialList("Resources/Models/AlienTree3Leaves.txt");
                        break;
                    default:
                        /// Do nothing
                        break;
                    }

                }
                break;
                default:
                    /// Do nothing
                    return 1;
                }

                /// Add name
                ObjectStaticNode->SetName("GeneratedAsset_Object");

                /// Create Nodes and COmponents
                ObjectStaticModelBase->SetCastShadows(true);

                if(objecttypes==EnvironmentBuild_TREES)
                {
                    ObjectStaticModelBaseLeaves->SetCastShadows(true);
                }

                /// Get dimensions
                BoundingBox  ObjectStaticModelBox = ObjectStaticModelBase ->GetBoundingBox();
                Vector3  ObjectStaticModelCenter= ObjectStaticModelBox.HalfSize();

                float ychange=0;

                /// if ROCKS and dessert
                if(terrainrules.worldtype==WORLD_DESERT&&objecttypes==EnvironmentBuild_ROCKSSMALL)
                {
                    ychange=0;
                }
                else
                {
                    ychange=ObjectStaticModelCenter.y_;
                }


                /// Select a possible position to place a Rock
                /// Select a possible position to place a Rock
                Vector3 selectPosition=Vector3(position_x,terrain->GetHeight(Vector3(position_x,0.0f,position_z))+ychange,position_z);
                //Vector3 selectPosition=Vector3(position_x,0.0f,position_z);


                /// Set Rock position
                ObjectStaticNode->SetPosition(selectPosition);
                ObjectStaticNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(Vector3(position_x,0.0f,position_z))));
                ObjectStaticNode->SetRotation(Quaternion(RandomRule.RandRange(360),Vector3(0.0f,1.0f,0.0f)));

                if(objecttypes==EnvironmentBuild_ROCKSSMALL)
                {
                    int scale = RandomRule.RandRange(50);
                    float scalefloat = (float)(scale+50)/100;

                    ObjectStaticNode->SetScale(scalefloat);
                }
            }
        }
    }
    while(olddistance<=lengthlimitdistance);


    return 1;
}


int EnvironmentBuild::GenerateGrass2(Terrain* terrain, float worldsize_x, float worldsize_y, Image * terrainHeightMap)
{

    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    /// Check if a scene exist
    if(!scene_)
    {
        return 0;
    }

    if(!terrain)
    {
        return 0;
    }
        /// Define random point variables
    float GrowthSpotx=0;
    float GrowthSpotz=0;
    int InitialSpotx=0;
    int InitialSpotz=0;

    int TerrainInitialSpotx=0;
    int TerrainInitialSpotz=0;
    float TerrainGrowthSpotx=0.0f;
    float TerrainGrowthSpotz=0.0f;

    float GowthRandomSpotx=0;
    float GowthRandomSpotz=0;
    int InitialrandomSpotx=0;
    int InitialrandomSpotz=0;

    int NumberOfPlantingsGrowth=0;
    int NumberOfPlantings=0;
    unsigned int InitialRange=0;
    unsigned int  SpreadRange=0;

    int j=0;
    bool failattempts;

    /// blend cliff
    Vector2 nworld;
    Vector3 worldvalue;
    Vector3 normalvalue;
    float heightvalue=0;

    /// Change parameters based on type
    switch (terrainrules.worldtype)
    {

    case WORLD_DESERT:
        NumberOfPlantingsGrowth=25;
        NumberOfPlantings=50;
        InitialRange=100;
        SpreadRange=100;
        break;
    case WORLD_TERRAIN:
        NumberOfPlantingsGrowth=500;
        NumberOfPlantings=1200;
        InitialRange=500;
        SpreadRange=32;
        break;
    default:
        break;
    }

    float steep=0.0f;

    cout << terrainrules.sealevel <<endl;

    /// create billboards
    for(unsigned int i=0; i<NumberOfPlantings; ++i)
    {

        /// Pick random values
        InitialSpotx=RandomRule.RandRange(InitialRange);
        InitialSpotz=RandomRule.RandRange(InitialRange);

        TerrainInitialSpotx=InitialSpotx+2048;
        TerrainInitialSpotz=InitialSpotz+2048;

        Node* GrassInitialNode = scene_->CreateChild("GrassBillboardSetNode");
        GrassInitialNode->SetPosition(Vector3(InitialrandomSpotx-2048,0.0f,InitialrandomSpotz-2048));
        GrassInitialNode->SetName("GeneratedAsset_GrassBillNode");

        BillboardSet* billboardObject = GrassInitialNode->CreateComponent<BillboardSet>();
        billboardObject->SetNumBillboards(NumberOfPlantingsGrowth);

        billboardObject->SetMaterial(cache->GetResource<Material>("Resources/Materials/Grass.xml"));
        billboardObject->SetSorted(true);
        billboardObject->SetCastShadows(true);

        /// reset
        failattempts=false;

        j=0;

        /// blend cliff
        nworld=Vector2(  TerrainInitialSpotx/(float)4097, TerrainInitialSpotz/(float)4097);
        worldvalue=NormalizedToWorld( terrainHeightMap,terrain,nworld);
        normalvalue=terrain->GetNormal(worldvalue);

        failattempts=false;

        if(failattempts==false)
        {
            for(unsigned int j=0; j<NumberOfPlantingsGrowth; ++j)
            {
                GrowthSpotx=RandomRule.RandRange(SpreadRange*100);
                GrowthSpotz=RandomRule.RandRange(SpreadRange*100);

                GowthRandomSpotx=((float)GrowthSpotx/100)-(SpreadRange/2);
                GowthRandomSpotz=((float)GrowthSpotz/100)-(SpreadRange/2);

                TerrainGrowthSpotx=TerrainInitialSpotx+GowthRandomSpotx;
                TerrainGrowthSpotz=TerrainInitialSpotz+GowthRandomSpotz;

                nworld=Vector2(  TerrainGrowthSpotx/(float)4097, TerrainGrowthSpotz/(float)4097);
                worldvalue=NormalizedToWorld( terrainHeightMap,terrain,nworld);
                normalvalue=terrain->GetNormal(worldvalue);

                steep=1.0f-normalvalue.y_;

                if (steep>0.05f)
                {
                    continue;
                }


                Vector3 normalvalue=terrain->GetNormal(worldvalue);
                Billboard* bb = billboardObject->GetBillboard(j);

                /// Select a possible position to place a plant
                nworld=Vector2(  TerrainInitialSpotx/(float)4097, TerrainInitialSpotz/(float)4097);
                Vector3 selectPosition=Vector3(TerrainGrowthSpotx,terrain->GetHeight(Vector3(TerrainGrowthSpotx-2048.0f,0.0f,TerrainGrowthSpotz-2048.0f)),TerrainGrowthSpotz);

                bb->position_ =selectPosition;
                bb->size_ = Vector2(Random(0.3f) + 0.1f, Random(0.3f) + 0.1f);

                bb->enabled_ = true;
            }
        }
    }

    return 1;
}


Vector3 EnvironmentBuild:: NormalizedToWorld(Image *height, Terrain *terrain, Vector2 normalized)
{
    if(!terrain || !height) return Vector2(0,0);
    Vector3 spacing=terrain->GetSpacing();
    int patchSize=terrain->GetPatchSize();
    IntVector2 numPatches=IntVector2((height->GetWidth()-1) / patchSize, (height->GetHeight()-1) / patchSize);
    Vector2 patchWorldSize=Vector2(spacing.x_*(float)(patchSize*numPatches.x_), spacing.z_*(float)(patchSize*numPatches.y_));
    Vector2 patchWorldOrigin=Vector2(-0.5f * patchWorldSize.x_, -0.5f * patchWorldSize.y_);
    return Vector3(patchWorldOrigin.x_+normalized.x_*patchWorldSize.x_, 0, patchWorldOrigin.y_+normalized.y_*patchWorldSize.y_);
}

