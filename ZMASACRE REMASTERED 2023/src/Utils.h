/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#pragma once


struct								actor_info* actor_info_get(int id, int flags);
struct								vehicle_info* vehicle_info_get(int id, int flags);

void								Main_Hook();
void								renderSAMP();
void								RenderGUI();

void								CalcScreenCoors(D3DXVECTOR3* vecWorld, D3DXVECTOR3* vecScreen);

void								RenderSpecialFuncs();

bool								isBadPtr_writeAny(void* pointer, ULONG size);
bool								isBadPtr_readAny(void* pointer, ULONG size);
void								DisembarkInstantly();

int									memcpy_safe(void* dest, const void* src, uint32_t len, int check = NULL, const void* checkdata = NULL);
int									memset_safe(void* _dest, int c, uint32_t len);
int									memcmp_safe(const void* _s1, const void* _s2, uint32_t len);

void*								dll_baseptr_get(const char* dll_name);

size_t								strlcpy(char* dst, const char* src, size_t size);
size_t								strlcat(char* dst, const char* src, size_t size);

bool								findstrinstr(char* text, char* find);

void*								memdup(const void* src, int len);
uint8_t*							hex_to_bin(const char* str);
bool								hex_is_valid(const std::string& hex);

bool								IsPlayerFriend(uint16_t playerID);
bool								IsPlayerEnemy(uint16_t playerID);
bool								IsPlayerConnected(uint16_t playerID);
bool								IsPlayerStreamed(uint16_t playerID);
bool								IsPlayerInVehicle();
bool								IsChatOpened();

bool								isInsideCircle(float circle_x, float circle_y, float rad, float x, float y);

void								vehicle_setColor0(vehicle_info* vinfo, int new_color);
void								vehicle_setColor1(vehicle_info* vinfo, int new_color);

DWORD								rainbow();
D3DCOLOR							RandomColorz();

const std::string					currentDateTime();

void								GTAfunc_requestModelLoad(int iModelID);
int									GTAfunc_isModelLoaded(int iModelID);
void								GTAfunc_loadRequestedModels(void);

void								spectateHandle(void);
void								spectatePlayer(int iPlayerID);
void								spectatePlayeroff(int iPlayerID);

int									GetAimingPlayer();

void								update_translateGTASAMP_vehiclePool(void);
void								update_translateGTASAMP_pedPool(void);

int									isBadPtr_GTA_pPed(CPed* pCPed);

void								PressKey(DWORD Key);

int									gta_menu_active(void);

D3DXVECTOR3							CVecToD3DXVEC(CVector vec);
float								CalculateDamage(void);
void								process(void*);
void								GetAimingCenter(float& x, float& y);

int									getPedGTAIDFromInterface(DWORD* ped);

char*								dumb_hex_dump(void* data, uint32_t len);
int									patcher_install(struct patch_set* set);
int									patcher_remove(struct patch_set* set);
int									is_patch_installed(struct patch_set* patch);
void								patcher_free(struct patch_set* set);
int									GTAPatchIDFinder(DWORD patchChunk0Ptr);
int									sampPatchDisableNameTags(int iEnabled);

const struct vehicle_entry*			gta_vehicle_get_by_id(int id);

void								cheat_teleport(const float pos[3], int interior_id);
void								cheat_actor_teleport(struct actor_info* info, const float pos[3], int interior_id);

void								Discord_Ýnit();
