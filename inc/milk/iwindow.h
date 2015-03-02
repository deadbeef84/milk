#ifndef MILK_IWINDOW_H_
#define MILK_IWINDOW_H_

#include <string>
#include "milk/types.h"
#include "milk/math/cvector.h"
#include "milk/math/crect.h"
#include "milk/renderer/irendertarget.h"

namespace milk
{
	class IApplication;

	class IWindow : public IRenderTarget
	{
	public:
		enum Flags
		{
			FULLSCREEN	= 1<<0,
			RESIZABLE	= 1<<1,
			NOFRAME		= 1<<2
		};

		friend inline Flags operator|(Flags a, Flags b)
		{ return static_cast<Flags>(static_cast<int>(a)|static_cast<int>(b)); }

		/// creates a window
		/**
		Only one window can be created at one time.
		flags can be FULLSCREEN, RESIZABLE ad NOFRAME. depth is the color-depth, mostly 16 or 32.
		zdepth is the precision of the z-buffer, mostly 16 or 24 bits. MULUX, do this for me :P
		*/
		IWindow(IApplication& owner, const std::string& title, int width, int height,
			Flags flags = Flags(0), int depth = 0, int zDepth = 16, int sDepth = 0, int msb=0, int mss=0);

		virtual ~IWindow();

		/// change the title of the window
		virtual void setWindowTitle(const std::string& title);

		/// resize the window. this does not work in fullscreen
		virtual void onResizeWindow(int width, int height);

		/// toggle between window and fullscreen... this doesn't quite work, i think
		virtual void toggleFullscreen();

		/////////////////////////////////////////////

		bool beginRender();

		void endRender();

		/////////////////////////////////////////////

		/// is the current window active (eg, not minimized)?
		virtual bool isActive() const;

		/////////////////////////////////////////////

		/// switch on/off v-sync
		virtual void setVerticalSync(bool enabled);
		/// return whether v-sync is on/off
		virtual bool getVerticalSync() const;

		/////////////////////////////////////////////

		void run();
		virtual void update()=0;
		virtual void render()=0;

		/////////////////////////////////////////////

		static IWindow *getInstance()
		{ return ms_instance; }
		
		static int getClamp();

	protected:
		virtual IApplication& getOwner()
		{ return m_owner; }

		int m_frame;

	private:
		IApplication& m_owner;
		int m_depth, m_zDepth, m_sDepth;
		bool m_fullscreen;
		int m_isRendering;

		// Clamp mode
		int m_clamp;

		// pointer to the created window. 0 if no window present
		static IWindow *ms_instance;

		IWindow(IWindow&);
		IWindow& operator=(IWindow&);
	};

}

#endif
