#ifndef TWEAKS_MENUS_H__
#define TWEAKS_MENUS_H__

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <SDL/SDL_image.h>

#include "components/list.h"

#include "./appstate.h"
#include "./formatters.h"
#include "./values.h"
#include "./actions.h"
#include "./tools.h"

static List _menu_main;
static List _menu_system;
static List _menu_button_action;
static List _menu_user_interface;
static List _menu_theme_overrides;
static List _menu_battery_percentage;
static List _menu_advanced;
static List _menu_reset_settings;
static List _menu_tools;

void menu_system(void *_)
{
	if (!_menu_system._created) {
		_menu_system = list_create(6, LIST_SMALL);
		strcpy(_menu_system.title, "System");
		list_addItem(&_menu_system, (ListItem){
			.label = "Auto-resume last game",
			.item_type = TOGGLE,
			.value = (int)settings.startup_auto_resume,
			.action = action_setStartupAutoResume
		});
		list_addItem(&_menu_system, (ListItem){
			.label = "Save and exit when battery <4%",
			.item_type = TOGGLE,
			.value = (int)settings.low_battery_autosave,
			.action = action_setLowBatteryAutoSave
		});
		list_addItem(&_menu_system, (ListItem){
			.label = "Start application",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = {"MainUI", "GameSwitcher", "RetroArch"},
			.value = settings.startup_application,
			.action = action_setStartupApplication
		});
		list_addItem(&_menu_system, (ListItem){
			.label = "MainUI: Start tab",
			.item_type = MULTIVALUE,
			.value_max = 5,
			.value_formatter = formatter_startupTab,
			.value = settings.startup_tab,
			.action = action_setStartupTab
		});
		list_addItem(&_menu_system, (ListItem){
			.label = "Emulated time skip",
			.item_type = MULTIVALUE,
			.value_max = 24,
			.value_formatter = formatter_timeSkip,
			.value = settings.time_skip,
			.action = action_setTimeSkip
		});
		list_addItem(&_menu_system, (ListItem){
			.label = "Vibration intensity",
			.item_type = MULTIVALUE,
			.value_max = 3,
			.value_labels = {"Off", "Low", "Normal", "High"},
			.value = settings.vibration,
			.action = action_setVibration
		});
	}
	menu_stack[++level] = &_menu_system;
	header_changed = true;
}

void menu_buttonAction(void *_)
{
	if (!_menu_button_action._created) {
		_menu_button_action = list_create(7, LIST_SMALL);
		strcpy(_menu_button_action.title, "Menu button");
		list_addItem(&_menu_button_action, (ListItem){
			.label = "Vibrate on single press",
			.item_type = TOGGLE,
			.value = (int)settings.menu_button_haptics,
			.action = action_setMenuButtonHaptics
		});
		list_addItem(&_menu_button_action, (ListItem){
			.label = "MainUI: Single press",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = BUTTON_MAINUI_LABELS,
			.value = settings.mainui_single_press,
			.action_id = 0,
			.action = action_setMenuButtonKeymap
		});
		list_addItem(&_menu_button_action, (ListItem){
			.label = "MainUI: Long press",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = BUTTON_MAINUI_LABELS,
			.value = settings.mainui_long_press,
			.action_id = 1,
			.action = action_setMenuButtonKeymap
		});
		list_addItem(&_menu_button_action, (ListItem){
			.label = "MainUI: Double press",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = BUTTON_MAINUI_LABELS,
			.value = settings.mainui_double_press,
			.action_id = 2,
			.action = action_setMenuButtonKeymap
		});
		list_addItem(&_menu_button_action, (ListItem){
			.label = "In-game: Single press",
			.item_type = MULTIVALUE,
			.value_max = 3,
			.value_labels = BUTTON_INGAME_LABELS,
			.value = settings.ingame_single_press,
			.action_id = 3,
			.action = action_setMenuButtonKeymap
		});
		list_addItem(&_menu_button_action, (ListItem){
			.label = "In-game: Long press",
			.item_type = MULTIVALUE,
			.value_max = 3,
			.value_labels = BUTTON_INGAME_LABELS,
			.value = settings.ingame_long_press,
			.action_id = 4,
			.action = action_setMenuButtonKeymap
		});
		list_addItem(&_menu_button_action, (ListItem){
			.label = "In-game: Double press",
			.item_type = MULTIVALUE,
			.value_max = 3,
			.value_labels = BUTTON_INGAME_LABELS,
			.value = settings.ingame_double_press,
			.action_id = 5,
			.action = action_setMenuButtonKeymap
		});
	}
	menu_stack[++level] = &_menu_button_action;
	header_changed = true;
}

void menu_batteryPercentage(void *_)
{
	if (!_menu_battery_percentage._created) {
		_menu_battery_percentage = list_create(7, LIST_SMALL);
		strcpy(_menu_battery_percentage.title, "Battery percentage");
		list_addItem(&_menu_battery_percentage, (ListItem){
			.label = "Visible",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = THEME_TOGGLE_LABELS,
			.value = value_batteryPercentageVisible(),
			.action = action_batteryPercentageVisible
		});
		list_addItem(&_menu_battery_percentage, (ListItem){
			.label = "Position",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = {"-", "Left", "Right"},
			.value = value_batteryPercentagePosition(),
			.action = action_batteryPercentagePosition
		});
		list_addItem(&_menu_battery_percentage, (ListItem){
			.label = "Horizontal offset",
			.item_type = MULTIVALUE,
			.value_max = 21,
			.value_formatter = formatter_positionOffset,
			.value = value_batteryPercentageOffsetX(),
			.action = action_batteryPercentageOffsetX
		});
		list_addItem(&_menu_battery_percentage, (ListItem){
			.label = "Vertical offset",
			.item_type = MULTIVALUE,
			.value_max = 21,
			.value_formatter = formatter_positionOffset,
			.value = value_batteryPercentageOffsetY(),
			.action = action_batteryPercentageOffsetY
		});
	}
	menu_stack[++level] = &_menu_battery_percentage;
	header_changed = true;
}

void menu_themeOverrides(void *_)
{
	if (!_menu_theme_overrides._created) {
		_menu_theme_overrides = list_create(7, LIST_SMALL);
		strcpy(_menu_theme_overrides.title, "Theme overrides");
		list_addItem(&_menu_theme_overrides, (ListItem){
			.label = "Battery percentage...",
			.action = menu_batteryPercentage
		});
		list_addItem(&_menu_theme_overrides, (ListItem){
			.label = "Hide icon labels",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = THEME_TOGGLE_LABELS,
			.value = value_hideLabelsIcons(),
			.action = action_hideLabelsIcons
		});
		list_addItem(&_menu_theme_overrides, (ListItem){
			.label = "Hide hint labels",
			.item_type = MULTIVALUE,
			.value_max = 2,
			.value_labels = THEME_TOGGLE_LABELS,
			.value = value_hideLabelsHints(),
			.action = action_hideLabelsHints
		});
		// list_addItem(&_menu_theme_overrides, (ListItem){
		// 	.label = "Font family", .item_type = MULTIVALUE, .value_max = num_font_families, .value_formatter = formatter_fontFamily
		// });
		// list_addItem(&_menu_theme_overrides, (ListItem){
		// 	.label = "[Title] Font size", .item_type = MULTIVALUE, .value_max = num_font_sizes, .value_formatter = formatter_fontSize
		// });
		// list_addItem(&_menu_theme_overrides, (ListItem){
		// 	.label = "[List] Font size", .item_type = MULTIVALUE, .value_max = num_font_sizes, .value_formatter = formatter_fontSize
		// });
		// list_addItem(&_menu_theme_overrides, (ListItem){
		// 	.label = "[Hint] Font size", .item_type = MULTIVALUE, .value_max = num_font_sizes, .value_formatter = formatter_fontSize
		// });
		// list_addItem(&_menu_theme_overrides, (ListItem){
		// 	.label = "[Battery] Font size", .item_type = MULTIVALUE, .value_max = num_font_sizes, .value_formatter = formatter_fontSize
		// });
	}
	menu_stack[++level] = &_menu_theme_overrides;
	header_changed = true;
}

void menu_userInterface(void *_)
{
	if (!_menu_user_interface._created) {
		_menu_user_interface = list_create(4, LIST_SMALL);
		strcpy(_menu_user_interface.title, "User interface");
		list_addItem(&_menu_user_interface, (ListItem){
			.label = "Show recents",
			.item_type = TOGGLE,
			.value = settings.show_recents,
			.action = action_setShowRecents
		});
		list_addItem(&_menu_user_interface, (ListItem){
			.label = "Show expert mode",
			.item_type = TOGGLE,
			.value = settings.show_expert,
			.action = action_setShowExpert
		});
		list_addItem(&_menu_user_interface, (ListItem){
			.label = "Low battery warning",
			.item_type = MULTIVALUE,
			.value_max = 5,
			.value_formatter = formatter_battWarn,
			.value = settings.low_battery_warn_at / 5,
			.action = action_setLowBatteryWarnAt
		});
		list_addItem(&_menu_user_interface, (ListItem){
			.label = "Theme overrides...",
			.action = menu_themeOverrides
		});
	}
	menu_stack[++level] = &_menu_user_interface;
	header_changed = true;
}

void menu_resetSettings(void *_)
{
	if (!_menu_reset_settings._created) {
		_menu_reset_settings = list_create(5, LIST_SMALL);
		strcpy(_menu_reset_settings.title, "Reset settings");
		list_addItem(&_menu_reset_settings, (ListItem){
			.label = "Reset all"
		});
		list_addItem(&_menu_reset_settings, (ListItem){
			.label = "Reset tweaks"
		});
		list_addItem(&_menu_reset_settings, (ListItem){
			.label = "Reset MainUI settings"
		});
		list_addItem(&_menu_reset_settings, (ListItem){
			.label = "Reset RetroArch main configuration"
		});
		list_addItem(&_menu_reset_settings, (ListItem){
			.label = "Reset RetroArch core configuration..." // TODO: This needs to lead to a dynamic menu listing the cores
		});
	}
	menu_stack[++level] = &_menu_reset_settings;
	header_changed = true;
}

void menu_advanced(void *_)
{
	if (!_menu_advanced._created) {
		_menu_advanced = list_create(3, LIST_SMALL);
		strcpy(_menu_advanced.title, "Advanced");
		list_addItem(&_menu_advanced, (ListItem){
			.label = "Fast forward multiplier", .item_type = MULTIVALUE, .value_max = 50, .value_formatter = formatter_fastForward
		});
		list_addItem(&_menu_advanced, (ListItem){
			.label = "Swap L/R with L2/R2", .item_type = TOGGLE
		});
		list_addItem(&_menu_advanced, (ListItem){
			.label = "Reset settings...", .action = menu_resetSettings
		});
	}
	menu_stack[++level] = &_menu_advanced;
	header_changed = true;
}

void menu_tools(void *_)
{
	if (!_menu_tools._created) {
		_menu_tools = list_create(5, LIST_SMALL);
		strcpy(_menu_tools.title, "Tools");
		list_addItem(&_menu_tools, (ListItem){
			.label = "Favorites: Sort alphabetically",
			.action = tool_favoritesSortAlpha
		});
		list_addItem(&_menu_tools, (ListItem){
			.label = "Favorites: Sort by system",
			.action = tool_favoritesSortSystem
		});
		list_addItem(&_menu_tools, (ListItem){
			.label = "Favorites: Fix thumbnails",
			.action = tool_favoritesFixThumbnails
		});
		// list_addItem(&_menu_tools, (ListItem){
		// 	.label = "Remove apps from recents",
		// 	.action = tool_recentsRemoveApps
		// }); // TODO: Currently not working! - implement new efficient method
		list_addItem(&_menu_tools, (ListItem){
			.label = "Remove OSX system files",
			.action = tool_removeDotUnderscoreFiles
		});
	}
	menu_stack[++level] = &_menu_tools;
	header_changed = true;
}

void menu_main(void)
{
	if (!_menu_main._created) {
		_menu_main = list_create(5, LIST_LARGE);
		strcpy(_menu_main.title, "Tweaks");
		list_addItem(&_menu_main, (ListItem){
			.label = "System",
			.description = "Startup, save and exit, vibration",
			.action = menu_system,
			.icon_ptr = (void*)IMG_Load("res/tweaks_system.png")
		});
		list_addItem(&_menu_main, (ListItem){
			.label = "Menu button",
			.description = "Customize menu button actions",
			.action = menu_buttonAction,
			.icon_ptr = (void*)IMG_Load("res/tweaks_menu_button.png")
		});
		list_addItem(&_menu_main, (ListItem){
			.label = "User interface",
			.description = "Extra menus, low batt. warn., theme",
			.action = menu_userInterface,
			.icon_ptr = (void*)IMG_Load("res/tweaks_user_interface.png")
		});
		list_addItem(&_menu_main, (ListItem){
			.label = "Advanced",
			.description = "Emulator tweaks, reset settings",
			.action = menu_advanced,
			.icon_ptr = (void*)IMG_Load("res/tweaks_advanced.png")
		});
		list_addItem(&_menu_main, (ListItem){
			.label = "Tools",
			.description = "Favorites, clean files",
			.action = menu_tools,
			.icon_ptr = (void*)IMG_Load("res/tweaks_tools.png")
		});
	}
	menu_stack[0] = &_menu_main;
	header_changed = true;
}

#endif