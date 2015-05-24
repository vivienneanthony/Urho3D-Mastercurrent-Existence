#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <string>

#define PLAYERFILE "character.xml"


/// Define player levels
using namespace  std;

struct playeralliance
{
    /// Player alliances
    unsigned int alienrace;
    bool alienalliancealigned;
};

struct playerbasicinfo
{
    /// name information
    std::string firstname;
    std::string middlename;
    std::string lastname;
};

struct playercharacteristics
{
    /// Player Characterteristics
    unsigned int gender;
    unsigned int personalitytrait;
};

struct playerlevels
{
    /// Set player experience
    unsigned int level;
    unsigned int experience;
    unsigned int reputation;

    /// player moves for surface of planet to a ship so it relations change for the x,y,z for respective object
    unsigned int reputation1;
    unsigned int reputation2;
    unsigned int reputation3;
    unsigned int reputation4;
    unsigned int reputation5;
};

/// Player Object structure
struct playerobject
{
    /// hffehgjfa3eaadfff2xZealth
    int health;

    /// name information
    std::string firstname;
    std::string middlename;
    std::string lastname;

    /// Set player experience
    unsigned long level;
    unsigned long experience;
    unsigned long reputation;

    /// player moves for surface of planet to a ship so it relations change for the x,y,z for respective object
    unsigned long reputation1;
    unsigned long reputation2;
    unsigned long reputation3;
    unsigned long reputation4;
    unsigned long reputation5;

    /// character spec
    unsigned int alienrace;
    bool alienalliancealigned;
    unsigned int gender;
    unsigned int personalitytrait;
};


/// Create a player class
class Player
{
public:
    Player();
    virtual ~Player();
    int Start(void);
    int Clear(void);

    /// Set and get the character health
    int SetHealth(int health);
    int GetHealth(void);

    /// Set and get player characteristc and alliances (Primarily for temporary usage)
    playercharacteristics GetCharacteristics(void);
    playeralliance GetAlliance(void);
    int SetCharacteristics(playercharacteristics TempCharacteristics);
    int SetAlliance(playeralliance TempAlliance);

    /// Set basic information
    int SetPlayerInfo(playerbasicinfo TempPlayer);
    playerbasicinfo GetPlayerInfo(void);

    int SetLevels(playerlevels TempPlayer);
    playerlevels GetLevels(void);

protected:
    playerobject CharacterPlayer;


private:

};

#endif // PLAYER_H
