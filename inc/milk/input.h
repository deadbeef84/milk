#ifndef MILK_INPUT_H_
#define MILK_INPUT_H_

#include <map>
#include <string>
#include <vector>
#include "milk/math/cvector.h"
#include "milk/math/crect.h"
#include "milk/types.h"
#include "milk/vectorpod.h"
#include <SDL.h>

// Milk SDL-Extended keys
enum
{
	SDLK_MOUSE_BUTTON = SDLK_LAST,

	SDLK_MOUSE_L		= (SDLK_LAST + 1),
	SDLK_MOUSE_M		= (SDLK_LAST + 2),
	SDLK_MOUSE_R		= (SDLK_LAST + 3),
	SDLK_SCROLL_UP		= (SDLK_LAST + 4),
	SDLK_SCROLL_DOWN	= (SDLK_LAST + 5),

	KEYBUFFERSIZE = (SDLK_LAST + 10)		//set the maximum nr of mouse-buttons to 10
};

namespace milk
{
	/// key-class, describes a key and name
	class CKey
	{
	public:
		CKey(uint key);
		CKey(const std::string& key);
		CKey(const char *key);
		virtual ~CKey(){}

		uint toEnum() const;
		const std::string& toString() const;

	private:
		uint m_key;

		friend class Input;
		static void init();
		static void free();
		static std::string ms_names[KEYBUFFERSIZE];
	};


	class CControl;

	/// modifiers, SHIFT, ALT etc...
	enum Modifier
	{
		NONE,

		SHIFT,
		LSHIFT,
		RSHIFT,

		CTRL,
		LCTRL,
		RCTRL,

		ALT,
		LALT,
		RALT,

		META,
		LMETA,
		RMETA,

		NUM,
		CAPS,
		MODE
	};

	/// class for handling input
	class Input
	{
	public:
		static bool down(CKey key, bool override=false);
		static bool up(CKey key, bool override=false);
		static bool pressed(CKey key, bool override=false);
		static bool released(CKey key, bool override=false);

		static bool modifier(Modifier m);
		
		static CVector2<int> getMousePos();
		static CVector2<int> getMouseRel();
		static void setMousePos(const CVector2<int>&);
		static void setMousePos(int x, int y);

		static int getMouseScroll();
		static int getMouseScrollRel();
		static void setMouseScroll(int);
		
		static void setBoundary(const CRect<int>& rect);
		static const CRect<int>& getBoundary();

		static void activateBoundary();
		static void inactivateBoundary();
		static bool boundary();

		static void bindInput(std::string* str);
		static bool isInputting();
		
		static void setGrabInput(bool);
		static bool getGrabInput();
		static void setShowCursor(bool);
		static bool getShowCursor();

		static void update();


		static void ignoreNextEvent()
		{ m_ignore = true; }

	private:
		Input();
		static void init();
		static void free();

		static void checkCallbacks();

		static void addCallback(CControl *ptr);
		static void removeCallback(CControl *ptr);

		static bool m_keys[KEYBUFFERSIZE];				// Current keybuffer
		static bool m_keysPressed[KEYBUFFERSIZE];
		static bool m_keysReleased[KEYBUFFERSIZE];
		static CVector3<int> m_mousePos, m_mousePosRel;	// mouse-pos + scroll
		static CRect<int> m_mouseRect;					// the boundary of the mouse
		static bool m_boundary;							// Boundary active?
		static vectorPOD<std::string*> m_inputStr;		// input string
		static vectorPOD<std::pair<bool, CControl*> > m_callbacks;		// key-callbacks
		static bool m_ignore;
		static bool m_grab;
		static bool m_show;
		static bool m_active;

		/* used for storing the number a
		user is inputting with alt+keypad */
		static int m_charCode;

		friend class CControl;
		friend class IApplication;
		friend class IWindow;
	};
}

#endif
