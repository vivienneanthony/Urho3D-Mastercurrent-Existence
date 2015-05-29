#ifndef GameStateComponent_H
#define GameStateComponent_H


#include "../../../Urho3D/Resource/Image.h"
#include "../../../Urho3D/Procedural/Procedural.h"
#include "../../../Urho3D/Procedural/Rules.h"
#include "../../../Urho3D/Procedural/RandomNumberGenerator.h"

#include <vector>


using namespace std;
using namespace Urho3D;

class GameStateComponent : public LogicComponent
{
    /// Define subclass
    OBJECT(GameStateComponent)

public:
    GameStateComponent(Urho3D::Context* context);
    virtual ~GameStateComponent();
    static void RegisterNewSubsystem(Urho3D::Context* context);
    /// enter
    virtual void Enter();
    /// exit
    virtual void Exit();
    /// update event handler
protected:
private:
};

#endif // EnvironmentBuild_H
