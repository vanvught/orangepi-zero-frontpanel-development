/*
 * localconfig.cpp
 */

#include <cstdint>
#include <cstdio>
#include <cassert>

#include "localconfig.h"

#include "display.h"
#include "remoteconfig.h"
#include "network.h"
#include "artnetnode.h"
#include "lightset.h"

extern bool g_bRun;

enum class State {
	MAIN, ITEM, EDIT, QUIT
};

enum class StateEditor {
	START, ROW_TOP, ROW_TOP_QUIT, ROW_BOTTEM_EDITING
};

enum class EditorType {
	BOOL, BOOL_NO_RETURN, BOOL_WITH_PORT, UNIVERSE, PROTOCOL, DIRECTION, MERGE, IP_ADDRESS
};

struct MenuItem {
	const char *pLabel;
	State nextState;
	const void *items;
};

struct MenuItems {
	const struct MenuItem *items;
	const uint32_t nItems;
};

struct Editor {
	EditorType type;
	union {
		bool (*get_bool) ();
		bool (*get_bool_with_port) (const uint32_t);
	};
	union {
		bool (*set_bool) (bool);
		void (*set_bool_no_return) (bool);
		void (*set_bool_with_port) (const uint32_t, const bool);
	};
};

namespace remoteconfig {
static void SetDisable(bool b) {
	RemoteConfig::Get()->SetDisable(b);
}
static bool GetDisable() {
	return RemoteConfig::Get()->GetDisable();
}
static void SetDisableWrite(bool b) {
	RemoteConfig::Get()->SetDisableWrite(b);
}
static void SetEnableReboot(bool b) {
	RemoteConfig::Get()->SetEnableReboot(b);
}
static void SetEnableUptime(bool b) {
	RemoteConfig::Get()->SetEnableUptime(b);
}
static void SetEnableFactory(bool b) {
	RemoteConfig::Get()->SetEnableFactory(b);
}
static bool GetEnableFactory() {
	return RemoteConfig::Get()->GetEnableFactory();
}
}  // namespace remoteconfig

namespace network {
static bool IsDhcpUsed() {
	return Network::Get()->IsDhcpUsed();
}
static bool EnableDhcp(bool b) {
	if (b) {
		Network::Get()->EnableDhcp();
	}
	return Network::Get()->IsDhcpUsed();
}
}  // namespace network

namespace artnet {
static void SetRdm(const uint32_t nPortIndex, const bool bEnable) {
	ArtNetNode::Get()->SetRdm(nPortIndex, bEnable);
}

static bool GetRdm(const uint32_t nPortIndex) {
	return ArtNetNode::Get()->GetRdm(nPortIndex);
}
}  // namespace artnet

static constexpr Editor RemoteConfigDisable      = { EditorType::BOOL_NO_RETURN, .get_bool = remoteconfig::GetDisable, .set_bool_no_return = remoteconfig::SetDisable };
static constexpr Editor RemoteConfiDisableWrite  = { EditorType::BOOL_NO_RETURN, .get_bool = remoteconfig::GetDisable, .set_bool_no_return = remoteconfig::SetDisableWrite };
static constexpr Editor RemoteConfigEnableReboot = { EditorType::BOOL_NO_RETURN, .get_bool = remoteconfig::GetDisable, .set_bool_no_return = remoteconfig::SetEnableReboot };
static constexpr Editor RemoteConfigEnableUptime = { EditorType::BOOL_NO_RETURN, .get_bool = remoteconfig::GetDisable, .set_bool_no_return = remoteconfig::SetEnableUptime };
static constexpr Editor RemoteConfigEnableFactory= { EditorType::BOOL_NO_RETURN, .get_bool = remoteconfig::GetEnableFactory, .set_bool_no_return = remoteconfig::SetEnableFactory };

static constexpr struct MenuItem remoteconfigmenu[] = {
		{ "Disable", State::EDIT, &RemoteConfigDisable },
		{ "Disable write", State::EDIT, &RemoteConfiDisableWrite },
		{ "Enable reboot", State::EDIT, &RemoteConfigEnableReboot },
		{ "Enable uptime", State::EDIT, &RemoteConfigEnableUptime },
		{ "Enable factory", State::EDIT, &RemoteConfigEnableFactory  }
};

static constexpr struct MenuItems remoteconfigmenus = { remoteconfigmenu, 5 };

static constexpr Editor NetworkUseDHCP = { EditorType::BOOL, .get_bool = network::IsDhcpUsed, .set_bool = network::EnableDhcp };

static constexpr struct MenuItem networkmenu[] = {
		{ "Use DHCP", State::EDIT, &NetworkUseDHCP },
		{ "DHCP Retry", State::EDIT, nullptr  },
		{ "IP Address", State::EDIT, nullptr  },
		{ "Netmask", State::EDIT, nullptr  },
		{ "Gateway", State::EDIT, nullptr  }
};

static constexpr struct MenuItems networkmenuitems = { networkmenu, 5 };

static constexpr Editor ArtNetRdm = { EditorType::BOOL_WITH_PORT, .get_bool_with_port = artnet::GetRdm , .set_bool_with_port = artnet::SetRdm };

static constexpr struct MenuItem artnetportmenu[] = {
		{ "Universe", State::EDIT, nullptr },
		{ "Direction", State::EDIT, nullptr },
		{ "Merge", State::EDIT, nullptr },
		{ "Protocol", State::EDIT, nullptr },
		{ "Destination IP", State::EDIT, nullptr },
		{ "RDM", State::EDIT, &ArtNetRdm }
};

static constexpr struct MenuItems artnetportitems = { artnetportmenu, 6 };

static constexpr struct MenuItem artnetmenu[] = {
		{ "Port A", State::MAIN, &artnetportitems },
		{ "Port B", State::MAIN, &artnetportitems },
		{ "Port C", State::MAIN, &artnetportitems },
		{ "Port D", State::MAIN, &artnetportitems },
		{ "Map Universe 0", State::MAIN, nullptr },
};

static constexpr struct MenuItems artnetmenuitems = { artnetmenu, 5 };

static constexpr struct MenuItem menu[] = {
		{ "Remote config", State::ITEM, &remoteconfigmenus},
		{ "Display UDF", State::ITEM, nullptr},
		{ "Network", State::ITEM, &networkmenuitems },
		{ "ArtNet 4", State::ITEM, &artnetmenuitems }
};

static constexpr struct MenuItems menuitems = { menu, 4 };

static uint32_t s_nIndex;
static uint32_t s_nIndexItem;
static auto *currentMenu = reinterpret_cast<const void *>(&menuitems);
static constexpr uint32_t MAX_MENU_LEVELS = 4;
static void *menuStack[MAX_MENU_LEVELS];
static uint32_t indexStack[MAX_MENU_LEVELS];
static int32_t menuStackTop = -1;
static State s_State;
static StateEditor s_StateEditor;

static void push() {
	if (menuStackTop == MAX_MENU_LEVELS - 1) {
		assert(0);
	} else {
		menuStackTop++;
		indexStack[menuStackTop] = s_nIndex;
		menuStack[menuStackTop] = const_cast<void *>(reinterpret_cast<const void *>(currentMenu));
	}
}

static void pop() {
	if (menuStackTop == -1) {
		assert(0);
	} else {
		s_nIndex = indexStack[menuStackTop];
		currentMenu = reinterpret_cast<struct MenuItems *>(menuStack[menuStackTop]);
		menuStackTop--;
	}
}

void debug_state() {
	char s[6];

	s[0] = '0' + static_cast<char>(s_nIndex);
	s[1] = '0' + static_cast<char>(s_nIndexItem);
	s[2] = '0' + static_cast<char>(s_State);
	s[3] = '0' + static_cast<char>(s_StateEditor);
	s[4] = menuStackTop == -1 ? ' ' : '0' + menuStackTop;
	s[5] = 0;

	Display::Get()->TextStatus(s);
}

static void show_menu() {
	Display::Get()->Cls();

	const auto menu = reinterpret_cast<const MenuItems *>(currentMenu);

	for (uint32_t nItem = 0; nItem < menu->nItems; nItem++) {
		Display::Get()->SetCursorPos(1, nItem);
		Display::Get()->PutString(menu->items[nItem].pLabel);
	}

	Display::Get()->SetCursorPos(0, s_nIndex);
	Display::Get()->PutString(">");

	debug_state();
}

static void editor_bool(const int c) {
	const auto menu = reinterpret_cast<const MenuItems*>(currentMenu);
	const auto editor = reinterpret_cast<const Editor*>(menu->items[s_nIndex].items);

	static bool b;

	if (s_StateEditor == StateEditor::START) {
		Display::Get()->SetCursorPos(2, 1);

		switch (editor->type) {
		case EditorType::BOOL:
		case EditorType::BOOL_NO_RETURN:
			b = editor->get_bool();
			break;
		case EditorType::BOOL_WITH_PORT:
			b = editor->get_bool_with_port(s_nIndexItem);
			break;
		default:
			assert(0);
			break;
		} // switch (editor->type)

		if (b) {
			Display::Get()->PutString("Y");
		} else {
			Display::Get()->PutString("N");
		}
		return;
	}

	if (s_StateEditor == StateEditor::ROW_BOTTEM_EDITING) {
		Display::Get()->SetCursorPos(2, 1);

		if ((c == KEY_UP) || (c == KEY_DOWN)) {
			b = !b;
			if (b) {
				Display::Get()->PutString("Y");
			} else {
				Display::Get()->PutString("N");
			}
			return;
		}

		if (c == 10) {
			switch (editor->type) {
			case EditorType::BOOL:
				editor->set_bool(b);
				break;
			case EditorType::BOOL_NO_RETURN:
				editor->set_bool_no_return(b);
				break;
			case EditorType::BOOL_WITH_PORT:
				editor->set_bool_with_port(s_nIndexItem, b);
				break;
			default:
				assert(0);
				break;
			}

			s_StateEditor = StateEditor::ROW_TOP;
			return;
		}
	}
}

static void editor(const int c ) {
	debug_state();

	const auto menu = reinterpret_cast<const MenuItems *>(currentMenu);
	const auto editor = reinterpret_cast<const Editor *>(menu->items[s_nIndex].items);

	Display::Get()->SetCursorPos(0, 4);

	switch (editor->type) {
	case EditorType::BOOL:
		Display::Get()->PutString("BOOL          ");
		break;
	case EditorType::BOOL_NO_RETURN:
		Display::Get()->PutString("BOOL NO RETURN");
		break;
	case EditorType::BOOL_WITH_PORT:
		Display::Get()->PutString("BOOL WITH PORT");
		break;
	default:
		Display::Get()->PutString("UNDEF         ");
		break;
	}

	switch (s_StateEditor) {
	case StateEditor::START:
		Display::Get()->Cls();
		Display::Get()->SetCursorPos(0, 0);
		Display::Get()->PutString(">");
		Display::Get()->PutString(menu->items[s_nIndex].pLabel);

		switch (editor->type) {
		case EditorType::BOOL:
		case EditorType::BOOL_NO_RETURN:
		case EditorType::BOOL_WITH_PORT:
			editor_bool(c);
			break;
		default:
			break;
		}

		s_StateEditor = StateEditor::ROW_TOP;
		debug_state();
		return;
		break;
	case StateEditor::ROW_TOP:
		if (c == KEY_UP) {
			s_StateEditor = StateEditor::ROW_TOP_QUIT;
			Display::Get()->SetCursorPos(0, 0);
			Display::Get()->PutString("<");
			return;
		} else if (c == 10) {
			s_StateEditor = StateEditor::ROW_BOTTEM_EDITING;
			Display::Get()->SetCursorPos(0, 0);
			Display::Get()->PutString(" ");
			Display::Get()->SetCursorPos(0, 1);
			Display::Get()->PutString(">");
			return;
		}
		break;
	case StateEditor::ROW_TOP_QUIT:
		if (c == 10) {
			pop();
			s_StateEditor = StateEditor::START;
			s_State = State::ITEM;
			show_menu();
			return;
		} else if (c == KEY_DOWN) {
			s_StateEditor = StateEditor::ROW_TOP;
			Display::Get()->SetCursorPos(0, 0);
			Display::Get()->PutString(">");
			return;
		}
		break;
	case StateEditor::ROW_BOTTEM_EDITING:
		switch (editor->type) {
		case EditorType::BOOL:
		case EditorType::BOOL_NO_RETURN:
		case EditorType::BOOL_WITH_PORT:
			editor_bool(c);
			if (s_StateEditor == StateEditor::ROW_TOP) {
				Display::Get()->SetCursorPos(0, 0);
				Display::Get()->PutString(">");
				Display::Get()->SetCursorPos(0, 1);
				Display::Get()->PutString(" ");
			}
			return;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	Display::Get()->SetCursorPos(0, 3);

	switch (c) {
	case KEY_UP:
		Display::Get()->PutString("UP   ");
		break;
	case KEY_DOWN:
		Display::Get()->PutString("DOWN ");
		break;
	case 10: // Enter
		Display::Get()->PutString("ENTER");
		break;
	default:
		Display::Get()->PutString("<   >");
		break;
	}
}

LocalConfig::LocalConfig() {
	show_menu();
}

void LocalConfig::Run() {
	if (!Display::Get()->IsAvailable()) {
		return;
	}

	const auto c = Display::Get()->GetChar();

	if (c == 'q') {
		g_bRun = false;
		return;
	}

	if (c == KEY_UP) {
		debug_state();

		switch (s_State) {
		case State::MAIN:
		case State::ITEM:
			if (s_nIndex > 0) {
				s_nIndex--;
			} else if (s_nIndex == 0) {
				Display::Get()->SetCursorPos(0, s_nIndex);
				Display::Get()->PutString("<");
				s_State = State::QUIT;
				return;
			}
			show_menu();
			return;
			break;
		case State::EDIT:
			editor(c);
			return;
		default:
			break;
		}
		return;
	} else if (c == KEY_DOWN) {
		debug_state();

		switch (s_State) {
		case State::MAIN:
		case State::ITEM: {
			const auto menu = reinterpret_cast<const MenuItems *>(currentMenu);
			if (s_nIndex == (menu->nItems - 1)) {
				s_nIndex = 0;
			} else if (s_nIndex < (menu->nItems - 1)) {
				s_nIndex++;
			}
			show_menu();
			return;
		}
		break;
		case State::EDIT:
			editor(c);
			return;
		case State::QUIT:
			Display::Get()->SetCursorPos(0, s_nIndex);
			Display::Get()->PutString(">");
			s_State = State::MAIN;
			debug_state();
			return;
			break;
		default:
			break;
		}
		return;
	} else if (c == 10) { // ENTER
		debug_state();

		switch (s_State) {
		case State::MAIN:
		case State::ITEM: {
			const auto menu = reinterpret_cast<const MenuItems *>(currentMenu);
			if (menu->items[s_nIndex].items != nullptr) {
				s_State = menu->items[s_nIndex].nextState;
				push();
				switch (s_State) {
				case State::MAIN:
				case State::ITEM:
					if (s_State == State::MAIN) {
						s_nIndexItem = s_nIndex;
					}
					currentMenu = menu->items[s_nIndex].items;
					s_nIndex = 0;
					show_menu();
					return;
					break;
				case State::EDIT:
					editor(c);
					return;
					break;
				case State::QUIT:
					break;
				default:
					break;
				}
				show_menu();
				return;
			}
		}
		break;
		case State::EDIT:
			editor(c);
			return;
		case State::QUIT: {
			if (&menuitems == currentMenu) {
				g_bRun = false;
				return;
			}
			pop();
			const auto menu = reinterpret_cast<const MenuItems *>(currentMenu);
			s_State = menu->items[s_nIndex].nextState;
			show_menu();
			return;
		}
		break;
		default:
			break;
		}
		return;
	}
}
