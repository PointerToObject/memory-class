#include <cstdio>
#include "mem.h"

int main()
{
	Attach(L"ac_client.exe", L"ac_client.exe");
	int addr = 0x0086A5B8; //dynamic 
	while (true)
	{
		Write<int>(addr, 500);
		Sleep(150);
	}
}
