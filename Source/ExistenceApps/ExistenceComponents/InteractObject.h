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

#ifndef INTERACTOBJECT_H_INCLUDED
#define INTERACTOBJECT_H_INCLUDED

#include "../../../Urho3D/Scene/LogicComponent.h"
#include "../../../Urho3D/Core/Context.h"
#include "../../../Urho3D/Resource/ResourceCache.h"
#include "../../../Urho3D/Scene/Node.h"

#include "InteractEvent.h"


using namespace std;
using namespace Urho3D;

/// Character component, responsible for physical movement according to controls, as well as animation.
class InteractObject : public LogicComponent
{
    OBJECT(InteractObject);
public:
    /// Construct.
    InteractObject(Context* context);

    /// Register object factory and attributes.
    static void RegisterObject(Context* context);

    /// Handle startup. Called by LogicComponent base class.
    virtual void Start();
    /// Handle update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);


private:


};

#endif // InteractOBJECT_H_INCLUDED
