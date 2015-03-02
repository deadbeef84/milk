#ifndef MILK_NOOB_H_
#define MILK_NOOB_H_

#ifdef WIN32
#	if defined(_MSC_VER)
#		pragma comment(lib, "glew32.lib")		// OpenGL-libraries
#		pragma comment(lib, "opengl32.lib")
#		pragma comment(lib, "glu32.lib")
#		pragma comment(lib, "SDL.lib")			// SDL-libraries
#		pragma comment(lib, "SDL_image.lib")
#		pragma comment(lib, "wsock32.lib")		// for milk::Net
//#		pragma comment(lib, "ALut.lib")			// OpenAL-libraries
#		pragma comment(lib, "OpenAL32.lib")
#		pragma comment(lib, "ogg.lib")
#		pragma comment(lib, "vorbis.lib")
#		pragma comment(lib, "vorbisenc.lib")
#		pragma comment(lib, "vorbisfile.lib")
#	endif
#endif

#define MILK_APP(P, W)\
	class CApplication : public IApplication {\
	public:\
		CApplication() : IApplication(P) { }\
		void run() { W wnd(*this); wnd.run(); }\
	};\
	int main(int, char *[]) {\
		try { CApplication().run(); return 0; }\
		catch(const std::exception& e) {\
			if(!dialog::confirm("Error", (string)e.what()+"\nPress cancel to debug."))\
				throw;\
		}\
		catch(...) {\
			if(!dialog::confirm("Unknown exception", "This is quite strange.\nPress cancel to debug."))\
				throw;\
		}\
		return 0xdeadbeef;\
	}

#endif
