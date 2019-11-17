#define PATCH_MAX_BYTES 64
#define PATCH_BYTE_LEN 3
#define PATCH_BYTE_SPLIT "\\x"
#define PATCH_BYTE_SPLIT_LEN 2

enum
{
	MemoryIndex_Address = 0,
	MemoryIndex_PayloadCount,
	MemoryIndex_Patched,
	MemoryIndex_Max
};

static GameData g_hGameData = null;

methodmap MemoryPatch < ArrayList
{
	public static void SetGameData(GameData hConfig)
	{
		if (g_hGameData != null) delete g_hGameData;
		
		g_hGameData = view_as<GameData>(CloneHandle(hConfig));
	}
	
	public MemoryPatch(const char[] sPatchName, GameData hGameData = null)
	{
		if (hGameData == null)
			hGameData = g_hGameData;
		
		if (hGameData == null)
		{
			LogError("Attempted to create a MemoryPatch without gamedata.");
			return null;
		}
		
		ArrayList patch = new ArrayList();
		
		Address address = hGameData.GetAddress(sPatchName);
		if (address == Address_Null)
		{
			LogError("Couldn't retrieve address for MemoryPatch.");
			return null;
		}
		
		char sBytesBuffer[PATCH_MAX_BYTES*PATCH_BYTE_SPLIT_LEN*PATCH_BYTE_LEN+1], sBytes[PATCH_MAX_BYTES][PATCH_BYTE_LEN];
		if (!hGameData.GetKeyValue(sPatchName, sBytesBuffer, sizeof(sBytesBuffer)))
		{
			delete patch;
			LogError("Attempted to create a MemoryPatch with no bytes to patch.");
			return null;
		}
		
		int iPayloadCount = ExplodeString(sBytesBuffer, PATCH_BYTE_SPLIT, sBytes, PATCH_MAX_BYTES, PATCH_BYTE_LEN);
		
		patch.Push(address); // MemoryIndex_Address
		patch.Push(iPayloadCount-1); // MemoryIndex_PayloadCount
		patch.Push(false); // MemoryIndex_Patched
		for (int i = 1; i < iPayloadCount; i++)
		{
			patch.Push(StringToInt(sBytes[i], 16)); // Push the bytes
		}

		return view_as<MemoryPatch>(patch);
	}
	
	property bool bEnabled
	{
		public get()
		{
			return this.Get(MemoryIndex_Patched);
		}
	}
	
	public void Toggle()
	{
		Address address = this.Get(MemoryIndex_Address);
#if defined DEBUG
		LogMessage("Patching address 0x%X...", address);
#endif
		
		for (int i = 0, iMax = this.Get(MemoryIndex_PayloadCount); i < iMax; i++)
		{
			Address byte = this.Get(i+MemoryIndex_Max);
			Address originalByte = view_as<Address>(LoadFromAddress(address+view_as<Address>(i), NumberType_Int8));
			
#if defined DEBUG
			LogMessage("[%i] [%02X] --> [%02X]", i, originalByte, byte);
#endif
			this.Set(i+MemoryIndex_Max, originalByte);
			StoreToAddress(address+view_as<Address>(i), view_as<int>(byte), NumberType_Int8);
		}
	}
	
	public void Enable()
	{
		if (this.bEnabled) return;
		
		this.Toggle();
		this.Set(MemoryIndex_Patched, true);
	}
	
	public void Disable()
	{
		if (!this.bEnabled) return;
		
		this.Toggle();
		this.Set(MemoryIndex_Patched, false);
	}
}