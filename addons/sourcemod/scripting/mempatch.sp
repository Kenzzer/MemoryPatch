#define DEBUG

#include <sourcemod>
#include <memorypatch>

MemoryPatch g_hPickupWepPatch;

public void OnPluginStart()
{
	GameData hData = new GameData("mem");
	MemoryPatch.SetGameData(hData);
	
	g_hPickupWepPatch = new MemoryPatch("Patch_PickupWeaponFromOther");
	if (g_hPickupWepPatch != null)
		g_hPickupWepPatch.Enable();
	
	delete hData;
}

public void OnPluginEnd()
{
	g_hPickupWepPatch.Disable();
}