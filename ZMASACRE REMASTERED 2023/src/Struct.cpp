/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/


#include "main.h"

// GTA_SA LIBRARY EXTENSION
CGameSA*								pGameInterface = NULL;
CPools*									pPools = NULL;
CPed*									pPedSelf = NULL;
CPedSAInterface*						pPedSelfSA = NULL;

struct pool*							pool_actor;
struct pool*							pool_vehicle;

stTranslateGTASAMP_vehiclePool			translateGTASAMP_vehiclePool;
stTranslateGTASAMP_pedPool				translateGTASAMP_pedPool;

char									g_szWorkingDirectory[MAX_PATH];
uint32_t								g_dwSAMP_Addr = NULL;

std::unique_ptr <nLogger>				logger;

CMenu*									ÝmMenu;

CD3DRender*								render = new CD3DRender(128);

CD3DFont*								pD3DFont = new CD3DFont("Tahoma", 10, FCR_BORDER);
CD3DFont*								pD3DFontFixedSmall = new CD3DFont("Small Fonts", 8, FCR_BORDER);
CD3DFont*								pD3DFontCheatState = new CD3DFont("Display", 10, FCR_BORDER);
CD3DFont*								pD3DFontWeapon = new CD3DFont("GTAWEAPON3", 30, FCR_BORDER);

stNetPatch								netPatch[INI_NETPATCHES_MAX];
stNetPatch*								netPatchAssoc[RAKNET_MAX_PACKET][4];

struct patch_set						patch[INI_PATCHES_MAX];

int										player_state = 0;

int										g_iSpectateEnabled = 0,
										g_iSpectateLock = 0,
										g_iSpectatePlayerID = -1;

// No Wanted Config
int										aimedplayer;

/// <serverside>
bool									kayitliserver;
bool									girisyapildi;
/// <serverside>

// Bullet 
bool									silentaim;
int										silent_body_type = 0;
bool									RangeShot;
bool									FakeBullet;
bool									NoReload;
bool									autoaim;
int										aimbot_type = 0;

// Player
bool									ActorSlapper;
bool									antistun;
bool									NoCollisionPlayerss;
bool									ActorUPInv;
bool									afk;
bool									antics;
bool									box;
bool									trace;
bool									render_player_t4gs;

int										trace_type = 0;

// Vehicle
int										car_color = 0;
int										random_car_color_id = 0;
int										car_health = 100;
bool									NoCollisionVehicles;
bool									VehiclePosInv;
bool									CarSlapper;
bool									CarDeformer;

// Other
bool									Discord_RPC;
bool									HasarYedimTick;
bool									ShowEnemyPanel;

// Chosen
int										SlapperType = 0;
bool									PlayerHaveMyColor;
bool									OnlyEnemyFilter;
bool									IgnoreOnfoot;
bool									FakeLag;
bool									Ignore2Levels;
bool									IgnoreUnder10Levels;
bool									IgnoreDriver;

const struct weapon_entry				weapon_list[] =
{
	{ 0, 0, -1, "Fist" },
	{ 1, 0, 331, "Brass Knuckles" },
	{ 2, 1, 333, "Golf Club" },
	{ 3, 1, 334, "Nitestick" },
	{ 4, 1, 335, "Knife" },
	{ 5, 1, 336, "Baseball Bat" },
	{ 6, 1, 337, "Shovel" },
	{ 7, 1, 338, "Pool Cue" },
	{ 8, 1, 339, "Katana" },
	{ 9, 1, 341, "Chainsaw" },
	{ 22, 2, 346, "PISTOL" },
	{ 23, 2, 347, "PISTOL" },
	{ 24, 2, 348, "DEAGLE" },
	{ 25, 3, 349, "SHOTGUN" },
	{ 26, 3, 350, "SAWN-OFF" },
	{ 27, 3, 351, "SPAZ-12" },
	{ 28, 4, 352, "UZI" },
	{ 29, 4, 353, "MP-5" },
	{ 32, 4, 372, "TECH-9" },
	{ 30, 5, 355, "AK-47" },
	{ 31, 5, 356, "M4" },
	{ 33, 6, 357, "RIFLE" },
	{ 34, 6, 358, "SNIPER" },
	{ 35, 7, 359, "ROCKET" },
	{ 36, 7, 360, "Heat Seeking RPG" },
	{ 37, 7, 361, "Flame Thrower" },
	{ 38, 7, 362, "MINIGUN" },
	{ 16, 8, 342, "Grenade" },
	{ 17, 8, 343, "Teargas" },
	{ 18, 8, 344, "Molotov Cocktail" },
	{ 39, 8, 363, "Remote Explosives" },
	{ 41, 9, 365, "Spray Can" },
	{ 42, 9, 366, "Fire Extinguisher" },
	{ 43, 9, 367, "Camera" },
	{ 10, 10, 321, "Dildo 1" },
	{ 11, 10, 322, "Dildo 2" },
	{ 12, 10, 323, "Vibe 1" },
	{ 13, 10, 324, "Vibe 2" },
	{ 14, 10, 325, "Flowers" },
	{ 15, 10, 326, "Cane" },
	{ 44, 11, 368, "NV Goggles" },
	{ 45, 11, 369, "IR Goggles" },
	{ 46, 11, 371, "Parachute" },
	{ 40, 12, 364, "Detonator" },
	{ -1, -1, -1, NULL }
};

/* IDs must be sequential */
const struct vehicle_entry	vehicle_list[VEHICLE_LIST_SIZE] =
{	// model_id, vehicle class, vehicle name, passenger seats
	{ 400, VEHICLE_CLASS_CAR, "Landstalker", 3 },
	{ 401, VEHICLE_CLASS_CAR, "Bravura", 1 },
	{ 402, VEHICLE_CLASS_CAR_FAST, "Buffalo", 1 },
	{ 403, VEHICLE_CLASS_HEAVY, "Linerunner", 1 },
	{ 404, VEHICLE_CLASS_CAR, "Perennial", 3 },
	{ 405, VEHICLE_CLASS_CAR, "Sentinel", 3 },
	{ 406, VEHICLE_CLASS_HEAVY, "Dumper", 0 },
	{ 407, VEHICLE_CLASS_HEAVY, "Fire Truck", 1 },
	{ 408, VEHICLE_CLASS_HEAVY, "Trashmaster", 1 },
	{ 409, VEHICLE_CLASS_HEAVY, "Stretch", 3 },
	{ 410, VEHICLE_CLASS_CAR, "Manana", 1 },
	{ 411, VEHICLE_CLASS_CAR_FAST, "Infernus", 1 },
	{ 412, VEHICLE_CLASS_CAR, "Voodoo", 1 },
	{ 413, VEHICLE_CLASS_CAR, "Pony", 3 },
	{ 414, VEHICLE_CLASS_CAR, "Mule", 1 },
	{ 415, VEHICLE_CLASS_CAR_FAST, "Cheetah", 1 },
	{ 416, VEHICLE_CLASS_HEAVY, "Ambulance", 3 },
	{ 417, VEHICLE_CLASS_HELI, "Leviathan", 1 },
	{ 418, VEHICLE_CLASS_CAR, "Moonbeam", 3 },
	{ 419, VEHICLE_CLASS_CAR, "Esperanto", 1 },
	{ 420, VEHICLE_CLASS_CAR, "Taxi", 3 },
	{ 421, VEHICLE_CLASS_CAR, "Washington", 3 },
	{ 422, VEHICLE_CLASS_CAR, "Bobcat", 1 },
	{ 423, VEHICLE_CLASS_HEAVY, "Mr. Whoopee", 1 },
	{ 424, VEHICLE_CLASS_CAR, "BF Injection", 1 },
	{ 425, VEHICLE_CLASS_HELI, "Hunter", 0 },
	{ 426, VEHICLE_CLASS_CAR, "Premier", 3 },
	{ 427, VEHICLE_CLASS_HEAVY, "Enforcer", 3 },
	{ 428, VEHICLE_CLASS_HEAVY, "Securicar", 3 },
	{ 429, VEHICLE_CLASS_CAR_FAST, "Banshee", 1 },
	{ 430, VEHICLE_CLASS_BOAT, "Predator", 0 },
	{ 431, VEHICLE_CLASS_HEAVY, "Bus", 7 },
	{ 432, VEHICLE_CLASS_HEAVY, "Rhino", 0 },

	/* tank... */
	{ 433, VEHICLE_CLASS_HEAVY, "Barracks", 1 },
	{ 434, VEHICLE_CLASS_CAR_FAST, "Hotknife", 1 },
	{ 435, VEHICLE_CLASS_TRAILER, "Artict Trailer", 0 },
	{ 436, VEHICLE_CLASS_CAR, "Previon", 1 },
	{ 437, VEHICLE_CLASS_HEAVY, "Coach", 7 },
	{ 438, VEHICLE_CLASS_CAR, "Cabbie", 3 },
	{ 439, VEHICLE_CLASS_CAR_FAST, "Stallion", 1 },
	{ 440, VEHICLE_CLASS_CAR, "Rumpo", 3 },
	{ 441, VEHICLE_CLASS_MINI, "RC Bandit", 0 },
	{ 442, VEHICLE_CLASS_CAR, "Romero", 1 },
	{ 443, VEHICLE_CLASS_HEAVY, "Packer", 1 },
	{ 444, VEHICLE_CLASS_HEAVY, "Monster", 1 },
	{ 445, VEHICLE_CLASS_CAR, "Admiral", 3 },
	{ 446, VEHICLE_CLASS_BOAT, "Squalo", 0 },
	{ 447, VEHICLE_CLASS_HELI, "Seasparrow", 1 },
	{ 448, VEHICLE_CLASS_BIKE, "Pizza Boy", 0 },

	// needs to be researched to find actual max passengers in SA:MP
	{ 449, VEHICLE_CLASS_HEAVY, "Trolly", 1 },

	/* train... */
	{ 450, VEHICLE_CLASS_TRAILER, "Artict Trailer 2", 0 },
	{ 451, VEHICLE_CLASS_CAR_FAST, "Turismo", 1 },
	{ 452, VEHICLE_CLASS_BOAT, "Speeder", 0 },
	{ 453, VEHICLE_CLASS_BOAT, "Reefer", 0 },
	{ 454, VEHICLE_CLASS_BOAT, "Tropic", 0 },
	{ 455, VEHICLE_CLASS_HEAVY, "Flatbed", 1 },
	{ 456, VEHICLE_CLASS_HEAVY, "Yankee", 1 },
	{ 457, VEHICLE_CLASS_MINI, "Caddy", 1 },
	{ 458, VEHICLE_CLASS_CAR, "Solair", 3 },
	{ 459, VEHICLE_CLASS_HEAVY, "Berkley's RC Van", 3 },
	{ 460, VEHICLE_CLASS_AIRPLANE, "Skimmer", 1 },
	{ 461, VEHICLE_CLASS_BIKE, "PCJ-600", 1 },
	{ 462, VEHICLE_CLASS_BIKE, "Faggio", 1 },
	{ 463, VEHICLE_CLASS_BIKE, "Freeway", 1 },
	{ 464, VEHICLE_CLASS_MINI, "RC Baron", 0 },
	{ 465, VEHICLE_CLASS_MINI, "RC Raider", 0 },
	{ 466, VEHICLE_CLASS_CAR, "Glendale", 3 },
	{ 467, VEHICLE_CLASS_CAR, "Oceanic", 3 },
	{ 468, VEHICLE_CLASS_BIKE, "Sanchez", 1 },
	{ 469, VEHICLE_CLASS_HELI, "Sparrow", 1 },
	{ 470, VEHICLE_CLASS_CAR, "Patriot", 3 },
	{ 471, VEHICLE_CLASS_BIKE, "Quadbike", 1 },

	/* sort of.. */
	{ 472, VEHICLE_CLASS_BOAT, "Coastguard", 0 },
	{ 473, VEHICLE_CLASS_BOAT, "Dinghy", 0 },
	{ 474, VEHICLE_CLASS_CAR, "Hermes", 1 },
	{ 475, VEHICLE_CLASS_CAR, "Sabre", 1 },
	{ 476, VEHICLE_CLASS_AIRPLANE, "Rustler", 0 },
	{ 477, VEHICLE_CLASS_CAR_FAST, "ZR-350", 1 },
	{ 478, VEHICLE_CLASS_CAR, "Walton", 1 },
	{ 479, VEHICLE_CLASS_CAR, "Regina", 3 },
	{ 480, VEHICLE_CLASS_CAR_FAST, "Comet", 1 },
	{ 481, VEHICLE_CLASS_BIKE, "BMX", 0 },
	{ 482, VEHICLE_CLASS_HEAVY, "Burrito", 3 },

	// more research on this, the side door might allow 2 passengers
	{ 483, VEHICLE_CLASS_HEAVY, "Camper", 2 },
	{ 484, VEHICLE_CLASS_BOAT, "Marquis", 0 },
	{ 485, VEHICLE_CLASS_MINI, "Baggage", 0 },
	{ 486, VEHICLE_CLASS_HEAVY, "Dozer", 0 },
	{ 487, VEHICLE_CLASS_HELI, "Maverick", 3 },
	{ 488, VEHICLE_CLASS_HELI, "News Chopper", 1 },
	{ 489, VEHICLE_CLASS_CAR, "Rancher", 1 },
	{ 490, VEHICLE_CLASS_CAR, "FBI Rancher", 3 },
	{ 491, VEHICLE_CLASS_CAR, "Virgo", 1 },
	{ 492, VEHICLE_CLASS_CAR, "Greenwood", 3 },
	{ 493, VEHICLE_CLASS_BOAT, "Jetmax", 0 },
	{ 494, VEHICLE_CLASS_CAR_FAST, "Hotring Racer", 1 },
	{ 495, VEHICLE_CLASS_CAR, "Sandking", 1 },
	{ 496, VEHICLE_CLASS_CAR, "Blista Compact", 1 },
	{ 497, VEHICLE_CLASS_HELI, "Police Maverick", 3 },
	{ 498, VEHICLE_CLASS_HEAVY, "Boxville", 3 },
	{ 499, VEHICLE_CLASS_HEAVY, "Benson", 1 },
	{ 500, VEHICLE_CLASS_CAR, "Mesa", 1 },
	{ 501, VEHICLE_CLASS_MINI, "RC Goblin", 0 },
	{ 502, VEHICLE_CLASS_CAR_FAST, "Hotring Racer 2", 1 },
	{ 503, VEHICLE_CLASS_CAR_FAST, "Hotring Racer 3", 1 },
	{ 504, VEHICLE_CLASS_CAR_FAST, "Bloodring Banger", 1 },
	{ 505, VEHICLE_CLASS_CAR, "Rancher", 1 },
	{ 506, VEHICLE_CLASS_CAR_FAST, "Super GT", 1 },
	{ 507, VEHICLE_CLASS_CAR, "Elegant", 3 },
	{ 508, VEHICLE_CLASS_HEAVY, "Journey", 1 },
	{ 509, VEHICLE_CLASS_BIKE, "Bike", 0 },
	{ 510, VEHICLE_CLASS_BIKE, "Mountain Bike", 0 },
	{ 511, VEHICLE_CLASS_AIRPLANE, "Beagle", 1 },
	{ 512, VEHICLE_CLASS_AIRPLANE, "Cropduster", 0 },
	{ 513, VEHICLE_CLASS_AIRPLANE, "Stuntplane", 0 },
	{ 514, VEHICLE_CLASS_HEAVY, "Tanker", 1 },

	/* semi truck */
	{ 515, VEHICLE_CLASS_HEAVY, "Roadtrain", 1 },

	/* semi truck */
	{ 516, VEHICLE_CLASS_CAR, "Nebula", 3 },
	{ 517, VEHICLE_CLASS_CAR, "Majestic", 1 },
	{ 518, VEHICLE_CLASS_CAR, "Buccaneer", 1 },
	{ 519, VEHICLE_CLASS_AIRPLANE, "Shamal", 0 },
	{ 520, VEHICLE_CLASS_AIRPLANE, "Hydra", 0 },
	{ 521, VEHICLE_CLASS_BIKE, "FCR-900", 1 },
	{ 522, VEHICLE_CLASS_BIKE, "NRG-500", 1 },
	{ 523, VEHICLE_CLASS_BIKE, "HPV1000", 1 },
	{ 524, VEHICLE_CLASS_HEAVY, "Cement Truck", 1 },
	{ 525, VEHICLE_CLASS_HEAVY, "Towtruck", 1 },
	{ 526, VEHICLE_CLASS_CAR, "Fortune", 1 },
	{ 527, VEHICLE_CLASS_CAR, "Cadrona", 1 },
	{ 528, VEHICLE_CLASS_HEAVY, "FBI Truck", 1 },
	{ 529, VEHICLE_CLASS_CAR, "Willard", 3 },
	{ 530, VEHICLE_CLASS_MINI, "Forklift", 0 },
	{ 531, VEHICLE_CLASS_CAR, "Tractor", 0 },
	{ 532, VEHICLE_CLASS_HEAVY, "Combine Harvester", 0 },
	{ 533, VEHICLE_CLASS_CAR, "Feltzer", 1 },
	{ 534, VEHICLE_CLASS_CAR, "Remington", 1 },
	{ 535, VEHICLE_CLASS_CAR_FAST, "Slamvan", 1 },
	{ 536, VEHICLE_CLASS_CAR_FAST, "Blade", 1 },
	{ 537, VEHICLE_CLASS_HEAVY, "Freight", 1 },

	/* train engine... */
	{ 538, VEHICLE_CLASS_HEAVY, "Brown Streak Engine", 1 },

	/* train engine... */
	{ 539, VEHICLE_CLASS_BOAT, "Vortex", 0 },
	{ 540, VEHICLE_CLASS_CAR, "Vincent", 3 },
	{ 541, VEHICLE_CLASS_CAR_FAST, "Bullet", 1 },
	{ 542, VEHICLE_CLASS_CAR, "Clover", 1 },
	{ 543, VEHICLE_CLASS_CAR, "Sadler", 1 },
	{ 544, VEHICLE_CLASS_HEAVY, "Fire Truck with ladder", 1 },
	{ 545, VEHICLE_CLASS_CAR, "Hustler", 1 },
	{ 546, VEHICLE_CLASS_CAR, "Intruder", 3 },
	{ 547, VEHICLE_CLASS_CAR, "Primo", 3 },
	{ 548, VEHICLE_CLASS_HELI, "Cargobob", 1 },
	{ 549, VEHICLE_CLASS_CAR, "Tampa", 1 },
	{ 550, VEHICLE_CLASS_CAR, "Sunrise", 3 },
	{ 551, VEHICLE_CLASS_CAR, "Merit", 3 },
	{ 552, VEHICLE_CLASS_HEAVY, "Utility Van", 1 },
	{ 553, VEHICLE_CLASS_AIRPLANE, "Nevada", 0 },
	{ 554, VEHICLE_CLASS_CAR, "Yosemite", 1 },
	{ 555, VEHICLE_CLASS_CAR, "Windsor", 1 },
	{ 556, VEHICLE_CLASS_HEAVY, "Monster 2", 1 },
	{ 557, VEHICLE_CLASS_HEAVY, "Monster 3", 1 },
	{ 558, VEHICLE_CLASS_CAR_FAST, "Uranus", 1 },
	{ 559, VEHICLE_CLASS_CAR_FAST, "Jester", 1 },
	{ 560, VEHICLE_CLASS_CAR_FAST, "Sultan", 3 },
	{ 561, VEHICLE_CLASS_CAR, "Stratum", 3 },
	{ 562, VEHICLE_CLASS_CAR_FAST, "Elegy", 1 },
	{ 563, VEHICLE_CLASS_HELI, "Raindance", 1 },
	{ 564, VEHICLE_CLASS_MINI, "RC Tiger", 0 },
	{ 565, VEHICLE_CLASS_CAR, "Flash", 1 },
	{ 566, VEHICLE_CLASS_CAR, "Tahoma", 3 },
	{ 567, VEHICLE_CLASS_CAR, "Savanna", 3 },
	{ 568, VEHICLE_CLASS_CAR_FAST, "Bandito", 0 },
	{ 569, VEHICLE_CLASS_HEAVY, "Freight Train Flatbed", 0 },

	/* train car... */
	{ 570, VEHICLE_CLASS_HEAVY, "Brown Streak", 0 },

	/* train car... XXX dupe, streakc */
	{ 571, VEHICLE_CLASS_MINI, "Kart", 0 },
	{ 572, VEHICLE_CLASS_MINI, "Mower", 0 },
	{ 573, VEHICLE_CLASS_HEAVY, "Dune", 1 },
	{ 574, VEHICLE_CLASS_MINI, "Sweeper", 0 },
	{ 575, VEHICLE_CLASS_CAR, "Broadway", 1 },
	{ 576, VEHICLE_CLASS_CAR, "Tornado", 1 },
	{ 577, VEHICLE_CLASS_AIRPLANE, "AT-400", 0 },
	{ 578, VEHICLE_CLASS_HEAVY, "DFT-30", 1 },

	/* large flat-bed truck */
	{ 579, VEHICLE_CLASS_CAR, "Huntley", 3 },
	{ 580, VEHICLE_CLASS_CAR, "Stafford", 1 },
	{ 581, VEHICLE_CLASS_BIKE, "BF-400", 1 },
	{ 582, VEHICLE_CLASS_HEAVY, "News Van", 3 },
	{ 583, VEHICLE_CLASS_MINI, "Tug", 0 },
	{ 584, VEHICLE_CLASS_TRAILER, "Petrol Trailer", 0 },
	{ 585, VEHICLE_CLASS_CAR, "Emperor", 3 },
	{ 586, VEHICLE_CLASS_BIKE, "Wayfarer", 1 },
	{ 587, VEHICLE_CLASS_CAR_FAST, "Euros", 1 },
	{ 588, VEHICLE_CLASS_HEAVY, "Hotdog", 1 },
	{ 589, VEHICLE_CLASS_CAR, "Club", 1 },
	{ 590, VEHICLE_CLASS_HEAVY, "Freight Train Boxcar", 0 },

	/* train car... */
	{ 591, VEHICLE_CLASS_TRAILER, "Artict Trailer 3", 0 },
	{ 592, VEHICLE_CLASS_AIRPLANE, "Andromada", 0 },
	{ 593, VEHICLE_CLASS_AIRPLANE, "Dodo", 1 },
	{ 594, VEHICLE_CLASS_MINI, "RC Cam", 0 },
	{ 595, VEHICLE_CLASS_BOAT, "Launch", 0 },
	{ 596, VEHICLE_CLASS_CAR, "Police Car (LS)", 3 },
	{ 597, VEHICLE_CLASS_CAR, "Police Car (SF)", 3 },
	{ 598, VEHICLE_CLASS_CAR, "Police Car (LV)", 3 },
	{ 599, VEHICLE_CLASS_CAR, "Police Ranger", 1 },
	{ 600, VEHICLE_CLASS_CAR, "Picador", 1 },
	{ 601, VEHICLE_CLASS_HEAVY, "S.W.A.T.", 1 },
	{ 602, VEHICLE_CLASS_CAR_FAST, "Alpha", 1 },
	{ 603, VEHICLE_CLASS_CAR_FAST, "Phoenix", 1 },
	{ 604, VEHICLE_CLASS_CAR, "Damaged Glendale", 3 },
	{ 605, VEHICLE_CLASS_CAR, "Damaged Sadler", 1 },
	{ 606, VEHICLE_CLASS_TRAILER, "Baggage Trailer", 0 },
	{ 607, VEHICLE_CLASS_TRAILER, "Baggage Trailer 2", 0 },
	{ 608, VEHICLE_CLASS_TRAILER, "Tug Staircase", 0 },
	{ 609, VEHICLE_CLASS_HEAVY, "Black Boxville", 3 },
	{ 610, VEHICLE_CLASS_TRAILER, "Farm Trailer", 0 },
	{ 611, VEHICLE_CLASS_TRAILER, "Street Sweeper Trailer", 0 }
};

const struct interiors					interiors_list[] =
{
	{ 11, 2003.1178f, 1015.1948f, 33.008f, 351.5789f, "Four Dragons' Managerial Suite" },
	{ 5, 770.8033f, -0.7033f, 1000.7267f, 22.8599f, "Ganton Gym" },
	{ 3, 974.0177f, -9.5937f, 1001.1484f, 22.6045f, "Brothel" },
	{ 3, 961.9308f, -51.9071f, 1001.1172f, 95.5381f, "Brothel2" },
	{ 3, 830.6016f, 5.9404f, 1004.1797f, 125.8149f, "Inside Track Betting" },
	{ 3, 1037.8276f, 0.397f, 1001.2845f, 353.9335f, "Blastin' Fools Records" },
	{ 3, 1212.1489f, -28.5388f, 1000.9531f, 170.5692f, "The Big Spread Ranch" },
	{ 18, 1290.4106f, 1.9512f, 1001.0201f, 179.9419f, "Warehouse 1" },
	{ 1, 1412.1472f, -2.2836f, 1000.9241f, 114.661f, "Warehouse 2" },
	{ 3, 1527.0468f, -12.0236f, 1002.0971f, 350.0013f, "B Dup's Apartment" },
	{ 2, 1523.5098f, -47.8211f, 1002.2699f, 262.7038f, "B Dup's Crack Palace" },
	{ 3, 612.2191f, -123.9028f, 997.9922f, 266.5704f, "Wheel Arch Angels" },
	{ 3, 512.9291f, -11.6929f, 1001.5653f, 198.7669f, "OG Loc's House" },
	{ 3, 418.4666f, -80.4595f, 1001.8047f, 343.2358f, "Barber Shop" },
	{ 3, 386.5259f, 173.6381f, 1008.3828f, 63.7399f, "Planning Department" },
	{ 3, 288.4723f, 170.0647f, 1007.1794f, 22.0477f, "Las Venturas Police Department" },
	{ 3, 206.4627f, -137.7076f, 1003.0938f, 10.9347f, "Pro-Laps" },
	{ 3, -100.2674f, -22.9376f, 1000.7188f, 17.285f, "Sex Shop" },
	{ 3, -201.2236f, -43.2465f, 1002.2734f, 45.8613f, "Las Venturas Tattoo parlor" },
	{ 17, -202.9381f, -6.7006f, 1002.2734f, 204.2693f, "Lost San Fierro Tattoo parlor" },
	{ 17, -25.7220f, -187.8216f, 1003.5469f, 5.0760f, "24/7 (version 1)" },
	{ 5, 454.9853f, -107.2548f, 999.4376f, 309.0195f, "Diner 1" },
	{ 5, 372.5565f, -131.3607f, 1001.4922f, 354.2285f, "Pizza Stack" },
	{ 17, 378.026f, -190.5155f, 1000.6328f, 141.0245f, "Rusty Brown's Donuts" },
	{ 7, 315.244f, -140.8858f, 999.6016f, 7.4226f, "Ammu-nation" },
	{ 5, 225.0306f, -9.1838f, 1002.218f, 85.5322f, "Victim" },
	{ 2, 611.3536f, -77.5574f, 997.9995f, 320.9263f, "Loco Low Co" },
	{ 10, 246.0688f, 108.9703f, 1003.2188f, 0.2922f, "San Fierro Police Department" },
	{ 10, 6.0856f, -28.8966f, 1003.5494f, 5.0365f, "24/7 (version 2 - large)" },
	{ 7, 773.7318f, -74.6957f, 1000.6542f, 5.2304f, "Below The Belt Gym (Las Venturas)" },
	{ 1, 621.4528f, -23.7289f, 1000.9219f, 15.6789f, "Transfenders" },
	{ 1, 445.6003f, -6.9823f, 1000.7344f, 172.2105f, "World of Coq" },
	{ 1, 285.8361f, -39.0166f, 1001.5156f, 0.7529f, "Ammu-nation (version 2)" },
	{ 1, 204.1174f, -46.8047f, 1001.8047f, 357.5777f, "SubUrban" },
	{ 1, 245.2307f, 304.7632f, 999.1484f, 273.4364f, "Denise's Bedroom" },
	{ 3, 290.623f, 309.0622f, 999.1484f, 89.9164f, "Helena's Barn" },
	{ 5, 322.5014f, 303.6906f, 999.1484f, 8.1747f, "Barbara's Love nest" },
	{ 1, -2041.2334f, 178.3969f, 28.8465f, 156.2153f, "San Fierro Garage" },
	{ 1, -1402.6613f, 106.3897f, 1032.2734f, 105.1356f, "Oval Stadium" },
	{ 7, -1403.0116f, -250.4526f, 1043.5341f, 355.8576f, "8-Track Stadium" },
	{ 2, 1204.6689f, -13.5429f, 1000.9219f, 350.0204f, "The Pig Pen (strip club 2)" },
	{ 10, 2016.1156f, 1017.1541f, 996.875f, 88.0055f, "Four Dragons" },
	{ 1, -741.8495f, 493.0036f, 1371.9766f, 71.7782f, "Liberty City" },
	{ 2, 2447.8704f, -1704.4509f, 1013.5078f, 314.5253f, "Ryder's house" },
	{ 1, 2527.0176f, -1679.2076f, 1015.4986f, 260.9709f, "Sweet's House" },
	{ 10, -1129.8909f, 1057.5424f, 1346.4141f, 274.5268f, "RC Battlefield" },
	{ 3, 2496.0549f, -1695.1749f, 1014.7422f, 179.2174f, "The Johnson House" },
	{ 10, 366.0248f, -73.3478f, 1001.5078f, 292.0084f, "Burger shot" },
	{ 1, 2233.9363f, 1711.8038f, 1011.6312f, 184.3891f, "Caligula's Casino" },
	{ 2, 269.6405f, 305.9512f, 999.1484f, 215.6625f, "Katie's Lovenest" },
	{ 2, 414.2987f, -18.8044f, 1001.8047f, 41.4265f, "Barber Shop 2 (Reece's)" },
	{ 2, 1.1853f, -3.2387f, 999.4284f, 87.5718f, "Angel \"Pine Trailer\"" },
	{ 18, -30.9875f, -89.6806f, 1003.5469f, 359.8401f, "24/7 (version 3)" },
	{ 18, 161.4048f, -94.2416f, 1001.8047f, 0.7938f, "Zip" },
	{ 3, -2638.8232f, 1407.3395f, 906.4609f, 94.6794f, "The Pleasure Domes" },
	{ 5, 1267.8407f, -776.9587f, 1091.9063f, 231.3418f, "Madd Dogg's Mansion" },
	{ 2, 2536.5322f, -1294.8425f, 1044.125f, 254.9548f, "Big Smoke's Crack Palace" },
	{ 5, 2350.1597f, -1181.0658f, 1027.9766f, 99.1864f, "Burning Desire Building" },
	{ 1, -2158.6731f, 642.09f, 1052.375f, 86.5402f, "Wu-Zi Mu's" },
	{ 10, 419.8936f, 2537.1155f, 10.0f, 67.6537f, "Abandoned AC tower" },
	{ 14, 256.9047f, -41.6537f, 1002.0234f, 85.8774f, "Wardrobe/Changing room" },
	{ 14, 204.1658f, -165.7678f, 1000.5234f, 181.7583f, "Didier Sachs" },
	{ 12, 1133.35f, -7.8462f, 1000.6797f, 165.8482f, "Casino (Redsands West)" },
	{ 14, -1420.4277f, 1616.9221f, 1052.5313f, 159.1255f, "Kickstart Stadium" },
	{ 17, 493.1443f, -24.2607f, 1000.6797f, 356.9864f, "Club" },
	{ 18, 1727.2853f, -1642.9451f, 20.2254f, 172.4193f, "Atrium" },
	{ 16, -202.842f, -24.0325f, 1002.2734f, 252.8154f, "Los Santos Tattoo Parlor" },
	{ 5, 2233.6919f, -1112.8107f, 1050.8828f, 8.6483f, "Safe House group 1" },
	{ 6, 1211.2484f, 1049.0234f, 359.941f, 170.9341f, "Safe House group 2" },
	{ 9, 2319.1272f, -1023.9562f, 1050.2109f, 167.3959f, "Safe House group 3" },
	{ 10, 2261.0977f, -1137.8833f, 1050.6328f, 266.88f, "Safe House group 4" },
	{ 17, -944.2402f, 1886.1536f, 5.0051f, 179.8548f, "Sherman Dam" },
	{ 16, -26.1856f, -140.9164f, 1003.5469f, 2.9087f, "24/7 (version 4)" },
	{ 15, 2217.281f, -1150.5349f, 1025.7969f, 273.7328f, "Jefferson Motel" },
	{ 1, 1.5491f, 23.3183f, 1199.5938f, 359.9054f, "Jet Interior" },
	{ 1, 681.6216f, -451.8933f, -25.6172f, 166.166f, "The Welcome Pump" },
	{ 3, 234.6087f, 1187.8195f, 1080.2578f, 349.4844f, "Burglary House X1" },
	{ 2, 225.5707f, 1240.0643f, 1082.1406f, 96.2852f, "Burglary House X2" },
	{ 1, 224.288f, 1289.1907f, 1082.1406f, 359.868f, "Burglary House X3" },
	{ 5, 239.2819f, 1114.1991f, 1080.9922f, 270.2654f, "Burglary House X4" },
	{ 15, 207.5219f, -109.7448f, 1005.1328f, 358.62f, "Binco" },
	{ 15, 295.1391f, 1473.3719f, 1080.2578f, 352.9526f, "4 Burglary houses" },
	{ 15, -1417.8927f, 932.4482f, 1041.5313f, 0.7013f, "Blood Bowl Stadium" },
	{ 12, 446.3247f, 509.9662f, 1001.4195f, 330.5671f, "Budget Inn Motel Room" },
	{ 0, 2306.3826f, -15.2365f, 26.7496f, 274.49f, "Palamino Bank" },
	{ 0, 2331.8984f, 6.7816f, 26.5032f, 100.2357f, "Palamino Diner" },
	{ 0, 663.0588f, -573.6274f, 16.3359f, 264.9829f, "Dillimore Gas Station" },
	{ 18, -227.5703f, 1401.5544f, 27.7656f, 269.2978f, "Lil' Probe Inn" },
	{ 0, -688.1496f, 942.0826f, 13.6328f, 177.6574f, "Torreno's Ranch" },
	{ 0, -1916.1268f, 714.8617f, 46.5625f, 152.2839f, "Zombotech - lobby area" },
	{ 0, 818.7714f, -1102.8689f, 25.794f, 91.1439f, "Crypt in LS cemetery (temple)" },
	{ 0, 255.2083f, -59.6753f, 1.5703f, 1.4645f, "Blueberry Liquor Store" },
	{ 2, 446.626f, 1397.738f, 1084.3047f, 343.9647f, "Pair of Burglary Houses" },
	{ 5, 227.3922f, 1114.6572f, 1080.9985f, 267.459f, "Crack Den" },
	{ 5, 227.7559f, 1114.3844f, 1080.9922f, 266.2624f, "Burglary House X11" },
	{ 4, 261.1165f, 1287.2197f, 1080.2578f, 178.9149f, "Burglary House X12" },
	{ 4, 291.7626f, -80.1306f, 1001.5156f, 290.2195f, "Ammu-nation (version 3)" },
	{ 4, 449.0172f, -88.9894f, 999.5547f, 89.6608f, "Jay's Diner" },
	{ 4, -27.844f, -26.6737f, 1003.5573f, 184.3118f, "24/7 (version 5)" },
	{ 0, 2135.2004f, -2276.2815f, 20.6719f, 318.59f, "Warehouse 3" },
	{ 4, 306.1966f, 307.819f, 1003.3047f, 203.1354f, "Michelle's Love Nest*" },
	{ 10, 24.3769f, 1341.1829f, 1084.375f, 8.3305f, "Burglary House X14" },
	{ 1, 963.0586f, 2159.7563f, 1011.0303f, 175.313f, "Sindacco Abatoir" },
	{ 0, 2548.4807f, 2823.7429f, 10.8203f, 270.6003f, "K.A.C.C. Military Fuels Depot" },
	{ 0, 215.1515f, 1874.0579f, 13.1406f, 177.5538f, "Area 69" },
	{ 4, 221.6766f, 1142.4962f, 1082.6094f, 184.9618f, "Burglary House X13" },
	{ 12, 2323.7063f, -1147.6509f, 1050.7101f, 206.5352f, "Unused Safe House" },
	{ 6, 344.9984f, 307.1824f, 999.1557f, 193.643f, "Millie's Bedroom" },
	{ 12, 411.9707f, -51.9217f, 1001.8984f, 173.3449f, "Barber Shop" },
	{ 4, -1421.5618f, -663.8262f, 1059.5569f, 170.9341f, "Dirtbike Stadium" },
	{ 6, 773.8887f, -47.7698f, 1000.5859f, 10.7161f, "Cobra Gym" },
	{ 6, 246.6695f, 65.8039f, 1003.6406f, 7.9562f, "Los Santos Police Department" },
	{ 14, -1864.9434f, 55.7325f, 1055.5276f, 85.8541f, "Los Santos Airport" },
	{ 4, -262.1759f, 1456.6158f, 1084.3672f, 82.459f, "Burglary House X15" },
	{ 5, 22.861f, 1404.9165f, 1084.4297f, 349.6158f, "Burglary House X16" },
	{ 5, 140.3679f, 1367.8837f, 1083.8621f, 349.2372f, "Burglary House X17" },
	{ 3, 1494.8589f, 1306.48f, 1093.2953f, 196.065f, "Bike School" },
	{ 14, -1813.213f, -58.012f, 1058.9641f, 335.3199f, "Francis International Airport" },
	{ 16, -1401.067f, 1265.3706f, 1039.8672f, 178.6483f, "Vice Stadium" },
	{ 6, 234.2826f, 1065.229f, 1084.2101f, 4.3864f, "Burglary House X18" },
	{ 6, -68.5145f, 1353.8485f, 1080.2109f, 3.5742f, "Burglary House X19" },
	{ 6, -2240.1028f, 136.973f, 1035.4141f, 269.0954f, "Zero's RC Shop" },
	{ 6, 297.144f, -109.8702f, 1001.5156f, 20.2254f, "Ammu-nation (version 4)" },
	{ 6, 316.5025f, -167.6272f, 999.5938f, 10.3031f, "Ammu-nation (version 5)" },
	{ 15, -285.2511f, 1471.197f, 1084.375f, 85.6547f, "Burglary House X20" },
	{ 6, -26.8339f, -55.5846f, 1003.5469f, 3.9528f, "24/7 (version 6)" },
	{ 6, 442.1295f, -52.4782f, 999.7167f, 177.9394f, "Secret Valley Diner" },
	{ 2, 2182.2017f, 1628.5848f, 1043.8723f, 224.8601f, "Rosenberg's Office in Caligulas" },
	{ 6, 748.4623f, 1438.2378f, 1102.9531f, 0.6069f, "Fanny Batter's Whore House" },
	{ 8, 2807.3604f, -1171.7048f, 1025.5703f, 193.7117f, "Colonel Furhberger's" },
	{ 9, 366.0002f, -9.4338f, 1001.8516f, 160.528f, "Cluckin' Bell" },
	{ 1, 2216.1282f, -1076.3052f, 1050.4844f, 86.428f, "The Camel's Toe Safehouse" },
	{ 1, 2268.5156f, 1647.7682f, 1084.2344f, 99.7331f, "Caligula's Roof" },
	{ 2, 2236.6997f, -1078.9478f, 1049.0234f, 2.5706f, "Old Venturas Strip Casino" },
	{ 3, -2031.1196f, -115.8287f, 1035.1719f, 190.1877f, "Driving School" },
	{ 8, 2365.1089f, -1133.0795f, 1050.875f, 177.3947f, "Verdant Bluffs Safehouse" },
	{ 0, 1168.512f, 1360.1145f, 10.9293f, 196.5933f, "Bike School" },
	{ 9, 315.4544f, 976.5972f, 1960.8511f, 359.6368f, "Andromada" },
	{ 10, 1893.0731f, 1017.8958f, 31.8828f, 86.1044f, "Four Dragons' Janitor's Office" },
	{ 11, 501.9578f, -70.5648f, 998.7578f, 171.5706f, "Bar" },
	{ 8, -42.5267f, 1408.23f, 1084.4297f, 172.068f, "Burglary House X21" },
	{ 11, 2283.3118f, 1139.307f, 1050.8984f, 19.7032f, "Willowfield Safehouse" },
	{ 9, 84.9244f, 1324.2983f, 1083.8594f, 159.5582f, "Burglary House X22" },
	{ 9, 260.7421f, 1238.2261f, 1084.2578f, 84.3084f, "Burglary House X23" },
	{ 0, -1658.1656f, 1215.0002f, 7.25f, 103.9074f, "Otto's Autos" },
	{ 0, -1961.6281f, 295.2378f, 35.4688f, 264.4891f, "Wang Cars" }
};