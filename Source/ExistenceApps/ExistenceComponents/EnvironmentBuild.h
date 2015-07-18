#ifndef EnvironmentBuild_H
#define EnvironmentBuild_H


#include "../../../Urho3D/Resource/Image.h"
#include "../../../Urho3D/Procedural/Procedural.h"
#include "../../../Urho3D/Procedural/Rules.h"
#include "../../../Urho3D/Procedural/RandomNumberGenerator.h"

#include <vector>


namespace Urho3D
{
class Geometry;
class Drawable;
class Light;
class Material;
class Pass;
class Technique;
class Octree;
class Graphics;
class RenderPath;
class RenderSurface;
class ResourceCache;
class Skeleton;
class OcclusionBuffer;
class Texture2D;
class TextureCube;
class View;
class Zone;
class Scene;
}

using namespace Urho3D;
using namespace std;


/// Define objects
#define WorldOjectCollisionMapLimit  1000

/// Define building blocks
#define EnvironmentBuild_ROCKEDGE              0
#define EnvironmentBuild_ROCKSSMALL            1
#define EnvironmentBuild_ROCKSXLARGE           4
#define EnvironmentBuild_TREES                 50
#define EnvironmentBuild_ICESPIKES             100

/// Temporary structure
struct WorldOjectCollisionMap
{
    float size_x;
    float size_y;
    float size_z;
    float origin_x;
    float origin_y;
    float origin_z;
    int lod;
};

class EnvironmentBuild : public LogicComponent
{
    /// Define subclass
    OBJECT(EnvironmentBuild)

public:

    /// Construct.
    EnvironmentBuild(Context* context);
    virtual ~EnvironmentBuild();

    /// Register object factory and attributes.
    static void RegisterNewSubsystem(Context* context);
    void SetScene(Scene *scene);

    int Initialize(void);

    /// public will remove
    int CreateObjectsAlongPath(Terrain * terrain, int objecttypes, float worldsize_x, float worldsize_y, float x, float z, float numberofobjects, float length, Image * terrainHeightMap);
    int GenerateWorldObjects(const time_t &timeseed,  terrain_rule terrainrule);
    int GenerateGrass2(Terrain * terrain, float worldsize_x, float worldsize_y, Image * terrainHeightMap);
    Vector3 NormalizedToWorld(Image *height, Terrain *terrain, Vector2 normalized);
    Vector2 NormalizedToWorld2D(Image *height, Terrain *terrain, Vector2 normalized);
    bool SetTextureMap(Image * InputBlendMap);

protected:

private:
    float ComputeDistance(int x1, int y1, int x2, int y2);

    /// Saved Collision Objects to 0
    int SaveCollisionObjects;

    /// Set world limit of objects to test for collision
    vector <WorldOjectCollisionMap> CollisionBounds;;
    terrain_rule terrainrules;
    RandomNumberGenerator RandomRule;

    /// Scene.
    Scene * scene_;

    Image * blendMap;


};

#endif // EnvironmentBuild_H
