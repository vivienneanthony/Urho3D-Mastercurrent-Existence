//
// Copyright (c) 2008-2015 the Urho3D project.
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

#include "../Core/CoreEvents.h"
#include "../Engine/EngineEvents.h"
#include "../IO/File.h"
#include "../IO/Log.h"
#include "../LuaScript/LuaFile.h"
#include "../LuaScript/LuaFunction.h"
#include "../LuaScript/LuaScript.h"
#include "../LuaScript/LuaScriptEventInvoker.h"
#include "../LuaScript/LuaScriptInstance.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Profiler.h"
#include "../Resource/ResourceCache.h"
#include "../Scene/Scene.h"

extern "C"
{
#include <lualib.h>
}

#include <toluapp/tolua++.h>
#include "../LuaScript/ToluaUtils.h"

#include "../DebugNew.h"

extern int tolua_AudioLuaAPI_open(lua_State*);
extern int tolua_CoreLuaAPI_open(lua_State*);
extern int tolua_EngineLuaAPI_open(lua_State*);
extern int tolua_GraphicsLuaAPI_open(lua_State*);
extern int tolua_InputLuaAPI_open(lua_State*);
extern int tolua_IOLuaAPI_open(lua_State*);
extern int tolua_MathLuaAPI_open(lua_State*);
#ifdef URHO3D_NAVIGATION
extern int tolua_NavigationLuaAPI_open(lua_State*);
#endif
#ifdef URHO3D_NETWORK
extern int tolua_NetworkLuaAPI_open(lua_State*);
#endif
#ifdef URHO3D_PHYSICS
extern int tolua_PhysicsLuaAPI_open(lua_State*);
#endif
extern int tolua_ResourceLuaAPI_open(lua_State*);
extern int tolua_SceneLuaAPI_open(lua_State*);
extern int tolua_UILuaAPI_open(lua_State*);
#ifdef URHO3D_URHO2D
extern int tolua_Urho2DLuaAPI_open(lua_State*);
#endif
extern int tolua_LuaScriptLuaAPI_open(lua_State*);

namespace Urho3D
{

LuaScript::LuaScript(Context* context) :
    Object(context),
    luaState_(0),
    executeConsoleCommands_(false)
{
    RegisterLuaScriptLibrary(context_);

    luaState_ = luaL_newstate();
    if (!luaState_)
    {
        LOGERROR("Could not create Lua state");
        return;
    }

    SetContext(luaState_, context_);

    lua_atpanic(luaState_, &LuaScript::AtPanic);

    luaL_openlibs(luaState_);
    RegisterLoader();
    ReplacePrint();

    tolua_MathLuaAPI_open(luaState_);
    tolua_CoreLuaAPI_open(luaState_);
    tolua_IOLuaAPI_open(luaState_);
    tolua_ResourceLuaAPI_open(luaState_);
    tolua_SceneLuaAPI_open(luaState_);
    tolua_AudioLuaAPI_open(luaState_);
    tolua_EngineLuaAPI_open(luaState_);
    tolua_GraphicsLuaAPI_open(luaState_);
    tolua_InputLuaAPI_open(luaState_);

#ifdef URHO3D_NAVIGATION
    tolua_NavigationLuaAPI_open(luaState_);
#endif

#ifdef URHO3D_NETWORK
    tolua_NetworkLuaAPI_open(luaState_);
#endif

#ifdef URHO3D_PHYSICS
    tolua_PhysicsLuaAPI_open(luaState_);
#endif

    tolua_UILuaAPI_open(luaState_);

#ifdef URHO3D_URHO2D
    tolua_Urho2DLuaAPI_open(luaState_);
#endif

    tolua_LuaScriptLuaAPI_open(luaState_);

    eventInvoker_ = new LuaScriptEventInvoker(context_);

    coroutineUpdate_ = GetFunction("coroutine.update");

    // Subscribe to post update
    SubscribeToEvent(E_POSTUPDATE, HANDLER(LuaScript, HandlePostUpdate));

    // Subscribe to console commands
    SetExecuteConsoleCommands(true);
}

LuaScript::~LuaScript()
{
    functionPointerToFunctionMap_.Clear();
    functionNameToFunctionMap_.Clear();
    
    lua_State* luaState = luaState_;
    luaState_ = 0;

    SetContext(luaState_, 0);

    if (luaState)
        lua_close(luaState);
}

void LuaScript::AddEventHandler(const String& eventName, int functionIndex)
{
    LuaFunction* function = GetFunction(functionIndex);
    if (function)
        eventInvoker_->AddEventHandler(0, eventName, function);
}

void LuaScript::AddEventHandler(const String& eventName, const String& functionName)
{
    LuaFunction* function = GetFunction(functionName);
    if (function)
        eventInvoker_->AddEventHandler(0, eventName, function);
}

void LuaScript::AddEventHandler(Object* sender, const String& eventName, int functionIndex)
{
    if (!sender)
        return;

    LuaFunction* function = GetFunction(functionIndex);
    if (function)
        eventInvoker_->AddEventHandler(sender, eventName, function);
}

void LuaScript::AddEventHandler(Object* sender, const String& eventName, const String& functionName)
{
    if (!sender)
        return;

    LuaFunction* function = GetFunction(functionName);
    if (function)
        eventInvoker_->AddEventHandler(sender, eventName, function);
}

void LuaScript::RemoveEventHandler(const String& eventName)
{
    eventInvoker_->UnsubscribeFromEvent(eventName);
}

void LuaScript::RemoveEventHandler(Object* sender, const String& eventName)
{
    if (!sender)
        return;

    eventInvoker_->UnsubscribeFromEvent(sender, eventName);
}

void LuaScript::RemoveEventHandlers(Object* sender)
{
    if (!sender)
        return;

    eventInvoker_->UnsubscribeFromEvents(sender);
}

void LuaScript::RemoveAllEventHandlers()
{
    eventInvoker_->UnsubscribeFromAllEvents();
}

void LuaScript::RemoveEventHandlersExcept(const Vector<String>& exceptionNames)
{
    PODVector<StringHash> exceptionTypes(exceptionNames.Size());
    for (unsigned i = 0; i < exceptionTypes.Size(); ++i)
        exceptionTypes[i] = StringHash(exceptionNames[i]);

    eventInvoker_->UnsubscribeFromAllEventsExcept(exceptionTypes, true);
}

bool LuaScript::ExecuteFile(const String& fileName)
{
    PROFILE(ExecuteFile);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    LuaFile* luaFile = cache->GetResource<LuaFile>(fileName);
    return luaFile && luaFile->LoadAndExecute(luaState_);
}

bool LuaScript::ExecuteString(const String& string)
{
    PROFILE(ExecuteString);

    int top = lua_gettop(luaState_);

    if (luaL_dostring(luaState_, string.CString()) != 0)
    {
        const char* message = lua_tostring(luaState_, -1);
        LOGERROR("Execute Lua string failed: " + String(message));
        lua_settop(luaState_, top);
        return false;
    }

    return true;
}

bool LuaScript::ExecuteFunction(const String& functionName)
{
    LuaFunction* function = GetFunction(functionName);
    return function && function->BeginCall() && function->EndCall();
}

void LuaScript::SendEvent(const String& eventName, VariantMap& eventData)
{
    Object::SendEvent(StringHash(eventName), eventData);
}

void LuaScript::SetExecuteConsoleCommands(bool enable)
{
    if (enable == executeConsoleCommands_)
        return;

    executeConsoleCommands_ = enable;
    if (enable)
        SubscribeToEvent(E_CONSOLECOMMAND, HANDLER(LuaScript, HandleConsoleCommand));
    else
        UnsubscribeFromEvent(E_CONSOLECOMMAND);
}

void LuaScript::RegisterLoader()
{
    // Get package.loaders table
    lua_getglobal(luaState_, "package");
    lua_getfield(luaState_, -1, "loaders");

    // Add LuaScript::Loader to the end of the table
    lua_pushinteger(luaState_, lua_objlen(luaState_, -1) + 1);
    lua_pushcfunction(luaState_, &LuaScript::Loader);
    lua_settable(luaState_, -3);
    lua_pop(luaState_, 2);
}

int LuaScript::AtPanic(lua_State* L)
{
    String errorMessage = luaL_checkstring(L, -1);
    LOGERROR("Lua error: Error message = '" + errorMessage + "'");
    lua_pop(L, 1);
    return 0;
}

int LuaScript::Loader(lua_State* L)
{
    ResourceCache* cache = ::GetContext(L)->GetSubsystem<ResourceCache>();

    // Get module name
    const char* name = luaL_checkstring(L, 1);

    // Attempt to get .luc file first.
    String lucFileName = String(name) + ".luc";
    LuaFile* lucFile = cache->GetResource<LuaFile>(lucFileName, false);
    if (lucFile)
        return lucFile->LoadChunk(L) ? 1 : 0;

    // Then try to get .lua file. If this also fails, error is logged and resource not found event is sent
    String luaFileName = String(name) + ".lua";
    LuaFile* luaFile = cache->GetResource<LuaFile>(luaFileName);
    if (luaFile)
        return luaFile->LoadChunk(L) ? 1 : 0;

    return 0;
}

void LuaScript::ReplacePrint()
{
    static const struct luaL_reg reg[] =
    {
        {"print", &LuaScript::Print},
        { NULL, NULL}
    };

    lua_getglobal(luaState_, "_G");
    luaL_register(luaState_, NULL, reg);
    lua_pop(luaState_, 1);
}

int LuaScript::Print(lua_State *L)
{
    String string;
    int n = lua_gettop(L);
    lua_getglobal(L, "tostring");
    for (int i = 1; i <= n; i++)
    {
        const char *s;
        // Function to be called
        lua_pushvalue(L, -1);
        // Value to print
        lua_pushvalue(L, i);
        lua_call(L, 1, 1);
        // Get result
        s = lua_tostring(L, -1);
        if (s == NULL)
            return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));

        if (i > 1)
            string.Append("    ");

        string.Append(s);
        // Pop result
        lua_pop(L, 1);
    }

    LOGRAW(string + "\n");

    return 0;
}

LuaFunction* LuaScript::GetFunction(int functionIndex)
{
    if (!lua_isfunction(luaState_, functionIndex))
        return 0;

    const void* functionPointer = lua_topointer(luaState_, functionIndex);
    if (!functionPointer)
        return 0;

    HashMap<const void*, SharedPtr<LuaFunction> >::Iterator i = functionPointerToFunctionMap_.Find(functionPointer);
    if (i != functionPointerToFunctionMap_.End())
        return i->second_;

    lua_pushvalue(luaState_, functionIndex);
    int functionRef = luaL_ref(luaState_, LUA_REGISTRYINDEX);

    SharedPtr<LuaFunction> function(new LuaFunction(luaState_, functionRef, false));
    functionPointerToFunctionMap_[functionPointer] = function;

    return function;
}

LuaFunction* LuaScript::GetFunction(const String& functionName, bool silentIfNotFound)
{
    if (!luaState_)
        return 0;

    HashMap<String, SharedPtr<LuaFunction> >::Iterator i = functionNameToFunctionMap_.Find(functionName);
    if (i != functionNameToFunctionMap_.End())
        return i->second_;

    int top = lua_gettop(luaState_);

    SharedPtr<LuaFunction> function;
    if (PushScriptFunction(functionName, silentIfNotFound))
        function = GetFunction(-1);

    lua_settop(luaState_, top);

    functionNameToFunctionMap_[functionName] = function;
    return function;
}

void LuaScript::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{   
    if (coroutineUpdate_ && coroutineUpdate_->BeginCall())
    {
        using namespace PostUpdate;
        float timeStep = eventData[P_TIMESTEP].GetFloat();
        coroutineUpdate_->PushFloat(timeStep);
        coroutineUpdate_->EndCall();
    }

    // Collect garbage
    {
        PROFILE(LuaCollectGarbage);
        lua_gc(luaState_, LUA_GCCOLLECT, 0);
    }
}

void LuaScript::HandleConsoleCommand(StringHash eventType, VariantMap& eventData)
{
    using namespace ConsoleCommand;
    if (eventData[P_ID].GetString() == GetTypeName())
        ExecuteString(eventData[P_COMMAND].GetString());
}

bool LuaScript::PushScriptFunction(const String& functionName, bool silentIfNotFound)
{
    Vector<String> splitedNames = functionName.Split('.');

    String currentName = splitedNames.Front();
    lua_getglobal(luaState_, currentName.CString());

    if (splitedNames.Size() > 1)
    {
        if (!lua_istable(luaState_, -1))
        {
            LOGERROR("Could not find Lua table: Table name = '" + currentName + "'");
            return false;
        }

        for (unsigned i = 1; i < splitedNames.Size() - 1; ++i)
        {
            currentName = currentName + "." + splitedNames[i];
            lua_getfield(luaState_, -1, splitedNames[i].CString());
            if (!lua_istable(luaState_, -1))
            {
                LOGERROR("Could not find Lua table: Table name = '" + currentName + "'");
                return false;
            }
        }

        currentName = currentName + "." + splitedNames.Back().CString();
        lua_getfield(luaState_, -1, splitedNames.Back().CString());
    }

    if (!lua_isfunction(luaState_, -1))
    {
        if (!silentIfNotFound)
            LOGERROR("Could not find Lua function: Function name = '" + currentName + "'");
        return false;
    }

    return true;
}

void RegisterLuaScriptLibrary(Context* context)
{
    LuaFile::RegisterObject(context);
    LuaScriptInstance::RegisterObject(context);
}

}
