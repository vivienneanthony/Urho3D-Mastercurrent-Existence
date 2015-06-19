#ifndef GAMESTATEEVENT_H
#define GAMESTATEEVENT_H

//namespace ExistenceClientStates
//{
/// P_CMD
enum GameStates
{
    GAME_STATE_SINGLETON,
    GAME_STATE_LOGIN,
    GAME_STATE_ACCOUNTCREATE,
    GAME_STATE_PROGRESS,
    GAME_STATE_PLAYERCREATE,
    GAME_STATE_MAINMENU,
    GAME_STATE_GAMEMODE,
    GAME_STATE_SPLASH,
    GAME_STATE_EXIT_SIM
};

/// Key pressed.
EVENT(G_STATES_CHANGE, GameState)
{
    PARAM(P_CMD, cmdType);                // States
    PARAM(P_ARG, cmdArguments);
    PARAM(P_OBJ, sender);                // class
}
///}

#endif
