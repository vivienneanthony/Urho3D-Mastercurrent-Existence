#ifndef EXISTENCECLIENTSTATEPROGRESS_H
#define EXISTENCECLIENTSTATEPROGRESS_H

#define PROGRESSDEFAULT 0
#define PROGRESSTIMEOUT 1

#define PROGRESSSCENELOAD 0
#define PROGRESSCHARACTERLOAD 1

#include "../../../Urho3D/Core/Object.h"

/// Key pressed.
EVENT(P_LOAD_CHANGE, LoadState)
{
    PARAM(P_CMD, cmdType);               // States
    PARAM(P_ARG, cmdArguments);          // Arguments
    PARAM(P_OBJ, cmdsender);                // class
}

#endif
