#ifndef INTERACTEVENT_H
#define INTERACTEVENT_H


/// Key pressed.
EVENT(INTERACTEVENT, InteractEvent)
{
    PARAM(P_CMD, cmdType);                // States
    PARAM(P_ARG, cmdArguments);
    PARAM(P_OBJ, sender);                // class
}
///}

#endif
