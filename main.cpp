#include <cstdio>
#include "mem.h"
#include "Vector.h"

// ac_client.exe+637E9 dec esi instruction
// ac_client.exe+637E9 - FF 0E                

int main()
{
	Attach(L"ac_client.exe", L"ac_client.exe");
	uintptr_t base = AttachModuleBase();
	uintptr_t entPtr = base + 0x10F4F4;
	uintptr_t ent = RPC(entPtr, { 0x0 });
	int ammo = ent + 0x150;
	bool on = false;
	while (true)
	{
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			on = !on;
		}
		if (on)
		{
			Nop((BYTE*)(base + 0x637E9), 2);
			printf("[ON] - NOP (DEC ESI)");
		}
		else
		{
			Patch((BYTE*)(base + 0x637E9), (BYTE*)"\xFF\x0E", 2);
			printf("[OFF] - NOP (DEC ESI)");
		}
		Sleep(25);
		system("CLS");
	}

}
