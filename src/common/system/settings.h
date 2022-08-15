#ifndef SETTINGS_H__
#define SETTINGS_H__

#include <stdbool.h>

#include "utils/file.h"
#include "utils/json.h"
#include "utils/config.h"
#include "display.h"

#define MAX_BRIGHTNESS 10
#define MAIN_UI_SETTINGS "/appconfigs/system.json"

static struct settings_s
{
    int volume;
    char keymap[JSON_STRING_LEN];
    int mute;
    int bgm_volume;
    int brightness;
    char language[JSON_STRING_LEN];
    int sleep_timer;
    int lumination;
    int hue;
    int saturation;
    int contrast;
    char theme[JSON_STRING_LEN];
    int fontsize;
    int audiofix;
    bool show_recents;
    bool show_expert;
    bool startup_auto_resume;
    bool menu_button_haptics;
    bool low_battery_autosave;
    bool low_battery_warning;
    int low_battery_warn_at;
    int vibration;
    int mainui_single_press;
    int mainui_long_press;
    int mainui_double_press;
    int ingame_single_press;
    int ingame_long_press;
    int ingame_double_press;
    int startup_application;
}
settings = {
    // MainUI settings
    .volume = 20,
    .keymap = "L2,L,R2,R,X,A,B,Y",
    .mute = 1,
    .bgm_volume = 20,
    .brightness = 7,
    .language = "en.lang",
    .sleep_timer = 5,
    .lumination = 7,
    .hue = 10,
    .saturation = 10,
    .contrast = 10,
    .theme = "/mnt/SDCARD/Themes/Silky by DiMo/",
    .fontsize = 24,
    .audiofix = 1,
    // Onion settings
    .show_recents = false,
    .show_expert = false,
    .startup_auto_resume = true,
    .menu_button_haptics = false,
    .low_battery_autosave = true,
    .low_battery_warning = true,
    .low_battery_warn_at = 15,
    .vibration = 2,
    // Menu button actions
    .mainui_single_press = 1,
    .mainui_long_press = 0,
    .mainui_double_press = 2,
    .ingame_single_press = 1,
    .ingame_long_press = 2,
    .ingame_double_press = 3,
    .startup_application = 0
};

static bool settings_loaded = false;

void _settings_load_keymap(void)
{
    if (!exists(CONFIG_PATH "menu_button_keymap.json"))
        return;

    cJSON *keymap = json_load(CONFIG_PATH "menu_button_keymap.json");
    json_getInt(keymap, "mainui_single_press", &settings.mainui_single_press);
    json_getInt(keymap, "mainui_long_press", &settings.mainui_long_press);
    json_getInt(keymap, "mainui_double_press", &settings.mainui_double_press);
    json_getInt(keymap, "ingame_single_press", &settings.ingame_single_press);
    json_getInt(keymap, "ingame_long_press", &settings.ingame_long_press);
    json_getInt(keymap, "ingame_double_press", &settings.ingame_double_press);
    cJSON_free(keymap);
}

void _settings_load_mainui(void)
{
    const char *json_str = NULL;

	if (!(json_str = file_read(MAIN_UI_SETTINGS)))
		return;

	cJSON* json_root = cJSON_Parse(json_str);

    json_getInt(json_root, "vol", &settings.volume);
    json_getInt(json_root, "mute", &settings.mute);
    json_getInt(json_root, "bgmvol", &settings.bgm_volume);
    json_getInt(json_root, "brightness",&settings.brightness);
    json_getInt(json_root, "hibernate", &settings.sleep_timer);
    json_getInt(json_root, "lumination", &settings.lumination);
    json_getInt(json_root, "hue", &settings.hue);
    json_getInt(json_root, "saturation", &settings.saturation);
    json_getInt(json_root, "contrast", &settings.contrast);
    json_getInt(json_root, "fontsize", &settings.fontsize);
    json_getInt(json_root, "audiofix", &settings.audiofix);

    json_getString(json_root, "keymap", settings.keymap);
    json_getString(json_root, "language", settings.language);
    json_getString(json_root, "theme", settings.theme);

    cJSON_free(json_root);
}

void settings_load(void)
{
    settings.startup_auto_resume = !config_flag_get(".noAutoStart");
    settings.menu_button_haptics = config_flag_get(".menuHaptics");
    settings.show_recents = !config_flag_get(".hideRecents");
    settings.show_expert = !config_flag_get(".hideExpert");
    settings.low_battery_autosave = !config_flag_get(".noLowBatteryAutoSave");

    if (config_flag_get(".noBatteryWarning"))
        settings.low_battery_warn_at = 0;

    config_get("battery/warnAt", "%d", &settings.low_battery_warn_at);
    config_get("startup/app", "%d", &settings.startup_application);
    config_get("vibration", "%d", &settings.vibration);

    if (config_flag_get(".menuInverted")) { // flag is deprecated, but keep compatibility
        settings.ingame_single_press = 2;
        settings.ingame_long_press = 1;
    }

    if (config_flag_get(".noGameSwitcher")) { // flag is deprecated, but keep compatibility
        settings.mainui_single_press = 0;
        settings.ingame_single_press = 2;
        settings.ingame_long_press = 0;
    }

    _settings_load_keymap();
    _settings_load_mainui();

    settings_loaded = true;
}

void _settings_save_keymap(void)
{
    FILE *fp;

    if ((fp = fopen(CONFIG_PATH "menu_button_keymap.json", "w+")) == 0)
        return;

    fprintf(fp, "{\n");
    fprintf(fp, JSON_FORMAT_NUMBER, "mainui_single_press", settings.mainui_single_press);
    fprintf(fp, JSON_FORMAT_NUMBER, "mainui_long_press", settings.mainui_long_press);
    fprintf(fp, JSON_FORMAT_NUMBER, "mainui_double_press", settings.mainui_double_press);
    fprintf(fp, JSON_FORMAT_NUMBER, "ingame_single_press", settings.ingame_single_press);
    fprintf(fp, JSON_FORMAT_NUMBER, "ingame_long_press", settings.ingame_long_press);
    fprintf(fp, JSON_FORMAT_NUMBER_NC, "ingame_double_press", settings.ingame_double_press);
    fprintf(fp, "}\n");

    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
}

void _settings_save_mainui(void)
{
    FILE *fp;

    if ((fp = fopen(MAIN_UI_SETTINGS, "w+")) == 0)
        return;

    fprintf(fp, "{\n");
    fprintf(fp, JSON_FORMAT_NUMBER, "vol", settings.volume);
    fprintf(fp, JSON_FORMAT_STRING, "keymap", settings.keymap);
    fprintf(fp, JSON_FORMAT_NUMBER, "mute", settings.mute);
    fprintf(fp, JSON_FORMAT_NUMBER, "bgmvol", settings.bgm_volume);
    fprintf(fp, JSON_FORMAT_NUMBER, "brightness", settings.brightness);
    fprintf(fp, JSON_FORMAT_STRING, "language", settings.language);
    fprintf(fp, JSON_FORMAT_NUMBER, "hibernate", settings.sleep_timer);
    fprintf(fp, JSON_FORMAT_NUMBER, "lumination", settings.lumination);
    fprintf(fp, JSON_FORMAT_NUMBER, "hue", settings.hue);
    fprintf(fp, JSON_FORMAT_NUMBER, "saturation", settings.saturation);
    fprintf(fp, JSON_FORMAT_NUMBER, "contrast", settings.contrast);
    fprintf(fp, JSON_FORMAT_STRING, "theme", settings.theme);
    fprintf(fp, JSON_FORMAT_NUMBER, "fontsize", settings.fontsize);
    fprintf(fp, JSON_FORMAT_NUMBER_NC, "audiofix", settings.audiofix);
    fprintf(fp, "}\n");

    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
}

void settings_save(void)
{
    config_flag_set(".noAutoStart", !settings.startup_auto_resume);
    config_flag_set(".noVibration", !settings.vibration);
    config_flag_set(".menuHaptics", settings.menu_button_haptics);
    config_flag_set(".hideRecents", !settings.show_recents);
    config_flag_set(".hideExpert", !settings.show_expert);
    config_flag_set(".noLowBatteryAutoSave", !settings.low_battery_autosave);
    config_setNumber("battery/warnAt", settings.low_battery_warn_at);
    config_setNumber("startup/app", settings.startup_application);
    config_setNumber("vibration", settings.vibration);

    _settings_save_keymap();
    _settings_save_mainui();
}

void settings_setBrightness(uint32_t value, bool apply)
{
    settings.brightness = value;

    if (apply)
        display_setBrightness(value);
}


#endif // SETTINGS_H__
