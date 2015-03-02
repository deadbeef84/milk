#include "milk/input.h"
#include "milk/error.h"
#include "milk/iwindow.h"
#include "milk/iapplication.h"
#include "milk/obsolete/milk_ccontrol.h"
#include "milk/helper.h"
#include "milk/vectorpod.h"
#include "milk/includes.h"
#include "milk/assert.h"
#include "milk/math/math.h"
#include <algorithm>
#include "milk/boost.h"

using namespace milk;
using namespace std;

string CKey::ms_names[KEYBUFFERSIZE];

static inline char tolower_fix(char c)
{
	return static_cast<char>(std::tolower(c));
}

CKey::CKey(uint key) : m_key(key)
{
}

CKey::CKey(const char *arg)
{
	string key = arg;
	transform(key.begin(), key.end(), key.begin(), tolower_fix);
	for (int i=0 ; i<KEYBUFFERSIZE ; ++i)
	{
		if (key == ms_names[i])
		{
			m_key = i;
			return;
		}
	}
	throw error::milk("\""+boost::lexical_cast<string>(key)+"\" is not a valid key-name");
}

CKey::CKey(const std::string& arg)
{
	string key = arg;
	transform(key.begin(), key.end(), key.begin(), tolower_fix);
	for (int i=0 ; i<KEYBUFFERSIZE ; ++i)
	{
		if (key == ms_names[i])
		{
			m_key = i;
			return;
		}
	}
	throw error::milk("\""+boost::lexical_cast<string>(key)+"\" is not a valid key-name");
}

uint CKey::toEnum() const
{
	return m_key;
}

const string& CKey::toString() const
{
	if (m_key >= KEYBUFFERSIZE || ms_names[m_key]=="")
		throw error::milk("\""+boost::lexical_cast<string>(m_key)+"\" is not a valid key-nr");
	else
		return ms_names[m_key];
}

void CKey::init()
{
	ms_names[SDLK_BACKSPACE] = "backspace";
	ms_names[SDLK_TAB] = "tab";
	ms_names[SDLK_CLEAR] = "clear";
	ms_names[SDLK_RETURN] = "return";
	ms_names[SDLK_PAUSE] = "pause";
	ms_names[SDLK_ESCAPE] = "escape";
	ms_names[SDLK_SPACE] = "space";
	ms_names[SDLK_EXCLAIM]  = "!";
	ms_names[SDLK_QUOTEDBL]  = "\"";
	ms_names[SDLK_HASH]  = "#";
	ms_names[SDLK_DOLLAR]  = "$";
	ms_names[SDLK_AMPERSAND]  = "&";
	ms_names[SDLK_QUOTE] = "'";
	ms_names[SDLK_LEFTPAREN] = "(";
	ms_names[SDLK_RIGHTPAREN] = ")";
	ms_names[SDLK_ASTERISK] = "*";
	ms_names[SDLK_PLUS] = "+";
	ms_names[SDLK_COMMA] = ",";
	ms_names[SDLK_MINUS] = "-";
	ms_names[SDLK_PERIOD] = ".";
	ms_names[SDLK_SLASH] = "/";
	ms_names[SDLK_0] = "0";
	ms_names[SDLK_1] = "1";
	ms_names[SDLK_2] = "2";
	ms_names[SDLK_3] = "3";
	ms_names[SDLK_4] = "4";
	ms_names[SDLK_5] = "5";
	ms_names[SDLK_6] = "6";
	ms_names[SDLK_7] = "7";
	ms_names[SDLK_8] = "8";
	ms_names[SDLK_9] = "9";
	ms_names[SDLK_COLON] = ":";
	ms_names[SDLK_SEMICOLON] = ";";
	ms_names[SDLK_LESS] = "<";
	ms_names[SDLK_EQUALS] = "=";
	ms_names[SDLK_GREATER] = ">";
	ms_names[SDLK_QUESTION] = "?";
	ms_names[SDLK_AT] = "@";
	ms_names[SDLK_LEFTBRACKET] = "[";
	ms_names[SDLK_BACKSLASH] = "\\";
	ms_names[SDLK_RIGHTBRACKET] = "]";
	ms_names[SDLK_CARET] = "^";
	ms_names[SDLK_UNDERSCORE] = "_";
	ms_names[SDLK_BACKQUOTE] = "`";
	ms_names[SDLK_a] = "a";
	ms_names[SDLK_b] = "b";
	ms_names[SDLK_c] = "c";
	ms_names[SDLK_d] = "d";
	ms_names[SDLK_e] = "e";
	ms_names[SDLK_f] = "f";
	ms_names[SDLK_g] = "g";
	ms_names[SDLK_h] = "h";
	ms_names[SDLK_i] = "i";
	ms_names[SDLK_j] = "j";
	ms_names[SDLK_k] = "k";
	ms_names[SDLK_l] = "l";
	ms_names[SDLK_m] = "m";
	ms_names[SDLK_n] = "n";
	ms_names[SDLK_o] = "o";
	ms_names[SDLK_p] = "p";
	ms_names[SDLK_q] = "q";
	ms_names[SDLK_r] = "r";
	ms_names[SDLK_s] = "s";
	ms_names[SDLK_t] = "t";
	ms_names[SDLK_u] = "u";
	ms_names[SDLK_v] = "v";
	ms_names[SDLK_w] = "w";
	ms_names[SDLK_x] = "x";
	ms_names[SDLK_y] = "y";
	ms_names[SDLK_z] = "z";
	ms_names[SDLK_DELETE] = "delete";

	ms_names[SDLK_WORLD_0] = "world 0";
	ms_names[SDLK_WORLD_1] = "world 1";
	ms_names[SDLK_WORLD_2] = "world 2";
	ms_names[SDLK_WORLD_3] = "world 3";
	ms_names[SDLK_WORLD_4] = "world 4";
	ms_names[SDLK_WORLD_5] = "world 5";
	ms_names[SDLK_WORLD_6] = "world 6";
	ms_names[SDLK_WORLD_7] = "world 7";
	ms_names[SDLK_WORLD_8] = "world 8";
	ms_names[SDLK_WORLD_9] = "world 9";
	ms_names[SDLK_WORLD_10] = "world 10";
	ms_names[SDLK_WORLD_11] = "world 11";
	ms_names[SDLK_WORLD_12] = "world 12";
	ms_names[SDLK_WORLD_13] = "world 13";
	ms_names[SDLK_WORLD_14] = "world 14";
	ms_names[SDLK_WORLD_15] = "world 15";
	ms_names[SDLK_WORLD_16] = "world 16";
	ms_names[SDLK_WORLD_17] = "world 17";
	ms_names[SDLK_WORLD_18] = "world 18";
	ms_names[SDLK_WORLD_19] = "world 19";
	ms_names[SDLK_WORLD_20] = "world 20";
	ms_names[SDLK_WORLD_21] = "world 21";
	ms_names[SDLK_WORLD_22] = "world 22";
	ms_names[SDLK_WORLD_23] = "world 23";
	ms_names[SDLK_WORLD_24] = "world 24";
	ms_names[SDLK_WORLD_25] = "world 25";
	ms_names[SDLK_WORLD_26] = "world 26";
	ms_names[SDLK_WORLD_27] = "world 27";
	ms_names[SDLK_WORLD_28] = "world 28";
	ms_names[SDLK_WORLD_29] = "world 29";
	ms_names[SDLK_WORLD_30] = "world 30";
	ms_names[SDLK_WORLD_31] = "world 31";
	ms_names[SDLK_WORLD_32] = "world 32";
	ms_names[SDLK_WORLD_33] = "world 33";
	ms_names[SDLK_WORLD_34] = "world 34";
	ms_names[SDLK_WORLD_35] = "world 35";
	ms_names[SDLK_WORLD_36] = "world 36";
	ms_names[SDLK_WORLD_37] = "world 37";
	ms_names[SDLK_WORLD_38] = "world 38";
	ms_names[SDLK_WORLD_39] = "world 39";
	ms_names[SDLK_WORLD_40] = "world 40";
	ms_names[SDLK_WORLD_41] = "world 41";
	ms_names[SDLK_WORLD_42] = "world 42";
	ms_names[SDLK_WORLD_43] = "world 43";
	ms_names[SDLK_WORLD_44] = "world 44";
	ms_names[SDLK_WORLD_45] = "world 45";
	ms_names[SDLK_WORLD_46] = "world 46";
	ms_names[SDLK_WORLD_47] = "world 47";
	ms_names[SDLK_WORLD_48] = "world 48";
	ms_names[SDLK_WORLD_49] = "world 49";
	ms_names[SDLK_WORLD_50] = "world 50";
	ms_names[SDLK_WORLD_51] = "world 51";
	ms_names[SDLK_WORLD_52] = "world 52";
	ms_names[SDLK_WORLD_53] = "world 53";
	ms_names[SDLK_WORLD_54] = "world 54";
	ms_names[SDLK_WORLD_55] = "world 55";
	ms_names[SDLK_WORLD_56] = "world 56";
	ms_names[SDLK_WORLD_57] = "world 57";
	ms_names[SDLK_WORLD_58] = "world 58";
	ms_names[SDLK_WORLD_59] = "world 59";
	ms_names[SDLK_WORLD_60] = "world 60";
	ms_names[SDLK_WORLD_61] = "world 61";
	ms_names[SDLK_WORLD_62] = "world 62";
	ms_names[SDLK_WORLD_63] = "world 63";
	ms_names[SDLK_WORLD_64] = "world 64";
	ms_names[SDLK_WORLD_65] = "world 65";
	ms_names[SDLK_WORLD_66] = "world 66";
	ms_names[SDLK_WORLD_67] = "world 67";
	ms_names[SDLK_WORLD_68] = "world 68";
	ms_names[SDLK_WORLD_69] = "world 69";
	ms_names[SDLK_WORLD_70] = "world 70";
	ms_names[SDLK_WORLD_71] = "world 71";
	ms_names[SDLK_WORLD_72] = "world 72";
	ms_names[SDLK_WORLD_73] = "world 73";
	ms_names[SDLK_WORLD_74] = "world 74";
	ms_names[SDLK_WORLD_75] = "world 75";
	ms_names[SDLK_WORLD_76] = "world 76";
	ms_names[SDLK_WORLD_77] = "world 77";
	ms_names[SDLK_WORLD_78] = "world 78";
	ms_names[SDLK_WORLD_79] = "world 79";
	ms_names[SDLK_WORLD_80] = "world 80";
	ms_names[SDLK_WORLD_81] = "world 81";
	ms_names[SDLK_WORLD_82] = "world 82";
	ms_names[SDLK_WORLD_83] = "world 83";
	ms_names[SDLK_WORLD_84] = "world 84";
	ms_names[SDLK_WORLD_85] = "world 85";
	ms_names[SDLK_WORLD_86] = "world 86";
	ms_names[SDLK_WORLD_87] = "world 87";
	ms_names[SDLK_WORLD_88] = "world 88";
	ms_names[SDLK_WORLD_89] = "world 89";
	ms_names[SDLK_WORLD_90] = "world 90";
	ms_names[SDLK_WORLD_91] = "world 91";
	ms_names[SDLK_WORLD_92] = "world 92";
	ms_names[SDLK_WORLD_93] = "world 93";
	ms_names[SDLK_WORLD_94] = "world 94";
	ms_names[SDLK_WORLD_95] = "world 95";

	ms_names[SDLK_KP0] = "[0]";
	ms_names[SDLK_KP1] = "[1]";
	ms_names[SDLK_KP2] = "[2]";
	ms_names[SDLK_KP3] = "[3]";
	ms_names[SDLK_KP4] = "[4]";
	ms_names[SDLK_KP5] = "[5]";
	ms_names[SDLK_KP6] = "[6]";
	ms_names[SDLK_KP7] = "[7]";
	ms_names[SDLK_KP8] = "[8]";
	ms_names[SDLK_KP9] = "[9]";
	ms_names[SDLK_KP_PERIOD] = "[.]";
	ms_names[SDLK_KP_DIVIDE] = "[/]";
	ms_names[SDLK_KP_MULTIPLY] = "[*]";
	ms_names[SDLK_KP_MINUS] = "[-]";
	ms_names[SDLK_KP_PLUS] = "[+]";
	ms_names[SDLK_KP_ENTER] = "enter";
	ms_names[SDLK_KP_EQUALS] = "equals";

	ms_names[SDLK_UP] = "up";
	ms_names[SDLK_DOWN] = "down";
	ms_names[SDLK_RIGHT] = "right";
	ms_names[SDLK_LEFT] = "left";
	ms_names[SDLK_DOWN] = "down";
	ms_names[SDLK_INSERT] = "insert";
	ms_names[SDLK_HOME] = "home";
	ms_names[SDLK_END] = "end";
	ms_names[SDLK_PAGEUP] = "page up";
	ms_names[SDLK_PAGEDOWN] = "page down";

	ms_names[SDLK_F1] = "f1";
	ms_names[SDLK_F2] = "f2";
	ms_names[SDLK_F3] = "f3";
	ms_names[SDLK_F4] = "f4";
	ms_names[SDLK_F5] = "f5";
	ms_names[SDLK_F6] = "f6";
	ms_names[SDLK_F7] = "f7";
	ms_names[SDLK_F8] = "f8";
	ms_names[SDLK_F9] = "f9";
	ms_names[SDLK_F10] = "f10";
	ms_names[SDLK_F11] = "f11";
	ms_names[SDLK_F12] = "f12";
	ms_names[SDLK_F13] = "f13";
	ms_names[SDLK_F14] = "f14";
	ms_names[SDLK_F15] = "f15";

	ms_names[SDLK_NUMLOCK] = "numlock";
	ms_names[SDLK_CAPSLOCK] = "caps lock";
	ms_names[SDLK_SCROLLOCK] = "scroll lock";
	ms_names[SDLK_RSHIFT] = "right shift";
	ms_names[SDLK_LSHIFT] = "left shift";
	ms_names[SDLK_RCTRL] = "right ctrl";
	ms_names[SDLK_LCTRL] = "left ctrl";
	ms_names[SDLK_RALT] = "right alt";
	ms_names[SDLK_LALT] = "left alt";
	ms_names[SDLK_RMETA] = "right meta";
	ms_names[SDLK_LMETA] = "left meta";
	ms_names[SDLK_LSUPER] = "left super";	/* "Windows" keys */
	ms_names[SDLK_RSUPER] = "right super";	
	ms_names[SDLK_MODE] = "alt graph";
	ms_names[SDLK_COMPOSE] = "compose";

	ms_names[SDLK_HELP] = "help";
	ms_names[SDLK_PRINT] = "print screen";
	ms_names[SDLK_SYSREQ] = "sys req";
	ms_names[SDLK_BREAK] = "break";
	ms_names[SDLK_MENU] = "menu";
	ms_names[SDLK_POWER] = "power";
	ms_names[SDLK_EURO] = "euro";
	ms_names[SDLK_UNDO] = "undo";


	ms_names[SDLK_LAST + 1] = "left mousebutton";
	ms_names[SDLK_LAST + 2] = "middle mousebutton";
	ms_names[SDLK_LAST + 3] = "right mousebutton";
	ms_names[SDLK_LAST + 4] = "scroll up";
	ms_names[SDLK_LAST + 5] = "scroll down";
	ms_names[SDLK_LAST + 6] = "mousebutton 6";
	ms_names[SDLK_LAST + 7] = "mousebutton 7";
	ms_names[SDLK_LAST + 8] = "mousebutton 8";
	ms_names[SDLK_LAST + 9] = "mousebutton 9";
}

void CKey::free()
{
	for(int i=0; i<KEYBUFFERSIZE; ++i)
		ms_names[i].clear();
}



bool Input::m_keys[KEYBUFFERSIZE];			// Current keybuffer
bool Input::m_keysPressed[KEYBUFFERSIZE];
bool Input::m_keysReleased[KEYBUFFERSIZE];
CVector3<int> Input::m_mousePos(0,0,0);		// mouse-pos
CVector3<int> Input::m_mousePosRel(0,0,0);	// mouse-movement
CRect<int> Input::m_mouseRect(0,0,0,0);		// the boundary of the mouse
bool Input::m_boundary = false;				// Boundary active?
vectorPOD<string*> Input::m_inputStr;			// input string
vectorPOD<pair<bool, CControl*> > Input::m_callbacks;		// key-callbacks
bool Input::m_ignore(false);
bool Input::m_grab(false);
bool Input::m_show(true);
bool Input::m_active(true);
int Input::m_charCode(0);

void Input::setGrabInput(bool g)
{
	m_grab = g;
	SDL_WM_GrabInput(g?SDL_GRAB_ON:SDL_GRAB_OFF);

	if(m_grab)
	{
		IWindow *window = IWindow::getInstance();
		BOOST_ASSERT(window);
		SDL_WarpMouse((Uint16)(window->size().x/2), (Uint16)(window->size().y/2));
	}
}

bool Input::getGrabInput()
{
	return m_grab;
}

void Input::setShowCursor(bool c)
{
	m_show = c;
	SDL_ShowCursor(c?SDL_ENABLE:SDL_DISABLE);
}

bool Input::getShowCursor()
{
	return m_show;
}

void Input::init()
{
	fill_n(m_keys,			(int)KEYBUFFERSIZE,	false);
	fill_n(m_keysPressed,	(int)KEYBUFFERSIZE,	false);
	fill_n(m_keysReleased,	(int)KEYBUFFERSIZE,	false);
	CKey::init();
}

void Input::free()
{
	CKey::free();
	setGrabInput(false);
	setShowCursor(true);
}

void Input::update()
{
	fill_n(m_keysPressed,	(int)KEYBUFFERSIZE, false);
	fill_n(m_keysReleased,	(int)KEYBUFFERSIZE, false);

	m_mousePosRel.set(0,0,0);

	SDL_Event event;
	while(SDL_PollEvent(&event)) 
	{
		switch(event.type)
		{
			case SDL_ACTIVEEVENT:
				if(event.active.state & SDL_APPACTIVE || event.active.state & SDL_APPINPUTFOCUS)
				{
					m_active = event.active.gain ? true : false;
					if(m_active)
					{
						// Restore grab/show
						setGrabInput(m_grab);
						setShowCursor(m_show);
					}
					else
					{
						SDL_WM_GrabInput(SDL_GRAB_OFF);
						SDL_ShowCursor(SDL_ENABLE);
					}
				}
				break;

			case SDL_KEYDOWN:
			{
				bool temp1 = m_keys[event.key.keysym.sym];
				bool temp2 = m_keysPressed[event.key.keysym.sym];
				m_keys[event.key.keysym.sym] = true;
				m_keysPressed[event.key.keysym.sym] = true;

				checkCallbacks();

				if (m_ignore)
				{
					m_ignore=false;

					m_keys[event.key.keysym.sym] = temp1;
					m_keysPressed[event.key.keysym.sym] = temp2;
				}
				// if we are currently inputting text!
				else if (!m_inputStr.empty())
				{
					string& str = *m_inputStr.back();
					SDL_keysym& keysym = event.key.keysym;

					if((keysym.sym == SDLK_v && modifier(CTRL)) || 
						(keysym.sym == SDLK_INSERT && modifier(SHIFT)))
					{
						/* windows only: pasting from clipboard! */
#ifdef WIN32
						if(OpenClipboard(NULL))
						{
							char* txt = (char*)GetClipboardData(CF_TEXT);
							if(txt)
								str.append(txt);
							CloseClipboard();
						}
#endif
					}

					// normal text input
					else if (!modifier(ALT) && !modifier(CTRL))
					{
						if (keysym.sym == SDLK_BACKSPACE)
						{
							if (!str.empty())
								str.resize(str.length()-1);
						}
						else if (keysym.sym == SDLK_RETURN || keysym.sym == SDLK_KP_ENTER)
						{
							str.append(1, '\n'); 
						}
						else if (keysym.sym == SDLK_TAB)
						{
							str.append(1, '\t'); 
						}
						else if (keysym.sym >= SDLK_KP0 && keysym.sym <= SDLK_KP9)	//numpad
						{
							str.append(1, (char)(keysym.sym-(SDLK_KP0-SDLK_0)));
						}
						else if ((int)keysym.unicode!=0)
						{
							str.append(1, (char)keysym.unicode);
						}
					}
					//abnormal text-input
					else if (modifier(ALT))
					{
						/* if someone is holding down alt and are using the
						numpad at the same time, someone is inputing charcter-code! */
						if (keysym.sym >= SDLK_KP0 && keysym.sym <= SDLK_KP9)	//numpad
						{
							m_charCode *= 10;
							m_charCode += (keysym.sym-SDLK_KP0);
						}
					}

				}
			}break;

			case SDL_KEYUP:
			{
				bool temp1 = m_keys[event.key.keysym.sym];
				bool temp2 = m_keysPressed[event.key.keysym.sym];
				m_keys[event.key.keysym.sym] = false;
				m_keysReleased[event.key.keysym.sym] = true;

				/* has someone entered a charecter-code using alt+numpad? */
				if (!m_inputStr.empty() && m_charCode!=0 &&
					(event.key.keysym.sym == SDLK_RALT ||
					event.key.keysym.sym == SDLK_LALT))
				{
					while (m_charCode >= 128)
						m_charCode -= 256;
					m_inputStr.back()->append(1, char(m_charCode));
					m_charCode = 0;
				}

				checkCallbacks();

				if (m_ignore)
				{
					m_ignore=false;

					m_keys[event.key.keysym.sym] = temp1;
					m_keysPressed[event.key.keysym.sym] = temp2;
				}
			}break;

			case SDL_MOUSEMOTION:
				if(m_active)
				{
					static bool first = true;
					if(first)
					{
						first = false;
						break;
					}
					m_mousePosRel.x += event.motion.xrel;
					m_mousePosRel.y += event.motion.yrel;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				m_keys[SDLK_MOUSE_BUTTON + event.button.button] = true;
				m_keysPressed[SDLK_MOUSE_BUTTON + event.button.button] = true;
				if		(event.button.button==SDL_BUTTON_WHEELUP)	++m_mousePosRel.z;
				else if	(event.button.button==SDL_BUTTON_WHEELDOWN)	--m_mousePosRel.z;
				checkCallbacks();
				break;

			case SDL_MOUSEBUTTONUP:
				m_keys[SDLK_LAST + event.button.button] = false;
				m_keysReleased[SDLK_LAST + event.button.button] = true;
				checkCallbacks();
				break;

			case SDL_JOYAXISMOTION: case SDL_JOYBALLMOTION: case SDL_JOYHATMOTION: case SDL_JOYBUTTONDOWN: case SDL_JOYBUTTONUP:
				break;

			case SDL_QUIT:
				IApplication::getInstance()->setQuitState(true);
				break;

			case SDL_SYSWMEVENT:
				break;

			case SDL_VIDEORESIZE:
				{
					IWindow *window = IWindow::getInstance();
					if (window)
						window->onResizeWindow(event.resize.w, event.resize.h);
				}
				break;

			case SDL_VIDEOEXPOSE:
				break;

			case SDL_USEREVENT:
				break;
		}
	}

	m_mousePos += m_mousePosRel;

	if (boundary())
	{
		m_mousePos.x = math::clamp(m_mousePos.x, m_mouseRect.x1, m_mouseRect.x2);
		m_mousePos.y = math::clamp(m_mousePos.y, m_mouseRect.y1, m_mouseRect.y2);
	}

	//clear the called-flag of the callbacks
	for (vectorPOD<pair<bool,CControl*> >::iterator it=m_callbacks.begin() ; it!=m_callbacks.end() ; ++it)
		it->first = false;
}


void Input::checkCallbacks()
{
	vectorPOD<CControl*> toCall;
	for (vectorPOD<pair<bool,CControl*> >::iterator it=m_callbacks.begin() ; it!=m_callbacks.end() ; ++it)
	{
		if (!it->first && *it->second)	//call only once per loop
		{
			toCall.push_back(it->second);
			it->first = true;
		}
	}
	for (vectorPOD<CControl*>::iterator it=toCall.begin() ; it!=toCall.end() ; ++it)
	{
		(*(*it)->m_callback)();
		if (m_ignore)
			break;
	}
}


bool Input::down(CKey key, bool override)
{
	if (!m_inputStr.empty() && !override)
		return false;
	return m_keys[key.toEnum()];
}

bool Input::up(CKey key, bool override)
{
	if (!m_inputStr.empty() && !override)
		return false;
	return !m_keys[key.toEnum()];
}

bool Input::pressed(CKey key, bool override)
{
	if (!m_inputStr.empty() && !override)
		return false;
	return m_keysPressed[key.toEnum()];
}

bool Input::released(CKey key, bool override)
{
	if (!m_inputStr.empty() && !override)
		return false;
	return m_keysReleased[key.toEnum()];
}

bool Input::modifier(Modifier m)
{
	SDLMod mod = SDL_GetModState();
	switch(m)
	{
	case NONE:
		return mod==KMOD_NONE;

	case SHIFT:
		return (mod & KMOD_SHIFT)!=0;
	case LSHIFT:
		return (mod & KMOD_LSHIFT)!=0;
	case RSHIFT:
		return (mod & KMOD_RSHIFT)!=0;

	case CTRL:
		return (mod & KMOD_CTRL)!=0;
	case LCTRL:
		return (mod & KMOD_LCTRL)!=0;
	case RCTRL:
		return (mod & KMOD_RCTRL)!=0;

	case ALT:
		return (mod & KMOD_ALT)!=0;
	case LALT:
		return (mod & KMOD_LALT)!=0;
	case RALT:
		return (mod & KMOD_RALT)!=0;

	case META:
		return (mod & KMOD_META)!=0;
	case LMETA:
		return (mod & KMOD_LMETA)!=0;
	case RMETA:
		return (mod & KMOD_RMETA)!=0;

	case NUM:
		return (mod & KMOD_NUM)!=0;
	case CAPS:
		return (mod & KMOD_CAPS)!=0;
	default: //case MODE:
		return (mod & KMOD_MODE)!=0;
	}
}

CVector2<int> Input::getMousePos()
{
	if(!m_grab && m_show)
		SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
	return CVector2<int>(m_mousePos.x, m_mousePos.y);
}

CVector2<int> Input::getMouseRel()
{
	return CVector2<int>(m_mousePosRel.x, m_mousePosRel.y);
}

void Input::setMousePos(const CVector2<int>& v)
{
	setMousePos(v.x, v.y);
}

void Input::setMousePos(int x, int y)
{
	SDL_WarpMouse((Uint16)x, (Uint16)y);
	m_mousePos.set(x, y, m_mousePos.z);
}

int Input::getMouseScrollRel()
{
	return m_mousePosRel.z;
}

int Input::getMouseScroll()
{
	return m_mousePos.z;
}

void Input::setMouseScroll(int a)
{
	m_mousePos.z = a;
}

void Input::setBoundary(const CRect<int>& rect)
{
	m_mouseRect = rect;
}

const CRect<int>& Input::getBoundary()
{
	return m_mouseRect;
}

void Input::activateBoundary()
{
	m_boundary = true;
}

void Input::inactivateBoundary()
{
	m_boundary = false;
}

bool Input::boundary()
{
	return m_boundary;
}

void Input::bindInput(string* str)
{
	if(!str)
	{
		BOOST_ASSERT(!m_inputStr.empty());
		m_inputStr.pop_back();
	}
	else
	{
		m_inputStr.push_back(str);
	}

	SDL_EnableUNICODE(m_inputStr.empty() ? 0 : 1);
	SDL_EnableKeyRepeat(m_inputStr.empty() ? 0 : SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

bool Input::isInputting()
{
	return (!m_inputStr.empty());
}

void Input::addCallback(CControl *ptr)
{
	m_callbacks.push_back(pair<bool,CControl*>(false, ptr));
}

void Input::removeCallback(CControl *ptr)
{
	for (vectorPOD<pair<bool,CControl*> >::iterator it=m_callbacks.begin() ; it!=m_callbacks.end() ; ++it)
	{
		if (it->second == ptr)
		{
			m_callbacks.erase(it);
			return;
		}
	}
	milk_on_debug( error::milk("Ptr not found in callbacks"); );
}
