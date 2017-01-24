/*  GNU Robbo
 *  Copyright (C) 2002-2010 The GNU Robbo Team (see AUTHORS).
 *
 *  GNU Robbo is free software - you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GNU Robbo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the impled warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU CC; see the file COPYING. If not, write to the
 *  Free Software Foundation, 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 */

/* Defines */
#define MAX_LOCALES 12
#define DEFAULT_LOCALE "en_GB"

#define TXT_INTRO_STORY_ROWS 4
#define TXT_INTRO_KEYS_ROWS 7

#if defined(PLATFORM_PSVITA) || defined(PLATFORM_PSP)
#define TXT_INTRO_KEYS_DESCRIPTION_ROWS 7
#else
#define TXT_INTRO_KEYS_DESCRIPTION_ROWS 6
#endif

#define TXT_INTRO_MENU_ROWS 7
#define TXT_INTRO_CREDITS_ROWS 3
#define TXT_HELP_PAGE0_ROWS 12
#define TXT_HELP_PAGE1_ROWS 7
#define TXT_HELP_PAGE2_ROWS 7
#define TXT_HELP_PAGE3_ROWS 7
#define TXT_OPTIONS_PAGE1_ROWS 12
#define TXT_ENDSCREEN_CONGRATS_ROWS 3
#define TXT_MAX_COLS 100

/* Variables */
char txt_intro_story[TXT_INTRO_STORY_ROWS * TXT_MAX_COLS];
char txt_intro_keys[TXT_INTRO_KEYS_ROWS * TXT_MAX_COLS];
char txt_intro_keys_description[TXT_INTRO_KEYS_DESCRIPTION_ROWS * TXT_MAX_COLS];
char txt_intro_menu[TXT_INTRO_MENU_ROWS * TXT_MAX_COLS];
char txt_of[TXT_MAX_COLS];
char txt_intro_credits[TXT_INTRO_CREDITS_ROWS * TXT_MAX_COLS];

char txt_help_page0[TXT_HELP_PAGE0_ROWS * TXT_MAX_COLS];
char txt_help_page1[TXT_HELP_PAGE1_ROWS * TXT_MAX_COLS];
char txt_help_page2[TXT_HELP_PAGE2_ROWS * TXT_MAX_COLS];
char txt_help_page3[TXT_HELP_PAGE3_ROWS * TXT_MAX_COLS];
char txt_Back[TXT_MAX_COLS];
char txt_Next[TXT_MAX_COLS];
char txt_Exit[TXT_MAX_COLS];

char txt_options_page1[TXT_OPTIONS_PAGE1_ROWS * TXT_MAX_COLS];
char txt_Save[TXT_MAX_COLS];
char txt_Game_Speed[TXT_MAX_COLS];
char txt_Fast[TXT_MAX_COLS];
char txt_Normal[TXT_MAX_COLS];
char txt_Slow[TXT_MAX_COLS];
char txt_Key_Repeat[TXT_MAX_COLS];
char txt_Delay[TXT_MAX_COLS];
char txt_Interval[TXT_MAX_COLS];
char txt_Default_Joystick[TXT_MAX_COLS];
char txt_No_joystick_found[TXT_MAX_COLS];
char txt_Joystick_Axes_Dead_Zone[TXT_MAX_COLS];
char txt_Language[TXT_MAX_COLS];
char txt_Translation_by[TXT_MAX_COLS];
char txt_Save_Frequency[TXT_MAX_COLS];
char txt_On_Exit[TXT_MAX_COLS];
char txt_On_Change[TXT_MAX_COLS];
char txt_Move_Up[TXT_MAX_COLS];
char txt_Move_Up_Right[TXT_MAX_COLS];
char txt_Move_Right[TXT_MAX_COLS];
char txt_Move_Down_Right[TXT_MAX_COLS];
char txt_Move_Down[TXT_MAX_COLS];
char txt_Move_Down_Left[TXT_MAX_COLS];
char txt_Move_Left[TXT_MAX_COLS];
char txt_Move_Up_Left[TXT_MAX_COLS];
char txt_Shoot_Up[TXT_MAX_COLS];
char txt_Shoot_Down[TXT_MAX_COLS];
char txt_Shoot_Left[TXT_MAX_COLS];
char txt_Shoot_Right[TXT_MAX_COLS];
char txt_Restart[TXT_MAX_COLS];
char txt_Confirm[TXT_MAX_COLS];
char txt_Help[TXT_MAX_COLS];
char txt_Options[TXT_MAX_COLS];
char txt_Previous_Level[TXT_MAX_COLS];
char txt_Next_Level[TXT_MAX_COLS];
char txt_Previous_Pack[TXT_MAX_COLS];
char txt_Next_Pack[TXT_MAX_COLS];
char txt_Toggle_Fullscreen[TXT_MAX_COLS];
char txt_Home[TXT_MAX_COLS];
char txt_End[TXT_MAX_COLS];
char txt_Page_Up[TXT_MAX_COLS];
char txt_Page_Down[TXT_MAX_COLS];
char txt_Volume_Up[TXT_MAX_COLS];
char txt_Volume_Down[TXT_MAX_COLS];
char txt_Modifier[TXT_MAX_COLS];
char txt_Restore_Default_Controls[TXT_MAX_COLS];
char txt_Toggle_Designer[TXT_MAX_COLS];
char txt_Scroll_Up[TXT_MAX_COLS];
char txt_Scroll_Down[TXT_MAX_COLS];
char txt_Primary_Click[TXT_MAX_COLS];
char txt_System_Pointer[TXT_MAX_COLS];
char txt_Disabled[TXT_MAX_COLS];
char txt_Enabled[TXT_MAX_COLS];
char txt_Pointer_Controls_Pad_Type[TXT_MAX_COLS];
char txt_Viewport[TXT_MAX_COLS];
char txt_Screen[TXT_MAX_COLS];
char txt_Sound[TXT_MAX_COLS];
char txt_Sfx_Volume[TXT_MAX_COLS];
char txt_Simulated_Pointer[TXT_MAX_COLS];

char txt_Key[TXT_MAX_COLS];
char txt_Joy[TXT_MAX_COLS];
char txt_Ptr[TXT_MAX_COLS];
char txt_Mod[TXT_MAX_COLS];
char txt_Skin[TXT_MAX_COLS];
char txt_By[TXT_MAX_COLS];

char txt_Level_Author[TXT_MAX_COLS];
char txt_endscreen_congrats[TXT_ENDSCREEN_CONGRATS_ROWS * TXT_MAX_COLS];

char txt_Volume[TXT_MAX_COLS];
char txt_Changes_saved[TXT_MAX_COLS];
char txt_Default_controls_restored[TXT_MAX_COLS];
char txt_Press_something_and_release[TXT_MAX_COLS];
char txt_No_input_was_detected[TXT_MAX_COLS];

char txt_ptr_BtnLeft[TXT_MAX_COLS];
char txt_ptr_BtnMiddle[TXT_MAX_COLS];
char txt_ptr_BtnRight[TXT_MAX_COLS];
char txt_ptr_WheelUp[TXT_MAX_COLS];
char txt_ptr_WheelDown[TXT_MAX_COLS];

char txt_key_Backspace[TXT_MAX_COLS];
char txt_key_Tab[TXT_MAX_COLS];
char txt_key_Clear[TXT_MAX_COLS];
char txt_key_Return[TXT_MAX_COLS];
char txt_key_Pause[TXT_MAX_COLS];
char txt_key_Escape[TXT_MAX_COLS];
char txt_key_Space[TXT_MAX_COLS];
char txt_key_Delete[TXT_MAX_COLS];
char txt_key_KP[TXT_MAX_COLS];
char txt_key_Up[TXT_MAX_COLS];
char txt_key_Down[TXT_MAX_COLS];
char txt_key_Right[TXT_MAX_COLS];
char txt_key_Left[TXT_MAX_COLS];
char txt_key_Insert[TXT_MAX_COLS];
char txt_key_Home[TXT_MAX_COLS];
char txt_key_End[TXT_MAX_COLS];
char txt_key_PgUp[TXT_MAX_COLS];
char txt_key_PgDn[TXT_MAX_COLS];
char txt_key_NumLk[TXT_MAX_COLS];
char txt_key_CapsLk[TXT_MAX_COLS];
char txt_key_ScrlLk[TXT_MAX_COLS];
char txt_key_RShift[TXT_MAX_COLS];
char txt_key_LShift[TXT_MAX_COLS];
char txt_key_RCtrl[TXT_MAX_COLS];
char txt_key_LCtrl[TXT_MAX_COLS];
char txt_key_RAlt[TXT_MAX_COLS];
char txt_key_LAlt[TXT_MAX_COLS];
char txt_key_RMeta[TXT_MAX_COLS];
char txt_key_LMeta[TXT_MAX_COLS];
char txt_key_LSuper[TXT_MAX_COLS];
char txt_key_RSuper[TXT_MAX_COLS];
char txt_key_AltGr[TXT_MAX_COLS];
char txt_key_Compose[TXT_MAX_COLS];
char txt_key_Help[TXT_MAX_COLS];
char txt_key_PrScr[TXT_MAX_COLS];
char txt_key_SysRq[TXT_MAX_COLS];
char txt_key_Break[TXT_MAX_COLS];
char txt_key_Menu[TXT_MAX_COLS];
char txt_key_Power[TXT_MAX_COLS];
char txt_key_Euro[TXT_MAX_COLS];
char txt_key_Undo[TXT_MAX_COLS];

char txt_konstruktor_Ground[TXT_MAX_COLS];
char txt_konstruktor_Stop[TXT_MAX_COLS];
char txt_konstruktor_Radioactive_Field[TXT_MAX_COLS];
char txt_konstruktor_Push_Box[TXT_MAX_COLS];
char txt_konstruktor_Capsule[TXT_MAX_COLS];
char txt_konstruktor_Bomb[TXT_MAX_COLS];
char txt_konstruktor_Bomb2[TXT_MAX_COLS];
char txt_konstruktor_Door[TXT_MAX_COLS];
char txt_konstruktor_Box[TXT_MAX_COLS];
char txt_konstruktor_Empty_Field[TXT_MAX_COLS];
char txt_konstruktor_Butterfly[TXT_MAX_COLS];
char txt_konstruktor_Gun_Fire[TXT_MAX_COLS];
char txt_konstruktor_Force_Field[TXT_MAX_COLS];
char txt_konstruktor_Screw[TXT_MAX_COLS];
char txt_konstruktor_Wall[TXT_MAX_COLS];
char txt_konstruktor_Black_Wall[TXT_MAX_COLS];
char txt_konstruktor_Teleport[TXT_MAX_COLS];
char txt_konstruktor_Gun[TXT_MAX_COLS];
char txt_konstruktor_Magnet[TXT_MAX_COLS];
char txt_konstruktor_Bear[TXT_MAX_COLS];
char txt_konstruktor_Black_Bear[TXT_MAX_COLS];
char txt_konstruktor_Bird[TXT_MAX_COLS];
char txt_konstruktor_Key[TXT_MAX_COLS];
char txt_konstruktor_Bullets[TXT_MAX_COLS];
char txt_konstruktor_on_map[TXT_MAX_COLS];
char txt_konstruktor_Laser_Gun[TXT_MAX_COLS];
char txt_konstruktor_Blaster_Gun[TXT_MAX_COLS];
char txt_konstruktor_Regular_Gun[TXT_MAX_COLS];
char txt_konstruktor_Fixed[TXT_MAX_COLS];
char txt_konstruktor_Moveable[TXT_MAX_COLS];
char txt_konstruktor_Rotating[TXT_MAX_COLS];
char txt_konstruktor_Moveable_Rotating[TXT_MAX_COLS];
char txt_konstruktor_Direction_right[TXT_MAX_COLS];
char txt_konstruktor_Direction_down[TXT_MAX_COLS];
char txt_konstruktor_Direction_left[TXT_MAX_COLS];
char txt_konstruktor_Direction_up[TXT_MAX_COLS];
char txt_konstruktor_Welcome_to_GNU_Robbo_Designer[TXT_MAX_COLS];
char txt_konstruktor_Exit_designer_select_again_to_exit[TXT_MAX_COLS];
char txt_konstruktor_Reload_level_select_again_to_reload[TXT_MAX_COLS];
char txt_konstruktor_Level_reloaded[TXT_MAX_COLS];
char txt_konstruktor_New_level_select_again_to_create[TXT_MAX_COLS];
char txt_konstruktor_Level_created[TXT_MAX_COLS];
char txt_konstruktor_Save_select_again_to_save_new_level[TXT_MAX_COLS];
char txt_konstruktor_New_level_appended_to_pack[TXT_MAX_COLS];
char txt_konstruktor_Clear_level_select_again_to_clear[TXT_MAX_COLS];
char txt_konstruktor_Level_cleared[TXT_MAX_COLS];
char txt_konstruktor_Save_select_again_to_save_changes[TXT_MAX_COLS];
char txt_konstruktor_Changes_saved[TXT_MAX_COLS];
char txt_konstruktor_Level_resized_to[TXT_MAX_COLS];
char txt_konstruktor_Screws[TXT_MAX_COLS];

struct locale {
	char foldername[100];		/* e.g. de_DE, en_GB, es_ES, pl_PL ... */
	char name[60];				/* e.g. Deutsch, English, Español, Polski ... */
	char author[60];			/* Enables translators to get recognition for their work */
};
struct locale locales[MAX_LOCALES];

int locale_count;
int selected_locale;
int temp_selected_locale;

/* Function prototypes */
int find_all_locales(void);
void read_localerc_files(void);
void sort_locales(void);
void load_selected_locale(void);
void set_locale_to_LANG(void);







