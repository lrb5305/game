#include "cbase.h"
#include "server_events.h"

#include "tier0/memdbgon.h"

void Momentum::OnServerDLLInit()
{
    TickSet::TickInit();
    // MOM_TODO: connect to site
    if (SteamAPI_IsSteamRunning())
    {
        mom_UTIL->GetRemoteRepoModVersion();
    }
}

//This is only called when "map ____" is called, if the user uses changelevel then...
// \/(o_o)\/
void Momentum::GameInit()
{
    ConVarRef gm("mom_gamemode");
    ConVarRef map("host_map");
    const char *pMapName = map.GetString();
    // This will only happen if the user didn't use the map selector to start a map

    //set gamemode depending on map name
    //MOM_TODO: This needs to read map entity/momfile data and set accordingly
    if (gm.GetInt() == MOMGM_UNKNOWN)
    {
        if (!Q_strnicmp(pMapName, "surf_", strlen("surf_")))
        {
            gm.SetValue(MOMGM_SURF);
        }
        else if (!Q_strnicmp(pMapName, "bhop_", strlen("bhop_")))
        {
            DevLog("SETTING THE GAMEMODE!\n");
            gm.SetValue(MOMGM_BHOP);
        }
        else if (!Q_strnicmp(pMapName, "kz_", strlen("kz_")))
        {
            DevLog("SETTING THE GAMEMODE!\n");
            gm.SetValue(MOMGM_SCROLL);
        }
        else if (!Q_strcmp(pMapName, "background") || !Q_strcmp(pMapName, "credits"))
        {
            gm.SetValue(MOMGM_ALLOWED);
        }
        else
        {
            gm.SetValue(MOMGM_UNKNOWN);
        }
    }
}

void CMOMServerEvents::LevelInitPostEntity()
{
    const char *pMapName = gpGlobals->mapname.ToCStr();
    // (Re-)Load zones
    if (zones)
    {
        delete zones;
        zones = nullptr;
    }
    zones = new CMapzoneData(pMapName);
    zones->SpawnMapZones();

    //Setup timer
    g_Timer->OnMapStart(pMapName);

    // Reset zone editing
    g_MapzoneEdit.Reset();

    //disable point_servercommand
    ConVarRef servercommand("sv_allow_point_servercommand");
    servercommand.SetValue("0");
}

void CMOMServerEvents::LevelShutdownPreEntity()
{
    const char *pMapName = gpGlobals->mapname.ToCStr();
    // Unload zones
    if (zones)
    {
        delete zones;
        zones = nullptr;
    }

    ConVarRef gm("mom_gamemode");
    gm.SetValue(gm.GetDefault());

    g_Timer->OnMapEnd(pMapName);
}

void CMOMServerEvents::FrameUpdatePreEntityThink()
{
    g_MapzoneEdit.Update();

    if (!g_Timer->GotCaughtCheating())
    {
        ConVarRef cheatsRef("sv_cheats");
        if (cheatsRef.GetBool())
        {
            g_Timer->SetCheating(true);
            g_Timer->Stop(false);
        }
    }
}

//Create the 
CMOMServerEvents g_MOMServerEvents("MOMServerEvents");