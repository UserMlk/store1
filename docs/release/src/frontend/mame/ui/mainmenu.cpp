// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles, Nathan Woods
/*********************************************************************

    ui/mainmenu.cpp

    Internal MAME menus for the user interface.

*********************************************************************/

#include "emu.h"
#include "ui/mainmenu.h"

#include "ui/about.h"
#include "ui/analogipt.h"
#include "ui/barcode.h"
#include "ui/cheatopt.h"
#include "ui/confswitch.h"
#include "ui/datmenu.h"
#include "ui/filemngr.h"
#include "ui/info.h"
#include "ui/info_pty.h"
#include "ui/inifile.h"
#include "ui/inputmap.h"
#include "ui/keyboard.h"
#include "ui/miscmenu.h"
#include "ui/pluginopt.h"
#include "ui/selgame.h"
#include "ui/simpleselgame.h"
#include "ui/sliders.h"
#include "ui/slotopt.h"
#include "ui/tapectrl.h"
#include "ui/videoopt.h"

#include "mame.h"
#include "luaengine.h"

#include "machine/bcreader.h"
#include "imagedev/cassette.h"

#include "crsshair.h"
#include "dipty.h"
#include "emuopts.h"
#include "natkeyboard.h"


namespace ui {

enum : unsigned {
	INPUT_GROUPS,
	INPUT_SPECIFIC,
	SETTINGS_DIP_SWITCHES,
	SETTINGS_DRIVER_CONFIG,
	ANALOG,
	BOOKKEEPING,
	GAME_INFO,
	WARN_INFO,
	IMAGE_MENU_IMAGE_INFO,
	IMAGE_MENU_FILE_MANAGER,
	TAPE_CONTROL,
	SLOT_DEVICES,
	NETWORK_DEVICES,
	KEYBOARD_MODE,
	SLIDERS,
	VIDEO_TARGETS,
	VIDEO_OPTIONS,
	CROSSHAIR,
	CHEAT,
	PLUGINS,
	BIOS_SELECTION,
	BARCODE_READ,
	PTY_INFO,
	EXTERNAL_DATS,
	ADD_FAVORITE,
	REMOVE_FAVORITE,
	ABOUT,
	QUIT_GAME,
	DISMISS,
	SELECT_GAME
};

/***************************************************************************
    MENU HANDLERS
***************************************************************************/

/*-------------------------------------------------
    menu_main constructor/destructor
-------------------------------------------------*/

menu_main::menu_main(mame_ui_manager &mui, render_container &container)
	: menu(mui, container)
	, m_phase(machine_phase::PREINIT)
{
	set_needs_prev_menu_item(false);
}

menu_main::~menu_main()
{
}


/*-------------------------------------------------
    menu_activated - handle coming to foreground
-------------------------------------------------*/

void menu_main::menu_activated()
{
	if (machine().phase() != m_phase)
		reset(reset_options::REMEMBER_REF);
}


/*-------------------------------------------------
    populate - populate main menu items
-------------------------------------------------*/

void menu_main::populate(float &customtop, float &custombottom)
{
	m_phase = machine().phase();

	item_append(_("Input (general)"), 0, (void *)INPUT_GROUPS);

	item_append(_("Input (this machine)"), 0, (void *)INPUT_SPECIFIC);

	if (ui().machine_info().has_analog())
		item_append(_("Analog Controls"), 0, (void *)ANALOG);
	if (ui().machine_info().has_dips())
		item_append(_("DIP Switches"), 0, (void *)SETTINGS_DIP_SWITCHES);
	if (ui().machine_info().has_configs())
		item_append(_("Machine Configuration"), 0, (void *)SETTINGS_DRIVER_CONFIG);

	item_append(_("Bookkeeping Info"), 0, (void *)BOOKKEEPING);

	item_append(_("Machine Information"), 0, (void *)GAME_INFO);

	if (ui().found_machine_warnings())
		item_append(_("Warning Information"), 0, (void *)WARN_INFO);

	for (device_image_interface &image : image_interface_enumerator(machine().root_device()))
	{
		if (image.user_loadable())
		{
			item_append(_("Image Information"), 0, (void *)IMAGE_MENU_IMAGE_INFO);

			item_append(_("File Manager"), 0, (void *)IMAGE_MENU_FILE_MANAGER);

			break;
		}
	}

	if (cassette_device_enumerator(machine().root_device()).first() != nullptr)
		item_append(_("Tape Control"), 0, (void *)TAPE_CONTROL);

	if (pty_interface_enumerator(machine().root_device()).first() != nullptr)
		item_append(_("Pseudo Terminals"), 0, (void *)PTY_INFO);

	if (ui().machine_info().has_bioses())
		item_append(_("BIOS Selection"), 0, (void *)BIOS_SELECTION);

	if (slot_interface_enumerator(machine().root_device()).first() != nullptr)
		item_append(_("Slot Devices"), 0, (void *)SLOT_DEVICES);

	if (barcode_reader_device_enumerator(machine().root_device()).first() != nullptr)
		item_append(_("Barcode Reader"), 0, (void *)BARCODE_READ);

	if (network_interface_enumerator(machine().root_device()).first() != nullptr)
		item_append(_("Network Devices"), 0, (void*)NETWORK_DEVICES);

	if (machine().natkeyboard().keyboard_count())
		item_append(_("Keyboard Mode"), 0, (void *)KEYBOARD_MODE);

	item_append(_("Slider Controls"), 0, (void *)SLIDERS);

	item_append(_("Video Options"), 0, (void *)VIDEO_TARGETS);

	if (machine().crosshair().get_usage())
		item_append(_("Crosshair Options"), 0, (void *)CROSSHAIR);

	if (machine().options().cheat())
		item_append(_("Cheat"), 0, (void *)CHEAT);

	if (machine_phase::RESET <= m_phase)
	{
		if (machine().options().plugins() && !mame_machine_manager::instance()->lua()->get_menu().empty())
			item_append(_("Plugin Options"), 0, (void *)PLUGINS);

		if (mame_machine_manager::instance()->lua()->call_plugin_check<const char *>("data_list", "", true))
			item_append(_("External DAT View"), 0, (void *)EXTERNAL_DATS);
	}

// MAMEFX items commented out
//	item_append(menu_item_type::SEPARATOR);

	/* add favorite menu */
//	if (!mame_machine_manager::instance()->favorite().isgame_favorite())
//		item_append(_("Add To Favorites"), "", 0, (void *)ADD_FAVORITE);
//	else
//		item_append(_("Remove From Favorites"), "", 0, (void *)REMOVE_FAVORITE);

//	item_append(menu_item_type::SEPARATOR);

//	item_append(_("Quit from Machine"), "", 0, (void *)QUIT_GAME);
//	item_append(string_format(_("About %1%s"), emulator_info::get_appname()), 0, (void *)ABOUT);

//	item_append(menu_item_type::SEPARATOR);

//  item_append(_("Quit from Machine"), 0, (void *)QUIT_GAME);

//	if (machine_phase::INIT == m_phase)
//	{
//		item_append(_("Start Machine"), 0, (void *)DISMISS);
//	}
//	else
//	{
//		item_append(_("Select New Machine"), 0, (void *)SELECT_GAME);
//		item_append(_("Return to Machine"), 0, (void *)DISMISS);
//	}
}


/*-------------------------------------------------
    handle - handle main menu events
-------------------------------------------------*/

void menu_main::handle(event const *ev)
{
	// process the menu
	if (ev && (ev->iptkey == IPT_UI_SELECT))
	{
		switch (uintptr_t(ev->itemref))
		{
		case INPUT_GROUPS:
			menu::stack_push<menu_input_groups>(ui(), container());
			break;

		case INPUT_SPECIFIC:
			menu::stack_push<menu_input_specific>(ui(), container());
			break;

		case SETTINGS_DIP_SWITCHES:
			menu::stack_push<menu_settings_dip_switches>(ui(), container());
			break;

		case SETTINGS_DRIVER_CONFIG:
			menu::stack_push<menu_settings_machine_config>(ui(), container());
			break;

		case ANALOG:
			menu::stack_push<menu_analog>(ui(), container());
			break;

		case BOOKKEEPING:
			menu::stack_push<menu_bookkeeping>(ui(), container());
			break;

		case GAME_INFO:
			menu::stack_push<menu_game_info>(ui(), container());
			break;

		case WARN_INFO:
			menu::stack_push<menu_warn_info>(ui(), container());
			break;

		case IMAGE_MENU_IMAGE_INFO:
			menu::stack_push<menu_image_info>(ui(), container());
			break;

		case IMAGE_MENU_FILE_MANAGER:
			menu::stack_push<menu_file_manager>(ui(), container(), nullptr);
			break;

		case TAPE_CONTROL:
			menu::stack_push<menu_tape_control>(ui(), container(), nullptr);
			break;

		case PTY_INFO:
			menu::stack_push<menu_pty_info>(ui(), container());
			break;

		case SLOT_DEVICES:
			menu::stack_push<menu_slot_devices>(ui(), container());
			break;

		case NETWORK_DEVICES:
			menu::stack_push<menu_network_devices>(ui(), container());
			break;

		case KEYBOARD_MODE:
			menu::stack_push<menu_keyboard_mode>(ui(), container());
			break;

		case SLIDERS:
			menu::stack_push<menu_sliders>(ui(), container(), false);
			break;

		case VIDEO_TARGETS:
			menu::stack_push<menu_video_targets>(ui(), container());
			break;

		case VIDEO_OPTIONS:
			menu::stack_push<menu_video_options>(ui(), container(), *machine().render().first_target(), false);
			break;

		case CROSSHAIR:
			menu::stack_push<menu_crosshair>(ui(), container());
			break;

		case CHEAT:
			menu::stack_push<menu_cheat>(ui(), container());
			break;

		case PLUGINS:
			menu::stack_push<menu_plugin>(ui(), container());
			break;

		case SELECT_GAME:
			if (machine().options().ui() == emu_options::UI_SIMPLE)
				menu::stack_push<simple_menu_select_game>(ui(), container(), nullptr);
			else
				menu::stack_push<menu_select_game>(ui(), container(), nullptr);
			break;

		case ABOUT:
			menu::stack_push<menu_about>(ui(), container());
			break;

		case BIOS_SELECTION:
			menu::stack_push<menu_bios_selection>(ui(), container());
			break;

		case BARCODE_READ:
			menu::stack_push<menu_barcode_reader>(ui(), container(), nullptr);
			break;

		case EXTERNAL_DATS:
			menu::stack_push<menu_dats_view>(ui(), container());
			break;

		case ADD_FAVORITE:
			mame_machine_manager::instance()->favorite().add_favorite(machine());
			reset(reset_options::REMEMBER_REF);
			break;

		case REMOVE_FAVORITE:
			mame_machine_manager::instance()->favorite().remove_favorite(machine());
			reset(reset_options::REMEMBER_REF);
			break;

		case QUIT_GAME:
			stack_pop();
			ui().request_quit();
			break;

		case DISMISS:
			stack_pop();
			return;

		default:
			fatalerror("ui::menu_main::handle - unknown reference\n");
		}
	}
}

} // namespace ui
