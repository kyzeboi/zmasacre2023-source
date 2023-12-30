/*

	 ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#pragma once

struct key_state
{
	uint8_t count : 5;
	uint8_t flip : 1;
	uint8_t pstate : 1;		/* previous state (1 down, 0 up) */
	uint8_t consume : 1;	/* KEY_CONSUME(vkey) */
};

#define KEY_DOWN			keyhook_key_down
#define KEY_UP				keyhook_key_up
#define KEY_PRESSED			keyhook_key_pressed
#define KEY_RELEASED		keyhook_key_released
#define KEY_CONSUME			keyhook_key_consume

#define KEYCOMBO_DOWN		keyhook_keycombo_down
#define KEYCOMBO_UP			keyhook_keycombo_up
#define KEYCOMBO_PRESSED	keyhook_keycombo_pressed
#define KEYCOMBO_RELEASED	keyhook_keycombo_released

void						keyhook_maybe_install(HWND wnd);
void						keyhook_uninstall(void);
	
void						keyhook_run(void);

int							keyhook_key_down(int v);
int							keyhook_key_up(int v);
int							keyhook_key_pressed(int v);
int							keyhook_key_released(int v);
void						keyhook_key_consume(int v);
void						keyhook_clear_states(void);


bool						keyhook_keycombo_down(const keycombo& comb);
bool						keyhook_keycombo_up(const keycombo& comb);
bool						keyhook_keycombo_pressed(const keycombo& comb);
bool						keyhook_keycombo_released(const keycombo& comb);

extern struct key_state		key_table[256];
extern int					key_being_pressed;
