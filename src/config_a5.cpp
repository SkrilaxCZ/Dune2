/* config_a5.c */

#include <allegro5/allegro.h>
#include <cctype>
#include <cstdio>
#include <Windows.h>

#include "os/math.h"

#include "config.h"

#include "audio/audio.h"
#include "enhancement.h"
#include "file.h"
#include "gfx.h"
#include "string.h"
#include "video/video.h"

#define CONFIG_FILENAME "dune2.cfg"

enum OptionType
{
	CONFIG_ASPECT_CORRECTION,
	CONFIG_BOOL,
	CONFIG_FLOAT,
	CONFIG_FLOAT_05_2,
	CONFIG_FLOAT_1_3,
	CONFIG_GRAPHICS_DRIVER,
	CONFIG_INT,
	CONFIG_INT_0_4,
	CONFIG_INT_1_16,
	CONFIG_STRING,
	CONFIG_WINDOW_MODE
};

struct GameOption
{
	const char* section;
	const char* key;
	OptionType type;
	void* data;
};

static ALLEGRO_CONFIG* s_configFile;

GameCfg g_gameConfig = {
#ifdef DEBUG
	WM_WINDOWED,
#else
	WM_FULLSCREEN,
#endif
	2, /* gameSpeed */
	true, /* hints */
	true, /* autoScroll */
	true, /* scrollAlongScreenEdge */
	4, /* scrollSpeed */
	false, /* leftClickOrders */
	false, /* holdControlToZoom */
	1.0f, /* panSensitivity */
};

static int saved_screen_width = 1280;
static int saved_screen_height = 720;

/*--------------------------------------------------------------*/

static const GameOption s_game_option[] = {
	{"game", "game_speed", CONFIG_INT_0_4, &g_gameConfig.gameSpeed},
	{"game", "hints", CONFIG_BOOL, &g_gameConfig.hints},

	{"graphics", "driver", CONFIG_GRAPHICS_DRIVER, &g_graphics_driver},
	{"graphics", "window_mode", CONFIG_WINDOW_MODE, &g_gameConfig.windowMode},
	{"graphics", "screen_width", CONFIG_INT, &saved_screen_width},
	{"graphics", "screen_height", CONFIG_INT, &saved_screen_height},
	{"graphics", "correct_aspect_ratio", CONFIG_ASPECT_CORRECTION, &g_aspect_correction},
	{"graphics", "menubar_scale", CONFIG_FLOAT_1_3, &g_screenDiv[SCREENDIV_MENUBAR].scalex},
	{"graphics", "sidebar_scale", CONFIG_FLOAT_1_3, &g_screenDiv[SCREENDIV_SIDEBAR].scalex},
	{"graphics", "viewport_scale", CONFIG_FLOAT_1_3, &g_screenDiv[SCREENDIV_VIEWPORT].scalex},

	{"controls", "auto_scroll", CONFIG_BOOL, &g_gameConfig.autoScroll},
	{"controls", "scroll_speed", CONFIG_INT_1_16, &g_gameConfig.scrollSpeed},
	{"controls", "scroll_along_screen_edge", CONFIG_BOOL, &g_gameConfig.scrollAlongScreenEdge},
	{"controls", "left_click_orders", CONFIG_BOOL, &g_gameConfig.leftClickOrders},
	{"controls", "hold_control_to_zoom", CONFIG_BOOL, &g_gameConfig.holdControlToZoom},
	{"controls", "pan_sensitivity", CONFIG_FLOAT_05_2, &g_gameConfig.panSensitivity},

	{"audio", "enable_music", CONFIG_BOOL, &g_enable_music},
	{"audio", "enable_sounds", CONFIG_BOOL, &g_enable_sound},

	{"audio", "enable_subtitles", CONFIG_BOOL, &g_enable_subtitles},
	{"audio", "music_volume", CONFIG_FLOAT, &music_volume},
	{"audio", "sound_volume", CONFIG_FLOAT, &sound_volume},
	{"audio", "voice_volume", CONFIG_FLOAT, &voice_volume},
	{"audio", "opl_mame", CONFIG_BOOL, &g_opl_mame},

	{"enhancement", "brutal_ai", CONFIG_BOOL, &enhancement_brutal_ai},
	{"enhancement", "fog_of_war", CONFIG_BOOL, &enhancement_fog_of_war},

	{NULL, NULL, CONFIG_BOOL, NULL}
};

/*--------------------------------------------------------------*/

static ALLEGRO_CONFIG* Config_CreateConfigFile()
{
	ALLEGRO_CONFIG* config = al_create_config();

	if (config != NULL)
	{
		al_add_config_comment(config, NULL, "# Dune Dynasty config file");
	}

	return config;
}

static void Config_GetAspectCorrection(const char* str, AspectRatioCorrection* value)
{
	const char* aspect_str = strchr(str, ',');
	const char c = tolower(str[0]);

	if (c == 'n')
	{
		g_pixel_aspect_ratio = 1.0f;
		*value = ASPECT_RATIO_CORRECTION_NONE;
	}

	/* menu or partial. */
	else if (c == 'm')
	{
		g_pixel_aspect_ratio = 1.1f;
		*value = ASPECT_RATIO_CORRECTION_PARTIAL;
	}
	else if (c == 'p')
	{
		g_pixel_aspect_ratio = 1.1f;
		*value = ASPECT_RATIO_CORRECTION_PARTIAL;
	}

	else if (c == 'f')
	{
		g_pixel_aspect_ratio = 1.2f;
		*value = ASPECT_RATIO_CORRECTION_FULL;
	}
	else if (c == 'a')
	{
		g_pixel_aspect_ratio = 1.1f;
		*value = ASPECT_RATIO_CORRECTION_AUTO;
	}

	if (aspect_str != NULL)
	{
		sscanf(aspect_str + 1, "%f", &g_pixel_aspect_ratio);
		g_pixel_aspect_ratio = clamp(0.5f, g_pixel_aspect_ratio, 2.0f);
	}
}

static void Config_GetFloat(const char* str, float min_val, float max_val, float* value)
{
	*value = clamp(min_val, atof(str), max_val);
}

static void Config_SetFloat(ALLEGRO_CONFIG* config, const char* section, const char* key, float value)
{
	char str[16];

	snprintf(str, sizeof(str), "%.2f", value);
	al_set_config_value(config, section, key, str);
}

static void Config_GetGraphicsDriver(const char* str, GraphicsDriver* value)
{
	UNUSED(str);

	*value = GRAPHICS_DRIVER_OPENGL;
	if (str[0] == 'D' || str[0] == 'd')
		*value = GRAPHICS_DRIVER_DIRECT3D;
}

static void Config_SetGraphicsDriver(ALLEGRO_CONFIG* config, const char* section, const char* key, GraphicsDriver graphics_driver)
{
	const char* str = "opengl";

	switch (graphics_driver)
	{
	case GRAPHICS_DRIVER_DIRECT3D:
		str = "direct3d";
		break;

	default:
		break;
	}

	al_set_config_value(config, section, key, str);
}

static void Config_GetInt(const char* str, int min_val, int max_val, int* value)
{
	*value = clamp(min_val, atoi(str), max_val);
}

static void Config_SetInt(ALLEGRO_CONFIG* config, const char* section, const char* key, int value)
{
	char str[16];

	snprintf(str, sizeof(str), "%d", value);
	al_set_config_value(config, section, key, str);
}

static void Config_GetWindowMode(const char* str, WindowMode* value)
{
#ifdef DEBUG
	/* Allow only windowed mode for debugging */
	*value = WM_WINDOWED;
	return;
#else

	/* Anything that's not 'fullscreen': win, window, windowed, etc. */
	if (str[0] != 'f' && str[0] != 'F')
	{
		*value = WM_WINDOWED;
		return;
	}

	*value = WM_FULLSCREEN;

	/* Anything with 'w': fsw, fullscreen_window, etc. */
	while (*str != '\0')
	{
		if (*str == 'w' || *str == 'W')
		{
			*value = WM_FULLSCREEN_WINDOW;
			return;
		}

		if (*str == '#' || *str == ';')
			break;

		str++;
	}
#endif
}

static void Config_SetWindowMode(ALLEGRO_CONFIG* config, const char* section, const char* key, WindowMode value)
{
#ifndef DEBUG
	const char* str[] = {"windowed", "fullscreen", "fullscreenwindow"};

	if (value > WM_FULLSCREEN_WINDOW)
		value = WM_WINDOWED;

	al_set_config_value(config, section, key, str[value]);
#endif
}

/*--------------------------------------------------------------*/

static void ConfigA5_InitDataDirectoriesAndLoadConfigFile()
{
#ifndef DEBUG
	ALLEGRO_PATH* dune_data_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	const char* dune_data_cstr = al_path_cstr(dune_data_path, ALLEGRO_NATIVE_PATH_SEP);
#else
	/* The executable is not in the global data directory */
	char dune_data_cstr[1024];
	GetCurrentDirectoryA(sizeof(dune_data_cstr), dune_data_cstr);
#endif
	ALLEGRO_PATH* user_data_path = al_get_standard_path(ALLEGRO_USER_DATA_PATH);
	const char* user_data_cstr = al_path_cstr(user_data_path, ALLEGRO_NATIVE_PATH_SEP);

	ALLEGRO_PATH* user_settings_path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
	const char* user_settings_cstr = al_path_cstr(user_settings_path, ALLEGRO_NATIVE_PATH_SEP);
	char filename[1024];

	snprintf(g_dune_data_dir, sizeof(g_dune_data_dir), "%s", dune_data_cstr);

	/* Find personal directory, and create subdirectories. */

	/* 1. Try current executable directory/dunedynasty.cfg. */
	snprintf(filename, sizeof(filename), "%s/%s", g_dune_data_dir, CONFIG_FILENAME);
	s_configFile = al_load_config_file(filename);

#ifndef DEBUG
	al_destroy_path(dune_data_path);
#endif
	al_destroy_path(user_data_path);
	al_destroy_path(user_settings_path);
	fprintf(stdout, "Dune data directory: %s\n", g_dune_data_dir);
}

void GameOptions_Load()
{
	ConfigA5_InitDataDirectoriesAndLoadConfigFile();
	if (s_configFile == NULL)
		return;

	for (int i = 0; s_game_option[i].key != NULL; i++)
	{
		const GameOption* opt = &s_game_option[i];

		const char* str = al_get_config_value(s_configFile, opt->section, opt->key);
		if (str == NULL)
			continue;

		switch (opt->type)
		{
		case CONFIG_ASPECT_CORRECTION:
			Config_GetAspectCorrection(str, (AspectRatioCorrection*)opt->data);
			break;

		case CONFIG_BOOL:
			String_GetBool(str, (bool*)opt->data);
			break;

		case CONFIG_FLOAT:
			Config_GetFloat(str, 0.0f, 1.0f, (float*)opt->data);
			break;

		case CONFIG_FLOAT_05_2:
			Config_GetFloat(str, 0.5f, 2.0f, (float*)opt->data);
			break;

		case CONFIG_FLOAT_1_3:
			Config_GetFloat(str, 1.0f, 3.0f, (float*)opt->data);
			break;

		case CONFIG_GRAPHICS_DRIVER:
			Config_GetGraphicsDriver(str, (GraphicsDriver*)opt->data);
			break;

		case CONFIG_INT:
			Config_GetInt(str, 0, INT_MAX, (int*)opt->data);
			break;

		case CONFIG_INT_0_4:
			Config_GetInt(str, 0, 4, (int*)opt->data);
			break;

		case CONFIG_INT_1_16:
			Config_GetInt(str, 1, 16, (int*)opt->data);
			break;

		case CONFIG_STRING:
			snprintf((char*)opt->data, 1024, "%s", str);
			break;

		case CONFIG_WINDOW_MODE:
			Config_GetWindowMode(str, (WindowMode*)opt->data);
			break;
		}
	}

	TRUE_DISPLAY_WIDTH = saved_screen_width;
	TRUE_DISPLAY_HEIGHT = saved_screen_height;
}

void GameOptions_Save()
{
	char filename[1024];

	if (s_configFile == NULL)
	{
		s_configFile = Config_CreateConfigFile();
		if (s_configFile == NULL)
			return;
	}

	for (int i = 0; s_game_option[i].key != NULL; i++)
	{
		const GameOption* opt = &s_game_option[i];

		switch (opt->type)
		{
		case CONFIG_BOOL:
			al_set_config_value(s_configFile, opt->section, opt->key, *((bool*)opt->data) ? "1" : "0");
			break;

		case CONFIG_FLOAT:
		case CONFIG_FLOAT_05_2:
		case CONFIG_FLOAT_1_3:
			Config_SetFloat(s_configFile, opt->section, opt->key, *((float*)opt->data));
			break;

		case CONFIG_GRAPHICS_DRIVER:
			Config_SetGraphicsDriver(s_configFile, opt->section, opt->key, *((GraphicsDriver*)opt->data));
			break;

		case CONFIG_INT:
		case CONFIG_INT_0_4:
		case CONFIG_INT_1_16:
			Config_SetInt(s_configFile, opt->section, opt->key, *((int*)opt->data));
			break;

		case CONFIG_STRING:
			al_set_config_value(s_configFile, opt->section, opt->key, (const char*)opt->data);
			break;

		case CONFIG_WINDOW_MODE:
			Config_SetWindowMode(s_configFile, opt->section, opt->key, *((WindowMode*)opt->data));
			break;

		case CONFIG_ASPECT_CORRECTION:
			/* Not saved (hidden). */
			break;
		}
	}

	snprintf(filename, sizeof(filename), "%s/%s", g_dune_data_dir, CONFIG_FILENAME);
	al_save_config_file(filename, s_configFile);
}
