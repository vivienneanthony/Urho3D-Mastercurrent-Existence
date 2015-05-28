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

#include "../../../Urho3D/Core/CoreEvents.h"
#include "../../../Urho3D/Engine/Engine.h"
#include "../../../Urho3D/UI/Font.h"
#include "../../../Urho3D/Input/Input.h"
#include "../../../Urho3D/Core/ProcessUtils.h"
#include "../../../Urho3D/UI/Text.h"
#include "../../../Urho3D/UI/UI.h"
#include "../../../Urho3D/Scene/Scene.h"
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
#include "Character.h"
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

#include "GameStateHandler.h"
#include "GameObject.h"
#include "EnvironmentBuild.h"
#include "Manager.h"
#include "../Account.h"

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

#include "../../../Urho3D/Procedural/Procedural.h"
#include "../../../Urho3D/Procedural/ProceduralTerrain.h"
#include "../../../Urho3D/Procedural/RandomNumberGenerator.h"

#include "ExistenceClient.h"
#include "ExistenceClientUI.h"

#include "../../Urho3D/Engine/DebugHud.h"


#define DEFAULTSIZE 4096

using namespace std;
using namespace Urho3D;
/// Load account information from a account file
void ExistenceClient::LoadAccount(void)
{
    String accountconfigfilename=ACCOUNTFILE;
    XMLElement accountElement;

    accountexist = NULL;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* accountconfig = cache->GetResource<XMLFile>(accountconfigfilename);

    /// if account file is found
    if(accountconfig!=NULL)
    {
        /// get account root
        accountElement = accountconfig -> GetRoot("account");

        /// return because of error no account
        if(accountElement==0)
        {
            return;
        }

        /// read info
        XMLElement idElement = accountElement . GetChild("uniqueid");
        XMLElement emailElement = accountElement . GetChild("email");
        XMLElement firstnameElement = accountElement . GetChild("firstname");
        XMLElement middlenameElement = accountElement . GetChild("middlename");
        XMLElement lastnameElement = accountElement . GetChild("lastname");
        XMLElement passwordElement = accountElement . GetChild("password");
    }

    return;
}


/// Load account information from a account file
int ExistenceClient::LoadAccountPlayers(void)
{

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    String accountplayersfile;

    accountplayersfile.Append(filesystem->GetProgramDir().CString());
    accountplayersfile.Append("CoreData/");
    accountplayersfile.Append(PLAYERFILE);

    /// Set XML related elements
    XMLFile* charactersplayer = cache->GetResource<XMLFile>(accountplayersfile);
    XMLElement charactersRootElement;
    XMLElement NextSibling;

    /// if account file is found
    if(charactersplayer!=NULL)
    {

        /// get account root
        charactersRootElement = charactersplayer -> GetRoot("characters");

        /// Set Counter
        unsigned int counter=0;

        NextSibling=charactersRootElement .GetChild();

        /// Resize memory to character list
        TemporaryAccountPlayerList = new Player[4];

        /// Loop through xml data
        do
        {

            /// Temporay player Info
            playerbasicinfo tempplayerinfo;
            playercharacteristics tempplayercharacteristics;
            playeralliance tempplayeralliance;

            /// Get elements
            XMLElement firstnameElement  = NextSibling.GetChild("firstname");
            XMLElement middlenameElement  = NextSibling.GetChild("middlename");
            XMLElement lastnameElement  = NextSibling.GetChild("lastname");
            XMLElement reputation1Element  = NextSibling.GetChild("reputation1");
            XMLElement reputation2Element  = NextSibling.GetChild("reputation2");
            XMLElement reputation3Element  = NextSibling.GetChild("reputation3");
            XMLElement reputation4Element  = NextSibling.GetChild("reputation4");
            XMLElement experienceElement  = NextSibling.GetChild("experience");
            XMLElement alienraceElement  = NextSibling.GetChild("alienrace");
            XMLElement alienalliancealignedElement  = NextSibling.GetChild("alienalliancealigned");
            XMLElement genderElement  = NextSibling.GetChild("gender");
            XMLElement personalitytraitElement  = NextSibling.GetChild("personalitytrait");

            /// Assisgn values
            tempplayerinfo.firstname.append(firstnameElement.GetAttributeCString("firstname"));
            tempplayerinfo.middlename.append(middlenameElement.GetAttributeCString("middlename"));
            tempplayerinfo.lastname.append(lastnameElement.GetAttributeCString("lastname"));

            tempplayercharacteristics.gender=genderElement.GetUInt("gender");
            tempplayercharacteristics.personalitytrait=genderElement.GetUInt("personalitytrait");

            tempplayeralliance.alienrace=alienraceElement.GetUInt("alienrace");
            tempplayeralliance.alienalliancealigned=alienalliancealignedElement.GetUInt("alienalliancealigned");

            /// Save information to table
            TemporaryAccountPlayerList[counter].SetAlliance(tempplayeralliance);
            TemporaryAccountPlayerList[counter].SetPlayerInfo(tempplayerinfo);
            TemporaryAccountPlayerList[counter].SetCharacteristics(tempplayercharacteristics);

            /// Get next info
            counter++;

            /// Break do loop
            if(counter==3)
            {
                break;
            }
        }
        while((NextSibling=NextSibling.GetNext())!=NULL);

        /// Save limit
        TemporaryAccountPlayerListLimit=counter++;

        return 1;
    }

    return 0;
}


/// Save account information to a file
void ExistenceClient::SaveAccount(accountinformation account)
{
    String accountconfigfilename;

    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    accountconfigfilename.Append(filesystem->GetProgramDir().CString());
    accountconfigfilename.Append("CoreData/");
    accountconfigfilename.Append(ACCOUNTFILE);

    File saveFile(context_, accountconfigfilename.CString(), FILE_WRITE);

    /// Check if the account file information exist
    if(!filesystem->FileExists(accountconfigfilename.CString()))
    {
        //cout << "\r\nAccount file ("<< accountconfigfilename.CString() << ") does not exist.";
    }

    XMLFile * accountconfig  = new XMLFile(context_);

    XMLElement configElem = accountconfig -> CreateRoot("account");
    XMLElement idElement = configElem.CreateChild("uniqueid");
    XMLElement emailElement= configElem.CreateChild("email");
    XMLElement usernameElement  = configElem.CreateChild("username");
    XMLElement firstnameElement  = configElem.CreateChild("firstname");
    XMLElement middlenameElement  = configElem.CreateChild("middlename");
    XMLElement lastnameElement  = configElem.CreateChild("lastname");
    XMLElement passwordElement = configElem.CreateChild("password");

    String AccountEmail(account.email.c_str());
    String AccountUsername(account.username.c_str());
    String AccountFirstname(account.firstname.c_str());
    String AccountMiddlename(account.middlename.c_str());
    String AccountLastname(account.lastname.c_str());
    String AccountPassword(account.password.c_str());

    idElement.SetString("uniqueid", "test");
    emailElement.SetString("email", AccountEmail);
    usernameElement.SetString("username", AccountUsername);
    firstnameElement.SetString("firstname", AccountFirstname);
    middlenameElement.SetString("middlename", AccountMiddlename);
    lastnameElement.SetString("lastname", AccountLastname);
    passwordElement.SetString("password", AccountPassword);

    accountconfig->Save(saveFile);

    return;
}

/// Save player information to a file either non-active newly created
void ExistenceClient::SavePlayer(bool activeplayer)
{

    /// Get resources for function
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Create variablle to test if a file exist
    bool fileexist=false, characterfound = false;

    /// Set variable to retain player information
    playerbasicinfo TempBasicInfo;
    playercharacteristics TempCharacteristics;
    playeralliance TempAlliance;
    playerlevels TempLevels;

    /// String for player filename
    String playerconfigfilename;

    playerconfigfilename.Append(filesystem->GetProgramDir().CString());
    playerconfigfilename.Append("CoreData/");
    playerconfigfilename.Append(PLAYERFILE);

    /// Check filesystem for character player file
    if(!filesystem->FileExists(playerconfigfilename.CString()))
    {
        fileexist=false;
    }

    /// Check filesystem for character file
    if(filesystem->FileExists(playerconfigfilename.CString()))
    {
        fileexist=true;
    }

    /// Set player info from Temp or Player
    if(activeplayer)
    {
        /// Get Player info from game mode
        TempBasicInfo = character_  -> GetPlayerInfo();
        TempCharacteristics = character_  -> GetCharacteristics();
        TempAlliance = character_ -> GetAlliance();
        TempLevels = character_ ->GetLevels();

    }
    else
    {
        /// Get player info from the actual temp
        TempBasicInfo = TemporaryPlayer.GetPlayerInfo();
        TempCharacteristics = TemporaryPlayer.GetCharacteristics();
        TempAlliance = TemporaryPlayer.GetAlliance();
        TempLevels = TemporaryPlayer.GetLevels();
    }

    /// If file did not exist create a new player file
    if (!fileexist)
    {
        File saveFile(context_, playerconfigfilename.CString(), FILE_READWRITE);

        XMLFile * playerconfig  = new XMLFile(context_);

        /// Create xml child
        XMLElement configElement = playerconfig -> CreateRoot("characters");
        XMLElement characterElement =  configElement.CreateChild("character");
        XMLElement firstnameElement  = characterElement.CreateChild("firstname");
        XMLElement middlenameElement  = characterElement.CreateChild("middlename");
        XMLElement lastnameElement  = characterElement.CreateChild("lastname");
        XMLElement levelElement  = characterElement.CreateChild("level");
        XMLElement reputationElement  = characterElement.CreateChild("reputation");
        XMLElement reputation1Element  = characterElement.CreateChild("reputation1");
        XMLElement reputation2Element  = characterElement.CreateChild("reputation2");
        XMLElement reputation3Element  = characterElement.CreateChild("reputation3");
        XMLElement reputation4Element  = characterElement.CreateChild("reputation4");
        XMLElement experienceElement  = characterElement.CreateChild("experience");
        XMLElement alienraceElement  = characterElement.CreateChild("alienrace");
        XMLElement alienalliancealignedElement  = characterElement.CreateChild("alienalliancealigned");
        XMLElement genderElement  = characterElement.CreateChild("gender");
        XMLElement personalitytraitElement  = characterElement.CreateChild("personalitytrait");

        /// copy player information into xml elements
        characterElement.SetString("character", "character");
        firstnameElement.SetString("firstname", String(TempBasicInfo.firstname.c_str()));
        middlenameElement.SetString("middlename", String(TempBasicInfo.middlename.c_str()));
        lastnameElement.SetString("lastname", String(TempBasicInfo.lastname.c_str()));
        levelElement.SetUInt("level",TempLevels.level);
        reputationElement.SetUInt("reputation",TempLevels.reputation);
        reputation1Element.SetUInt("reputation1",TempLevels.reputation1);
        reputation2Element.SetUInt("reputation2",TempLevels.reputation2);
        reputation3Element.SetUInt("reputation3",TempLevels.reputation3);
        reputation4Element.SetUInt("reputation4",TempLevels.reputation4);
        experienceElement.SetUInt("experience",TempLevels.experience);
        alienraceElement.SetUInt("alienrace",TempAlliance.alienrace);
        alienalliancealignedElement.SetBool("alienalliancealigned",TempAlliance.alienalliancealigned);
        genderElement.SetUInt("gender",TempCharacteristics.gender);
        personalitytraitElement.SetUInt("personalitytrait",TempCharacteristics.personalitytrait);

        playerconfig->Save(saveFile);
    }


    /// If character file exist  update player info
    if(fileexist)
    {
        /// Create XMLElements for character player exist
        XMLElement characterElement;
        XMLElement firstnameElement;
        XMLElement lastnameElement;

        File saveFile(context_, playerconfigfilename.CString(), FILE_READ);

        XMLFile * playerconfig  = new XMLFile(context_);

        playerconfig->Load(saveFile);

        /// Get root element
        XMLElement configElement = playerconfig -> GetRoot("characters");

        characterElement = configElement.GetChild("character");

        /// Loop through all players
        do
        {
            /// get each first and lastname
            firstnameElement = characterElement.GetChild("firstname");
            lastnameElement = characterElement.GetChild("lastname");

            /// check for matching character by firstname
            if(firstnameElement.GetAttributeCString("firstname")==TempBasicInfo.firstname&&lastnameElement.GetAttributeCString("lastname")==TempBasicInfo.lastname)
            {
                /// player found
                characterfound = true;
                break;
            }
        }
        while(characterElement = characterElement.GetNext("character"));

        /// if player not found
        if(characterfound&&activeplayer==true)
        {
            XMLElement firstnameElement  = characterElement.GetChild("firstname");
            XMLElement middlenameElement  = characterElement.GetChild("middlename");
            XMLElement lastnameElement  = characterElement.GetChild("lastname");
            XMLElement levelElement  = characterElement.GetChild("level");
            XMLElement reputationElement  = characterElement.GetChild("reputation");
            XMLElement reputation1Element  = characterElement.GetChild("reputation1");
            XMLElement reputation2Element  = characterElement.GetChild("reputation2");
            XMLElement reputation3Element  = characterElement.GetChild("reputation3");
            XMLElement reputation4Element  = characterElement.GetChild("reputation4");
            XMLElement experienceElement  = characterElement.GetChild("experience");
            XMLElement alienraceElement  = characterElement.GetChild("alienrace");
            XMLElement alienalliancealignedElement  = characterElement.GetChild("alienalliancealigned");
            XMLElement genderElement  = characterElement.GetChild("gender");
            XMLElement personalitytraitElement  = characterElement.GetChild("personalitytrait");

            /// copy player information into xml elements
            firstnameElement.SetString("firstname", String(TempBasicInfo.firstname.c_str()));
            middlenameElement.SetString("middlename", String(TempBasicInfo.middlename.c_str()));
            lastnameElement.SetString("lastname", String(TempBasicInfo.lastname.c_str()));
            levelElement.SetUInt("level",TempLevels.level);
            reputationElement.SetUInt("reputation",TempLevels.reputation);
            reputation1Element.SetUInt("reputation1",TempLevels.reputation1);
            reputation2Element.SetUInt("reputation2",TempLevels.reputation2);
            reputation3Element.SetUInt("reputation3",TempLevels.reputation3);
            reputation4Element.SetUInt("reputation4",TempLevels.reputation4);
            experienceElement.SetUInt("experience",TempLevels.experience);
            alienraceElement.SetUInt("alienrace",TempAlliance.alienrace);
            alienalliancealignedElement.SetBool("alienalliancealigned",TempAlliance.alienalliancealigned);
            genderElement.SetUInt("gender",TempCharacteristics.gender);
            personalitytraitElement.SetUInt("personalitytrait",TempCharacteristics.personalitytrait);
        }
        else
        {
            characterElement = configElement .CreateChild("character");
            XMLElement firstnameElement  = characterElement.CreateChild("firstname");
            XMLElement middlenameElement  = characterElement.CreateChild("middlename");
            XMLElement lastnameElement  = characterElement.CreateChild("lastname");
            XMLElement levelElement  = characterElement.CreateChild("level");
            XMLElement reputationElement  = characterElement.CreateChild("reputation");
            XMLElement reputation1Element  = characterElement.CreateChild("reputation1");
            XMLElement reputation2Element  = characterElement.CreateChild("reputation2");
            XMLElement reputation3Element  = characterElement.CreateChild("reputation3");
            XMLElement reputation4Element  = characterElement.CreateChild("reputation4");
            XMLElement experienceElement  = characterElement.CreateChild("experience");
            XMLElement alienraceElement  = characterElement.CreateChild("alienrace");
            XMLElement alienalliancealignedElement  = characterElement.CreateChild("alienalliancealigned");
            XMLElement genderElement  = characterElement.CreateChild("gender");
            XMLElement personalitytraitElement  = characterElement.CreateChild("personalitytrait");

            /// copy player information into xml elements
            characterElement.SetString("character", "character");
            firstnameElement.SetString("firstname", String(TempBasicInfo.firstname.c_str()));
            middlenameElement.SetString("middlename", String(TempBasicInfo.middlename.c_str()));
            lastnameElement.SetString("lastname", String(TempBasicInfo.lastname.c_str()));
            levelElement.SetUInt("level",TempLevels.level);
            reputationElement.SetUInt("reputation",TempLevels.reputation);
            reputation1Element.SetUInt("reputation1",TempLevels.reputation1);
            reputation2Element.SetUInt("reputation2",TempLevels.reputation2);
            reputation3Element.SetUInt("reputation3",TempLevels.reputation3);
            reputation4Element.SetUInt("reputation4",TempLevels.reputation4);
            experienceElement.SetUInt("experience",TempLevels.experience);
            alienraceElement.SetUInt("alienrace",TempAlliance.alienrace);
            alienalliancealignedElement.SetBool("alienalliancealigned",TempAlliance.alienalliancealigned);
            genderElement.SetUInt("gender",TempCharacteristics.gender);
            personalitytraitElement.SetUInt("personalitytrait",TempCharacteristics.personalitytrait);


        }

        File updateFile(context_, playerconfigfilename.CString(), FILE_WRITE);

        playerconfig->Save(updateFile);

    }

    return;
}




/// Load account information from a account file
int ExistenceClient::LoadTemporaryPlayer(int player)
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    String accountplayersfile;

    accountplayersfile.Append(filesystem->GetProgramDir().CString());
    accountplayersfile.Append("CoreData/");
    accountplayersfile.Append(PLAYERFILE);

    /// Set XML related elements
    XMLFile* charactersplayer = cache->GetResource<XMLFile>(accountplayersfile);
    XMLElement charactersRootElement;
    XMLElement NextSibling;

    /// if account file is found
    if(charactersplayer!=NULL)
    {
        /// get account root
        charactersRootElement = charactersplayer -> GetRoot("characters");

        /// Set Counter
        unsigned int counter=0;

        NextSibling=charactersRootElement .GetChild();

        for(unsigned int i=0; i<player; i++)
        {
            NextSibling=NextSibling.GetNext();
        }

        /// Temporay player Info
        playerbasicinfo tempplayerinfo;
        playercharacteristics tempplayercharacteristics;
        playeralliance tempplayeralliance;
        playerlevels templevels;

        /// Get elements
        XMLElement firstnameElement  = NextSibling.GetChild("firstname");
        XMLElement middlenameElement  = NextSibling.GetChild("middlename");
        XMLElement lastnameElement  = NextSibling.GetChild("lastname");
        XMLElement levelElement  = NextSibling.GetChild("level");
        XMLElement experienceElement  = NextSibling.GetChild("experience");
        XMLElement reputationElement  = NextSibling.GetChild("reputation");
        XMLElement reputation1Element  = NextSibling.GetChild("reputation1");
        XMLElement reputation2Element  = NextSibling.GetChild("reputation2");
        XMLElement reputation3Element  = NextSibling.GetChild("reputation3");
        XMLElement reputation4Element  = NextSibling.GetChild("reputation4");
        XMLElement alienraceElement  = NextSibling.GetChild("alienrace");
        XMLElement alienalliancealignedElement  = NextSibling.GetChild("alienalliancealigned");
        XMLElement genderElement  = NextSibling.GetChild("gender");
        XMLElement personalitytraitElement  = NextSibling.GetChild("personalitytrait");

        /// Assisgn values
        tempplayerinfo.firstname.append(firstnameElement.GetAttributeCString("firstname"));
        tempplayerinfo.middlename.append(middlenameElement.GetAttributeCString("middlename"));
        tempplayerinfo.lastname.append(lastnameElement.GetAttributeCString("lastname"));

        tempplayercharacteristics.gender=genderElement.GetUInt("gender");
        tempplayercharacteristics.personalitytrait=genderElement.GetUInt("personalitytrait");

        tempplayercharacteristics.gender=genderElement.GetUInt("gender");

        templevels.level= levelElement.GetUInt("level");
        templevels.experience=experienceElement.GetUInt("experience");
        templevels.reputation=reputationElement.GetUInt("reputation");
        templevels.reputation1=reputation1Element.GetUInt("reputation1");
        templevels.reputation2=reputation2Element.GetUInt("reputation2");
        templevels.reputation3=reputation3Element.GetUInt("reputation3");
        templevels.reputation4=reputation4Element.GetUInt("reputation4");

        tempplayeralliance.alienrace=alienraceElement.GetUInt("alienrace");
        tempplayeralliance.alienalliancealigned=alienalliancealignedElement.GetBool("alienalliancealigned");

        TemporaryPlayer.Clear();

        /// Copy to player
        TemporaryPlayer.SetAlliance(tempplayeralliance);
        TemporaryPlayer.SetCharacteristics(tempplayercharacteristics);
        TemporaryPlayer.SetPlayerInfo(tempplayerinfo);
        TemporaryPlayer.SetLevels(templevels);

        /// Set health
        TemporaryPlayer.SetHealth(100);
    }

    return 0;
}


/// Load account information from a account file
int ExistenceClient::LoadPlayer(int player)
{
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    String accountplayersfile;

    accountplayersfile.Append(filesystem->GetProgramDir().CString());
    accountplayersfile.Append("CoreData/");
    accountplayersfile.Append(PLAYERFILE);

    /// Set XML related elements
    XMLFile* charactersplayer = cache->GetResource<XMLFile>(accountplayersfile);
    XMLElement charactersRootElement;
    XMLElement NextSibling;

    /// if account file is found
    if(charactersplayer!=NULL)
    {
        /// get account root
        charactersRootElement = charactersplayer -> GetRoot("characters");

        /// Set Counter
        unsigned int counter=0;

        NextSibling=charactersRootElement .GetChild();

        for(unsigned int i=0; i<player; i++)
        {
            NextSibling=NextSibling.GetNext();
        }

        /// Temporay player Info
        playerbasicinfo tempplayerinfo;
        playercharacteristics tempplayercharacteristics;
        playeralliance tempplayeralliance;
        playerlevels templevels;

        /// Get elements
        XMLElement firstnameElement  = NextSibling.GetChild("firstname");
        XMLElement middlenameElement  = NextSibling.GetChild("middlename");
        XMLElement lastnameElement  = NextSibling.GetChild("lastname");
        XMLElement levelElement  = NextSibling.GetChild("level");
        XMLElement experienceElement  = NextSibling.GetChild("experience");
        XMLElement reputationElement  = NextSibling.GetChild("reputation");
        XMLElement reputation1Element  = NextSibling.GetChild("reputation1");
        XMLElement reputation2Element  = NextSibling.GetChild("reputation2");
        XMLElement reputation3Element  = NextSibling.GetChild("reputation3");
        XMLElement reputation4Element  = NextSibling.GetChild("reputation4");
        XMLElement alienraceElement  = NextSibling.GetChild("alienrace");
        XMLElement alienalliancealignedElement  = NextSibling.GetChild("alienalliancealigned");
        XMLElement genderElement  = NextSibling.GetChild("gender");
        XMLElement personalitytraitElement  = NextSibling.GetChild("personalitytrait");

        /// Assisgn values
        tempplayerinfo.firstname.append(firstnameElement.GetAttributeCString("firstname"));
        tempplayerinfo.middlename.append(middlenameElement.GetAttributeCString("middlename"));
        tempplayerinfo.lastname.append(lastnameElement.GetAttributeCString("lastname"));

        tempplayercharacteristics.gender=genderElement.GetUInt("gender");
        tempplayercharacteristics.personalitytrait=genderElement.GetUInt("personalitytrait");

        tempplayercharacteristics.gender=genderElement.GetUInt("gender");

        templevels.level= levelElement.GetUInt("level");
        templevels.experience=experienceElement.GetUInt("experience");
        templevels.reputation=reputationElement.GetUInt("reputation");
        templevels.reputation1=reputation1Element.GetUInt("reputation1");
        templevels.reputation2=reputation2Element.GetUInt("reputation2");
        templevels.reputation3=reputation3Element.GetUInt("reputation3");
        templevels.reputation4=reputation4Element.GetUInt("reputation4");

        tempplayeralliance.alienrace=alienraceElement.GetUInt("alienrace");
        tempplayeralliance.alienalliancealigned=alienalliancealignedElement.GetBool("alienalliancealigned");

        /// Copy to player
        character_->SetAlliance(tempplayeralliance);
        character_->SetCharacteristics(tempplayercharacteristics);
        character_->SetPlayerInfo(tempplayerinfo);
        character_->SetLevels(templevels);

        /// Set health
        character_ -> SetHealth(100);
    }

    return 0;
}


/// Load account information from a account file
int ExistenceClient::LoadEnvironmentSettings(const char *environment)
{

    /// Grab resources
    ResourceCache * cache = GetSubsystem<ResourceCache>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    bool findmatch=false;

    /// Check if scene exist
    if(scene_==NULL)
    {
        return 1;
    }

    /// Create String
    String loadenvironmentfile;

    /// Set directory
    loadenvironmentfile.Append(filesystem->GetProgramDir().CString());
    loadenvironmentfile.Append("Resources/ScenesData/");
    loadenvironmentfile.Append("environments.xml");

    /// Check if the account file information exist
    if(!filesystem->FileExists(loadenvironmentfile.CString()))
    {
        return 0;
    }

    XMLFile * loadingfile = new XMLFile(context_);

    /// Create a file in current context
    File LoadFile(context_, loadenvironmentfile.CString(), FILE_READ);

    loadingfile->Load(LoadFile);

    XMLElement ElementRoot = loadingfile->GetRoot("environments");

    XMLElement nextElement = ElementRoot.GetChild("node");

    do
    {
        /// Is enabled
        XMLElement nameElement;

        /// Is enabled
        nameElement = nextElement.GetChild ("attribute");
        /// name
        nameElement = nameElement.GetNext ("attribute");

        if(string(nameElement.GetAttributeCString("value"))==environment)
        {
            findmatch=true;
            break;
        }
        else
        {
            nextElement=nextElement.GetNext();
        }

    }
    while(nextElement.NotNull());

    /// return if node is not found
    if(findmatch==false)
    {
        return 0;
    }


    /// remove nodes
    Node * GeneratedLight1 = scene_ -> GetChild("GeneratedLight_Light1");
    Node * GeneratedLight2 = scene_ -> GetChild("GeneratedLight_Light2");
    Node * GeneratedLight3 = scene_ -> GetChild("GeneratedLight_Light3");

    /// remove
    GeneratedLight1 -> Remove();
    GeneratedLight2 -> Remove();
    GeneratedLight3 -> Remove();

    XMLElement lightElements = nextElement.GetChild("node");

    /// loop through each node
    do
    {
        /// Is enabled
        XMLElement AttributesPosition;

        /// Is enabled
        AttributesPosition = lightElements.GetChild ("attribute");

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

        Node * newNode = scene_->InstantiateXML (lightElements,(const Vector3 &)Vector3(NodePosition),(const Quaternion &)Quaternion(NodeRotation),REPLICATED);

        lightElements=lightElements.GetNext();

    }
    while(lightElements.NotNull());


    return 1;
}

int ExistenceClient::GenerateSceneLoadDifferential(const char *filename)
{
    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();
    Manager * manager_ = GetSubsystem<Manager>();

    /// create variables (urho3d)
    String additionresourcePath;

    additionresourcePath.Append(filesystem->GetProgramDir().CString());
    additionresourcePath.Append("Resources/ScenesData/");

    /// load
    manager_->LoadManagedNodes(filename);

}



/// Load a player mesh into a node
int ExistenceClient::loadplayerMesh(Node * playermeshNode, int alienrace, int gender, int mode)
{
    /// Get Needed SubSystems
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();

    FileSystem * filesystem = GetSubsystem<FileSystem>();

    /// Temporarily define faction information (Might make this a class)
    unsigned int factionslimit=4;
    unsigned int alienslimit=4;

    /// select active
    alienraces Alien[alienslimit];
    string alienracename;

    /// Define structure
    Alien[0].uniqueid=1;
    Alien[0].name.append("humans");
    Alien[0].button.append("logohumans");

    Alien[1].uniqueid=100;
    Alien[1].name.append("romulans");
    Alien[1].button.append("logoromulans");

    Alien[2].uniqueid=200;
    Alien[2].name.append("klingons");
    Alien[2].button.append("logoklingons");

    Alien[3].uniqueid=1000;
    Alien[3].name.append("orcins");
    Alien[3].button.append("logoorcins");

    alienracename.clear();



    /// Select currentalien
    for(unsigned int idx=0; idx<alienslimit; idx++)
    {

        if( alienrace==Alien[idx].uniqueid)
        {
            alienracename.append(Alien[idx].name);
            break;
        }
    }

    Node * playermeshNode1 = playermeshNode -> CreateChild("playermeshNode1");
    Node * playermeshNode2 = playermeshNode -> CreateChild("playermeshNode2");

    /// Created Animated MOdel
    AnimatedModel* playermeshObject1 = playermeshNode1 ->CreateComponent<AnimatedModel>();
    AnimatedModel* playermeshObject2 = playermeshNode2 ->CreateComponent<AnimatedModel>();

    /// Add Animation Controller
    playermeshNode1->CreateComponent<AnimationController>();
    playermeshNode2->CreateComponent<AnimationController>();

    /// Setup mesh and mesh details nodes and stactic models
    if((gender<51&&mode==DISPLAYMESH_SINGLECHARACTER)||mode==DISPLAYMESH_MUILTIPLECHARACTER)
    {
        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("female");

        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";

        playermeshObject1->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        playermeshObject1->ApplyMaterialList(String(temporarytexturefilename.c_str()));
        playermeshObject1->SetCastShadows(true);

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel * playermeshObject1outfitbottom = playermeshNode1->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femalebottom"+".txt";

        playermeshObject1outfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject1outfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject1outfittop =  playermeshNode1->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"femaletop"+".txt";

        playermeshObject1outfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject1outfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject1teeth =  playermeshNode1->CreateComponent<AnimatedModel>();
        playermeshObject1teeth->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleteeth.mdl"));
        playermeshObject1teeth->ApplyMaterialList("Resources/Models/standardfemaleteeth.txt");

        AnimatedModel* playermeshObject1eyes =  playermeshNode1->CreateComponent<AnimatedModel>();
        playermeshObject1eyes->SetModel(cache->GetResource<Model>("Resources/Models/standardfemaleeyes.mdl"));
        playermeshObject1eyes->ApplyMaterialList("Resources/Models/standardfemaleeyes.txt");

        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianfemaleIdleAction.ani");

        playermeshObject1 -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleFemaleAnimation");

        /// Get Controller
        AnimationController * playermeshAnimationController1 = playermeshNode1 -> GetComponent<AnimationController>();

        /// Set Morph Weight
        playermeshAnimationController1-> SetWeight("IdleFemaleAnimation",1);
        playermeshAnimationController1-> SetTime("IdleFemaleAnimation",1.80991);

        playermeshAnimationController1->Play("IdleFemaleAnimation",0,1,0);
    }

    /// Setup mesh and mesh details nodes and stactic models
    if((gender>50&&mode==DISPLAYMESH_SINGLECHARACTER)||mode==DISPLAYMESH_MUILTIPLECHARACTER)
    {
        /// generate generalfile
        string temporaryfilename;
        temporaryfilename.append(filesystem->GetProgramDir().CString());
        temporaryfilename.append("Resources/Models/");
        temporaryfilename.append("standardbody");
        temporaryfilename.append(alienracename);
        temporaryfilename.append("male");


        string temporarymodelfilename=temporaryfilename+".mdl";
        string temporarytexturefilename=temporaryfilename+".txt";

        playermeshObject2->SetModel(cache->GetResource<Model>(String(temporarymodelfilename.c_str())));
        playermeshObject2->ApplyMaterialList(String(temporarytexturefilename.c_str()));

        playermeshObject2->SetCastShadows(true);

        string clothingmodel;
        string clothingmaterial;

        AnimatedModel* playermeshObject2outfitbottom = playermeshNode2->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"malebottom"+".txt";

        playermeshObject2outfitbottom->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject2outfitbottom->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject2outfittop =  playermeshNode2->CreateComponent<AnimatedModel>();

        clothingmodel = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".mdl";
        clothingmaterial = filesystem->GetProgramDir().CString()+string("Resources/Models/")+string("standardoutfit")+alienracename+"maletop"+".txt";

        playermeshObject2outfittop->SetModel(cache->GetResource<Model>(clothingmodel.c_str()));
        playermeshObject2outfittop->ApplyMaterialList(clothingmaterial.c_str());

        AnimatedModel* playermeshObject2teeth =  playermeshNode2->CreateComponent<AnimatedModel>();
        playermeshObject2teeth->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleteeth.mdl"));
        playermeshObject2teeth->ApplyMaterialList("Resources/Models/standardmaleteeth.txt");

        AnimatedModel* playermeshObject2eyes =  playermeshNode2->CreateComponent<AnimatedModel>();
        playermeshObject2eyes->SetModel(cache->GetResource<Model>("Resources/Models/standardmaleeyes.mdl"));
        playermeshObject2eyes->ApplyMaterialList("Resources/Models/standardmaleeyes.txt");


        /// Add animation state
        Animation * IdleAnimation = new Animation(context_);
        IdleAnimation = cache->GetResource<Animation>("Resources/Models/standardbipedolianmaleIdleGuardAction.ani");

        playermeshObject2 -> AddAnimationState(IdleAnimation);
        IdleAnimation -> SetAnimationName ("IdleMaleAnimation");

        /// Get Controller
        AnimationController * playermeshAnimationController2 = playermeshNode2 -> GetComponent<AnimationController>();

        /// Set Morph Weight
        playermeshAnimationController2-> SetWeight("IdleMaleAnimation",1);
        playermeshAnimationController2-> SetTime("IdleMaleAnimation",1.80991);

        playermeshAnimationController2->Play("IdleMaleAnimation",0,1,0);

    }

    /// Position static models if muiltiple charaters shown
    if(mode==DISPLAYMESH_MUILTIPLECHARACTER)
    {
        playermeshNode1 -> SetPosition(Vector3(0.5f,0.0f,0.0f));
        playermeshNode1 ->SetRotation(Quaternion(0.0,10.0,0.0));
        playermeshNode2 -> SetPosition(Vector3(-0.5f,0.0f,0.0f));
        playermeshNode2 ->SetRotation(Quaternion(0.0,-10,0.0));
    }

    return 1;
}

/// Load a scene UI
void ExistenceClient::loadSceneUI(void)
{

    /// get resources
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Renderer* renderer = GetSubsystem<Renderer>();
    Graphics* graphics = GetSubsystem<Graphics>();
    UI* ui = GetSubsystem<UI>();
    FileSystem * filesystem = GetSubsystem<FileSystem>();

    ui->Clear();

    /// Removed old code use load quickmenu

    return;
}
