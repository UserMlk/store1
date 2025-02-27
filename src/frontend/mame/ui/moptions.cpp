// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

    ui/moptions.cpp

    UI main options manager.

***************************************************************************/

#include "emu.h"
#include "options.h"
#include "ui/moptions.h"


//**************************************************************************
//  UI EXTRA OPTIONS
//**************************************************************************

const options_entry ui_options::s_option_entries[] =
{
	// search path options
	{ nullptr,                              nullptr,                       OPTION_HEADER,  "UI SEARCH PATH OPTIONS" },
	{ OPTION_HISTORY_PATH,                  "history;dats;.",              OPTION_STRING,  "path to system/software info files" },
	{ OPTION_CATEGORYINI_PATH,              "folders",                     OPTION_STRING,  "path to category ini files" },
	{ OPTION_CABINETS_PATH,                 "cabinets;cabdevs",            OPTION_STRING,  "path to cabinets / devices image" },
	{ OPTION_CPANELS_PATH,                  "cpanel",                      OPTION_STRING,  "path to control panel image" },
	{ OPTION_PCBS_PATH,                     "pcb",                         OPTION_STRING,  "path to pcbs image" },
	{ OPTION_FLYERS_PATH,                   "flyers",                      OPTION_STRING,  "path to flyers image" },
	{ OPTION_TITLES_PATH,                   "titles",                      OPTION_STRING,  "path to titles image" },
	{ OPTION_ENDS_PATH,                     "ends",                        OPTION_STRING,  "path to ends image" },
	{ OPTION_MARQUEES_PATH,                 "marquees",                    OPTION_STRING,  "path to marquees image" },
	{ OPTION_ARTPREV_PATH,                  "artwork preview;artpreview",  OPTION_STRING,  "path to artwork preview image" },
	{ OPTION_BOSSES_PATH,                   "bosses",                      OPTION_STRING,  "path to bosses image" },
	{ OPTION_LOGOS_PATH,                    "logo",                        OPTION_STRING,  "path to logos image" },
	{ OPTION_SCORES_PATH,                   "scores",                      OPTION_STRING,  "path to scores image" },
	{ OPTION_VERSUS_PATH,                   "versus",                      OPTION_STRING,  "path to versus image" },
	{ OPTION_GAMEOVER_PATH,                 "gameover",                    OPTION_STRING,  "path to gameover image" },
	{ OPTION_HOWTO_PATH,                    "howto",                       OPTION_STRING,  "path to howto image" },
	{ OPTION_SELECT_PATH,                   "select",                      OPTION_STRING,  "path to select image" },
	{ OPTION_ICONS_PATH,                    "icons",                       OPTION_STRING,  "path to ICOns image" },
	{ OPTION_COVER_PATH,                    "covers",                      OPTION_STRING,  "path to software cover image" },
	{ OPTION_UI_PATH,                       "ui",                          OPTION_STRING,  "path to UI files" },

	// misc options
	{ nullptr,                              nullptr,    OPTION_HEADER,      "UI MISC OPTIONS" },
	{ OPTION_SYSTEM_NAMES,                  "",         OPTION_STRING,      "translated system names file" },
	{ OPTION_SKIP_WARNINGS,                 "0",        OPTION_BOOLEAN,     "display fewer repeated warnings about imperfect emulation" },
	{ OPTION_REMEMBER_LAST,                 "1",        OPTION_BOOLEAN,     "initially select last used system in main menu" },
	{ OPTION_ENLARGE_SNAPS,                 "1",        OPTION_BOOLEAN,     "enlarge artwork (snapshot, title, etc.) in right panel (keeping aspect ratio)" },
	{ OPTION_FORCED4X3,                     "1",        OPTION_BOOLEAN,     "force the appearance of the snapshot in the list software to 4:3" },
	{ OPTION_USE_BACKGROUND,                "1",        OPTION_BOOLEAN,     "enable background image in main view" },
	{ OPTION_SKIP_BIOS_MENU,                "0",        OPTION_BOOLEAN,     "skip bios submenu, start with configured or default" },
	{ OPTION_SKIP_PARTS_MENU,               "0",        OPTION_BOOLEAN,     "skip parts submenu, start with first part" },
	{ OPTION_LAST_USED_FILTER,              "",         OPTION_STRING,      "latest used filter" },
	{ OPTION_LAST_RIGHT_PANEL "(0-1)",      "0",        OPTION_INTEGER,     "latest right panel focus" },
	{ OPTION_LAST_USED_MACHINE,             "",         OPTION_STRING,      "latest used machine" },
	{ OPTION_INFO_AUTO_AUDIT,               "0",        OPTION_BOOLEAN,     "enable auto audit in the general info panel" },
	{ OPTION_HIDE_ROMLESS,                  "1",        OPTION_BOOLEAN,     "hide romless machine from available list" },
	{ OPTION_UNTHROTTLE_MUTE ";utm",        "0",        OPTION_BOOLEAN,     "mute audio when running unthrottled" },

	// UI options
	{ nullptr,                              nullptr,        OPTION_HEADER,      "UI OPTIONS" },
	{ OPTION_INFOS_SIZE "(0.20-1.00)",      "0.75",         OPTION_FLOAT,       "UI right panel infos text size (0.20 - 1.00)" },
	{ OPTION_FONT_ROWS "(25-40)",           "30",           OPTION_INTEGER,     "UI font lines per screen (25 - 40)" },
	{ OPTION_HIDE_PANELS "(0-3)",           "0",            OPTION_INTEGER,     "UI hide left/right panel in main view (0 = Show all, 1 = hide left, 2 = hide right, 3 = hide both" },
	{ OPTION_UI_BORDER_COLOR,               "ff000000",     OPTION_STRING,      "UI border color (ARGB)" },
	{ OPTION_UI_BACKGROUND_COLOR,           "efefefef",     OPTION_STRING,      "UI background color (ARGB)" },
	{ OPTION_UI_CLONE_COLOR,                "ff7f7f7f",     OPTION_STRING,      "UI clone color (ARGB)" },
	{ OPTION_UI_DIPSW_COLOR,                "ff7f7f7f",     OPTION_STRING,      "UI dipswitch color (ARGB)" },
	{ OPTION_UI_GFXVIEWER_BG_COLOR,         "ef000000",     OPTION_STRING,      "UI gfx viewer color (ARGB)" },
	{ OPTION_UI_MOUSEDOWN_BG_COLOR,         "ef00a2e8",     OPTION_STRING,      "UI mouse down bg color (ARGB)" },
	{ OPTION_UI_MOUSEDOWN_COLOR,            "fffff200",     OPTION_STRING,      "UI mouse down color (ARGB)" },
	{ OPTION_UI_MOUSEOVER_BG_COLOR,         "ef00a2e8",     OPTION_STRING,      "UI mouse over bg color (ARGB)" },
	{ OPTION_UI_MOUSEOVER_COLOR,            "fffff200",     OPTION_STRING,      "UI mouse over color (ARGB)" },
	{ OPTION_UI_SELECTED_BG_COLOR,          "ef3f48cc",     OPTION_STRING,      "UI selected bg color (ARGB)" },
	{ OPTION_UI_SELECTED_COLOR,             "fffff200",     OPTION_STRING,      "UI selected color (ARGB)" },
	{ OPTION_UI_SLIDER_COLOR,               "ef22b14c",     OPTION_STRING,      "UI slider color (ARGB)" },
	{ OPTION_UI_SUBITEM_COLOR,              "ef3f48cc",     OPTION_STRING,      "UI subitem color (ARGB)" },
	{ OPTION_UI_TEXT_BG_COLOR,              "efefefef",     OPTION_STRING,      "UI text bg color (ARGB)" },
	{ OPTION_UI_TEXT_COLOR,                 "ff000000",     OPTION_STRING,      "UI text color (ARGB)" },
	{ OPTION_UI_UNAVAILABLE_COLOR,          "ff7f7f7f",     OPTION_STRING,      "UI unavailable color (ARGB)" },
	{ nullptr }
};

//-------------------------------------------------
//  ui_options - constructor
//-------------------------------------------------

ui_options::ui_options() : core_options()
{
	add_entries(ui_options::s_option_entries);
}

//-------------------------------------------------
//  rgb_value - decode an RGB option
//-------------------------------------------------

rgb_t ui_options::rgb_value(const char *option) const
{
	// find the entry
	core_options::entry::shared_const_ptr entry = get_entry(option);

	// look up the value, and sanity check the result
	const char *value = entry->value();
	int len = strlen(value);
	if (len != 8)
		value = entry->default_value().c_str();

	// convert to an rgb_t
	return rgb_t((uint32_t)strtoul(value, nullptr, 16));
}
