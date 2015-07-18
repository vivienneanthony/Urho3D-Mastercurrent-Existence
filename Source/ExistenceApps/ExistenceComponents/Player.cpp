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

#include <Urho3D/Urho3D.h>

#include "../../../Urho3D/Graphics/AnimationController.h"
#include "../../../Urho3D/Core/Context.h"
#include "../../../Urho3D/IO/MemoryBuffer.h"
#include "../../../Urho3D/Physics/PhysicsEvents.h"
#include "../../../Urho3D/Physics/PhysicsWorld.h"
#include "../../../Urho3D/Physics/RigidBody.h"
#include "../../../Urho3D/Scene/Scene.h"
#include "../../../Urho3D/Scene/SceneEvents.h"


#include <iostream>

#include "Character.h"
#include "../Player.h"

using namespace std;

Player::Player()
{
    //ctor
}

Player::~Player()
{
    //dtor
}

/// Set and get the character health
int Player::SetHealth(int health)
{
    CharacterPlayer.health = health;

    return 1;

}

int Player::GetHealth(void)
{
    return CharacterPlayer.health;
}


/// Don nothing now;
int Player::Start(void)
{

    /// Initalize information
    CharacterPlayer.firstname="";
    CharacterPlayer.middlename="";
    CharacterPlayer.lastname="";

    CharacterPlayer.health= 0;

    /// player moves for surface of planet to a ship so it relations change for the x,y,z for respective object
    CharacterPlayer.reputation1= 0;
    CharacterPlayer.reputation2= 0;
    CharacterPlayer.reputation3= 0;
    CharacterPlayer.reputation4= 0;
    CharacterPlayer.reputation5= 0;

    /// Set player experience
    CharacterPlayer.level=0;
    CharacterPlayer.reputation=0;
    CharacterPlayer.experience = 0;
    CharacterPlayer.personalitytrait=0;

    /// Charcter specs
    CharacterPlayer.alienrace=0;
    CharacterPlayer.alienalliancealigned=0;
    CharacterPlayer.gender=0;


    return true;
}

int Player::Clear(void)
{
    Start();

    return 1;
}

/// Get player characteristics
playercharacteristics Player::GetCharacteristics(void)
{
    /// Charcter specs
    playercharacteristics Temporarycharacteristics;

    Temporarycharacteristics.gender = CharacterPlayer.gender;
    Temporarycharacteristics.personalitytrait=CharacterPlayer.personalitytrait;

    return Temporarycharacteristics;
}


/// Get player alliance
playeralliance Player::GetAlliance(void)
{
    /// Charcter specs
    playeralliance Temporaryalliance;

    Temporaryalliance.alienrace = CharacterPlayer.alienrace;
    Temporaryalliance.alienalliancealigned=CharacterPlayer.alienalliancealigned;

    return Temporaryalliance;
}

/// Set player characteristics
int Player::SetCharacteristics(playercharacteristics TempCharacteristics)
{
    /// Set character charactistics
    CharacterPlayer.gender= TempCharacteristics.gender;
    CharacterPlayer.personalitytrait= TempCharacteristics.personalitytrait;

    return 1;
}


/// Set player alliance
int Player::SetAlliance(playeralliance TempAlliance)
{
    /// Set charcter alliance
    CharacterPlayer.alienrace=TempAlliance.alienrace;
    CharacterPlayer.alienalliancealigned=TempAlliance.alienalliancealigned;

    return 1;
}

int Player::SetPlayerInfo(playerbasicinfo TempPlayer)
{
    /// Set charcter alliance
    CharacterPlayer.firstname=TempPlayer.firstname;
    CharacterPlayer.middlename=TempPlayer.middlename;
    CharacterPlayer.lastname=TempPlayer.lastname;

    return 1;
}

playerbasicinfo Player::GetPlayerInfo(void)
{
    /// Charcter specs
    playerbasicinfo Temporarybasicinfo;

    /// Set charcter alliance
    Temporarybasicinfo.firstname = CharacterPlayer.firstname;
    Temporarybasicinfo.middlename = CharacterPlayer.middlename;
    Temporarybasicinfo.lastname = CharacterPlayer.lastname;

    return Temporarybasicinfo;
}

int Player::SetLevels(playerlevels TempPlayer)
{
    /// Set charcter alliance
    CharacterPlayer.level=TempPlayer.level;
    CharacterPlayer.experience=TempPlayer.experience;
    CharacterPlayer.reputation=TempPlayer.reputation;
    CharacterPlayer.reputation1=TempPlayer.reputation1;
    CharacterPlayer.reputation2=TempPlayer.reputation2;
    CharacterPlayer.reputation3=TempPlayer.reputation3;
    CharacterPlayer.reputation4=TempPlayer.reputation4;

    return 1;
}

playerlevels Player::GetLevels(void)
{
    /// Charcter specs
    playerlevels Temporarylevels;

    /// Set charcter alliance
    Temporarylevels.level = CharacterPlayer.level;
    Temporarylevels.experience = CharacterPlayer.experience;
    Temporarylevels.reputation = CharacterPlayer.reputation;
    Temporarylevels.reputation1 = CharacterPlayer.reputation1;
    Temporarylevels.reputation2 = CharacterPlayer.reputation2;
    Temporarylevels.reputation3 = CharacterPlayer.reputation3;
    Temporarylevels.reputation4 = CharacterPlayer.reputation4;

    return Temporarylevels;
}

