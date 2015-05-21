/// Urho Related Header Files
#include  <Urho3D/Urho3D.h>

#include "../../../Urho3D/Core/CoreEvents.h"
#include "../../../Urho3D/Scene/Serializable.h"
#include "../../../Urho3D/Core/Context.h"
#include "../../../Urho3D/Core/Object.h"
#include "../../../Urho3D/Engine/Engine.h"
#include "../../../Urho3D/UI/Font.h"
#include "../../../Urho3D/Input/Input.h"
#include "../../../Urho3D/Core/ProcessUtils.h"
#include "../../../Urho3D/UI/Text.h"
#include "../../../Urho3D/UI/UI.h"
#include "../../../Urho3D/Scene/Scene.h"
#include "../../../Urho3D/Scene/Component.h"
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
#include "../../../Urho3D/Graphics/Graphics.h"
#include "../../../Urho3D/Container/Str.h"
#include "../../../Urho3D/Scene/Node.h"
#include "../../../Urho3D/Resource/XMLElement.h"
#include "../../../Urho3D/DebugNew.h"
#include "../../../Urho3D/Procedural/RandomNumberGenerator.h"


///C/C++ related header files
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

/// Existence Header files
#include "../Account.h"

/// Existence Specific
#include "Character.h"
#include "GameStateHandler.h"
#include "GameObject.h"
#include "EnvironmentBuild.h"
#include "Manager.h"


using namespace std;


Manager::Manager(Context* context) :
    LogicComponent(context)
{
    scene_ = NULL;
}

Manager::~Manager()
{
    //dtor
}

/// Register Subsystem
void Manager::RegisterNewSubsystem(Context* context)
{
    context -> RegisterSubsystem(new Manager(context));


}

/// Set Scene
void Manager::SetScene(Scene *scene)
{
    /// point
    scene_ = scene;

    /// reserve
    ManagedNodes.Reserve(1024);
    ManagedGeneratedNodes.Reserve(1024);

    /// Clear Managed Nodes
    ManagedNodes.Clear();
    ManagedGeneratedNodes.Clear();
}

int Manager::AddGeneratedObject(Node * node)
{
    ManagedGeneratedNodes.Push(node);
}

/// Add Object
int Manager::AddObject(int type, const char * name, float x, float y, float z, const char *filename, unsigned int physics)
{
    /// Get Needed SubSystems
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    int result=false;

    /// Check if a scene exist
    if(!scene_)
    {
        return 0;
    }

    String AddObjectFilename;

    /// Switch type for filename
    switch (type)
    {
    case MANAGESTATICMODEL:
    {
        /// First Check if .mdl is in the filename
        if(!string(filename).find(".mdl"))
        {
            return 0;
        }

        /// Create Urho3D equivalent of the filenames
        AddObjectFilename.Append(filesystem->GetProgramDir().CString());

        AddObjectFilename.Append("Resources/Models/");
        AddObjectFilename.Append(filename);

        /// If the model does not exist
        if(!filesystem->FileExists(AddObjectFilename.CString()))
        {
            return 0;
        }
    }

    break;
    case MANAGELIGHT:
    {
        /// nothing for now
    }
    break;
    default:
        return 0;


    }

    /// Switch type
    switch (type)
    {
    case MANAGESTATICMODEL:
    {
        /// check if static model exist

        /// create node
        Node * AddObjectNode = scene_ -> CreateChild (String(name));

        /// Setup node in scene
        AddObjectNode->SetPosition(Vector3(x,y,z));
        AddObjectNode->SetRotation(Quaternion(0.0f,0.0f,0.0f));
        AddObjectNode->SetScale(Vector3::ONE);

        StaticModel* AddObjectNodeObject = AddObjectNode->CreateComponent<StaticModel>();

        /// Addd Static Model component
        AddObjectNodeObject->SetModel(cache->GetResource<Model>(AddObjectFilename));
        AddObjectNodeObject-> SetCastShadows(true);

        /// Create material
        String AddObjectFilenameMaterial;
        AddObjectFilenameMaterial = AddObjectFilename.Replaced (String(".mdl"), String(".txt"), true);

        AddObjectNodeObject->ApplyMaterialList( AddObjectFilenameMaterial.CString());

        ///AddObjectNodeObject->SetMaterial(cache->GetResource<Material>(AddObjectFilenameMaterial));
        /// Add Physics

        if(physics)
        {
            /// Create rigidbody, and set non-zero mass so that the body becomes dynamic
            RigidBody* AddObjectNodeRigid =  AddObjectNode->CreateComponent<RigidBody>();
            AddObjectNodeRigid->SetCollisionLayer(1);

            AddObjectNodeRigid->SetMass(0);

            /// Get static model and bounding box, calculate offset
            Model * staticmodel=cache->GetResource<Model>(AddObjectFilename);

            /// Get static model and bounding box, calculate offset
            BoundingBox  AddObjectNodeObjectBounding = staticmodel->GetBoundingBox();

            Vector3 BoundBoxCenter = AddObjectNodeObjectBounding.Center();

            /// Set zero angular factor so that physics doesn't turn the character on its own.
            /// Instead we will control the character yaw manually
            AddObjectNodeRigid->SetAngularFactor(Vector3::ONE);
            AddObjectNodeRigid->SetLinearFactor (Vector3::ONE);

            /// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
            AddObjectNodeRigid->SetCollisionEventMode(COLLISION_ALWAYS);

            /// Set a capsule shape for collision
            CollisionShape* AddObjectNodeCollisionShape = AddObjectNode->CreateComponent<CollisionShape>();

            /// Set shape collision
            switch (physics)
            {
            case COLLISION_BOX:
                AddObjectNodeCollisionShape->SetBox(Vector3::ZERO);
                break;
            case COLLISION_CONVEX:
                AddObjectNodeCollisionShape->SetConvexHull(staticmodel,1, Vector3::ONE,Vector3::ZERO);
                break;
            case COLLISION_TRIANGLE:
                AddObjectNodeCollisionShape->SetTriangleMesh(staticmodel,1, Vector3::ONE,Vector3::ZERO);
                break;
            default:
                AddObjectNodeCollisionShape->SetBox(Vector3::ZERO);
                break;
            }

            /// Position is not correct hmmm.
            AddObjectNodeCollisionShape->SetLodLevel(1);
            AddObjectNodeCollisionShape->SetSize (Vector3::ONE);
        }

        /// Setup node in scene
        AddObjectNode->SetPosition(Vector3(x,y,z));
        AddObjectNode->SetRotation(Quaternion(0.0f,0.0f,0.0f));
        AddObjectNode->SetScale(Vector3::ONE);

        /// Add a component
        GameObject * AddObjectNodeGameComponent = AddObjectNode -> CreateComponent<GameObject>();

        AddObjectNodeGameComponent->SetLifetime(-1.0f);

        /// push
        ManagedNodes.Push(AddObjectNode);

        result=true;
    }
    break;

    case MANAGELIGHT:
    {
        /// Create a directional light to the world. Enable cascaded shadows on it
        Node* AddObjectNode = scene_->CreateChild(name);
        Light* AddObjectNodeLight = AddObjectNode->CreateComponent<Light>();
        AddObjectNodeLight->SetLightType(LIGHT_DIRECTIONAL);
        AddObjectNodeLight->SetCastShadows(true);
        AddObjectNodeLight->SetSpecularIntensity(.3f);
        AddObjectNodeLight->SetBrightness(.6);
        AddObjectNodeLight->SetColor(Color(1.0f, 1.0f,.95f));

        /// Setup node in scene
        AddObjectNode->SetPosition(Vector3(x,y,z));
        AddObjectNode->SetRotation(Quaternion(0.0f,0.0f,0.0f));
        AddObjectNode->SetScale(1);

        /// Add a component
        GameObject * AddObjectNodeGameComponent = AddObjectNode -> CreateComponent<GameObject>();

        AddObjectNodeGameComponent->SetLifetime(-1.0f);


        /// Use filename to change type
        if(filename=="directional")
        {
            AddObjectNodeLight->SetLightType(LIGHT_DIRECTIONAL);
        }

        if(filename=="spot")
        {
            AddObjectNodeLight->SetLightType(LIGHT_SPOT);
        }

        if(filename=="point")
        {
            AddObjectNodeLight->SetLightType(LIGHT_POINT);
        }


        /// push
        ManagedNodes.Push(AddObjectNode);
    }
    break;

    case MANAGEZONE:
    {


/// Create a Zone component for ambient lighting & fog control
        Node* AddObjectNode= scene_->CreateChild(name);
        Zone* AddObjectNodeZone = AddObjectNode->CreateComponent<Zone>();
        Vector3 boundingBoxMin(-1000.0f,0,-1000.0f);
        Vector3 boundingBoxMax(1000.0f,2000.0f,1000.0f);
/// change bounding box to something more accurate
        AddObjectNodeZone->SetBoundingBox(BoundingBox(boundingBoxMin,boundingBoxMax));
        AddObjectNodeZone->SetAmbientColor(Color(0.01f, 0.01f, .01f));
        AddObjectNodeZone->SetFogColor(Color(1.0f, 1.0f, 1.0f));
        AddObjectNodeZone->SetFogStart(0.0f);
        AddObjectNodeZone->SetFogEnd(1000.0f);
        AddObjectNodeZone->SetHeightFog (false);
        AddObjectNode->SetName(name);

        /// Add a component
        GameObject * AddObjectNodeGameComponent = AddObjectNode -> CreateComponent<GameObject>();

        AddObjectNodeGameComponent->SetLifetime(-1.0f);

        /// push
        ManagedNodes.Push(AddObjectNode);
    }
    break;

    default:
        break;
    }

    return result;
}

int Manager::SaveManagedNodes(const char *filename)
{
    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 0;
    }

    /// Check if Node is empty
    if (ManagedNodes.Empty())
    {
        return 0;
    }

    /// Force .xml on save
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String savesceneexport;

    /// Set directory
    savesceneexport.Append(filesystem->GetProgramDir().CString());
    savesceneexport.Append("Resources/ScenesData/");
    savesceneexport.Append(filename);

    savesceneexport = savesceneexport.Replaced(String(".xml"),String("diff.xml"),true);

    File saveFile(context_, savesceneexport.CString(), FILE_WRITE);

    XMLFile * savesceneexportxml= new XMLFile(context_);

    XMLElement xmfileelement = savesceneexportxml-> CreateRoot("scene");

    /// Loop through components
    for(Vector<Node *>:: Iterator it = ManagedNodes.Begin(); it != ManagedNodes.End(); ++it)
    {
        XMLElement xmfileelementnode = xmfileelement.CreateChild("node");
        (*it) -> SaveXML(xmfileelementnode);
    }

    savesceneexportxml->Save(saveFile);

    return 1;
}


int Manager::SaveManagedGeneratedNodes(const char *filename)
{
    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 0;
    }

    /// Check if Node is empty
    if (ManagedGeneratedNodes.Empty())
    {
        return 0;
    }

    /// Force .xml on save
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String savescenegenerateexport;

    /// Set directory
    savescenegenerateexport.Append(filesystem->GetProgramDir().CString());

    savescenegenerateexport.Append("Resources/ScenesData/");

    savescenegenerateexport.Append(filename);

    savescenegenerateexport = savescenegenerateexport.Replaced(String(".xml"),String("gene.xml"),true);

    File saveFile(context_, savescenegenerateexport.CString(), FILE_WRITE);

    XMLFile * savescenegenerateexportxml= new XMLFile(context_);

    XMLElement xmfileelement = savescenegenerateexportxml-> CreateRoot("scene");

    /// Loop through components
    for(Vector<Node *>:: Iterator it = ManagedGeneratedNodes.Begin(); it != ManagedGeneratedNodes.End(); ++it)
    {
        XMLElement xmfileelementnode = xmfileelement.CreateChild("node");
        (*it) -> SaveXML(xmfileelementnode);
    }

    savescenegenerateexportxml->Save(saveFile);

    return 1;
}


/// Load account information from a account file
int Manager::LoadManagedNodes(const char *filename)
{

    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 1;
    }

    /// Force .xml on Load
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String Loadsceneexport;

    /// Set directory
    Loadsceneexport.Append(filesystem->GetProgramDir().CString());
    Loadsceneexport.Append("Resources/ScenesData/");
    Loadsceneexport.Append(filename);

    /// Check if the account file information exist
    if(!filesystem->FileExists(Loadsceneexport.CString()))
    {
        return 0;
    }

    XMLFile * loadingfile = new XMLFile(context_);

    /// Create a file in current context
    File LoadFile(context_, Loadsceneexport.CString(), FILE_READ);

    loadingfile->Load(LoadFile);

    XMLElement ElementRoot = loadingfile->GetRoot("scene");

    XMLElement nextElement = ElementRoot.GetChild("node");

    do
    {


        /// get node attributes
        XMLElement AttributesPosition;

        /// Is enabled
        AttributesPosition = nextElement.GetChild ("attribute");

        /// name
        AttributesPosition = AttributesPosition.GetNext ("attribute");

        /// position
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        Vector3 NodePosition= AttributesPosition .GetVector3("value");

        /// rotation
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        Quaternion NodeRotation =  AttributesPosition.GetQuaternion ("value");


        /// rotation
        AttributesPosition = AttributesPosition.GetNext ("attribute");
        float nodeScale = AttributesPosition.GetFloat("Scale");

        Node * newNode = scene_->InstantiateXML (nextElement,(const Vector3 &)Vector3(NodePosition),(const Quaternion &)Quaternion(NodeRotation),REPLICATED);


        /// push
        ManagedNodes.Push(newNode);

        nextElement=nextElement.GetNext();

    }
    while(nextElement.NotNull());

    return 1;
}

int Manager::SaveSceneRaw(const char *filename)
{
    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 0;
    }

    /// Force .xml on save
    if(!string(filename).find(".xml"))
    {
        return 0;
    }

    /// Create String
    String savescenegenerateexport;

    /// Set directory and generate file
    savescenegenerateexport.Append(filesystem->GetProgramDir().CString());
    savescenegenerateexport.Append("Resources/ScenesData/");
    savescenegenerateexport.Append(filename);
    savescenegenerateexport = savescenegenerateexport.Replaced(String(".xml"),String("raw.xml"),true);

    File saveFile(context_, savescenegenerateexport.CString(), FILE_WRITE);

    scene_ -> SaveXML (saveFile);

    return 1;
}
