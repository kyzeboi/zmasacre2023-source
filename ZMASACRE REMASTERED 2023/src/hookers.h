/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#include "main.h"

// ---------------------------------------------------
void _cdecl CVehicle_constructor_hook(CVehicleSAInterface* vehicle)
{
	// create & add new CVehicle to CPools
	CVehicle* CVeh = pPools->AddVehicle((DWORD*)vehicle);
}

#define HOOKPOS_CVehicle_constructor	0x6D6259
DWORD	RETURN_CVehicle_constructor = 0x6D6263;
void _declspec (naked) HOOK_CVehicle_constructor()
{
	__asm
	{
		// perform over-written asm
		mov fs : 0, ecx
		add esp, 0x10

		// call handler
		push eax
		call CVehicle_constructor_hook
		add esp, 0x4

		// return
		jmp RETURN_CVehicle_constructor
	}
}

void _cdecl CVehicle_destructor_hook(CVehicleSAInterface* vehicle)
{
	// remove CVehicle from CPools and delete
	CVehicle* CVeh_toDelete = pPools->GetVehicle((DWORD*)vehicle);
	pPools->RemoveVehicle(CVeh_toDelete, false);
}

#define HOOKPOS_CVehicle_destructor 0x6E2B40
DWORD	RETURN_CVehicle_destructor = 0x6E2B47;
void _declspec (naked) HOOK_CVehicle_destructor()
{
	__asm
	{
		// call handler
		push ecx
		call CVehicle_destructor_hook
		pop ecx

		// perform over-written asm
		push 0x0FFFFFFFF
		push 0x848826

		// return
		jmp RETURN_CVehicle_destructor
	}
}

// ---------------------------------------------------
void _cdecl CPed_constructor_hook(CPedSAInterface* ped)
{
	pPools->AddPed((DWORD*)ped);
}

#define HOOKPOS_CPed_constructor	0x5E8606
DWORD	RETURN_CPed_constructor = 0x5E8610;
void _declspec (naked) HOOK_CPed_constructor()
{
	__asm
	{
		// perform over-written asm
		mov fs : 0, ecx
		add esp, 0x28

		// call handler
		push eax
		call CPed_constructor_hook

		//pop eax
		add esp, 0x4

		// return
		jmp RETURN_CPed_constructor
	}
}

void _cdecl CPed_destructor_hook(CPedSAInterface* ped)
{
	// remove CPed from CPools and delete
	CPed* CPed_toDelete = pPools->GetPed((DWORD*)ped);
	pPools->RemovePed(CPed_toDelete, false);
}

#define HOOKPOS_CPed_destructor 0x5E8620
DWORD	RETURN_CPed_destructor = 0x5E8627;
void _declspec (naked) HOOK_CPed_destructor()
{
	__asm
	{
		// call handler
		push ecx
		call CPed_destructor_hook
		pop ecx

		// perform over-written asm
		push 0x0FFFFFFFF
		push 0x83DA5A

		// return
		jmp RETURN_CPed_destructor
	}
}

void cheat_hookers_installhooks()
{
	HookInstall(HOOKPOS_CVehicle_constructor, (DWORD)HOOK_CVehicle_constructor, 6);
	HookInstall(HOOKPOS_CVehicle_destructor, (DWORD)HOOK_CVehicle_destructor, 6);
	HookInstall(HOOKPOS_CPed_constructor, (DWORD)HOOK_CPed_constructor, 6);
	HookInstall(HOOKPOS_CPed_destructor, (DWORD)HOOK_CPed_destructor, 6);
}
