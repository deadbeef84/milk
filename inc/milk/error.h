#ifndef MILK_H_
#define MILK_H_

#include <stdexcept>
#include <exception>
#include <string>
#include <sstream>
#include "milk/types.h"
#include "milk/obsolete/milk_clog.h"

#include "milk/obsolete/milk_exceptionreporter.h"

namespace milk
{

	namespace error
	{
#ifdef MILK_EH_TRANSLATOR
		void my_translator(unsigned code, EXCEPTION_POINTERS *info);
#endif

		/// the base error-class
		class milk : public std::runtime_error
		{
		public:
			enum ExceptionReportType
			{
				ME_CALLSTACK,
				ME_FULL,
				ME_CALLSTACKWRITE,
				ME_FULLWRITE
			};

			milk(const std::string& str=std::string())
				: runtime_error(str)
			{
				if (log)
				{
					Level old = log->getReportLevel();
					log->setReportLevel(level);
					*log << str << std::endl;
					log->setReportLevel(old);
				}
			}

			// debug-reported exception
			milk(const std::string& str, ExceptionReportType reportType)
				: runtime_error(str)
			{
				// following code creates link-errors - why the fuck?
				if (log)
				{
					Level old = log->getReportLevel();
					log->setReportLevel(level);
					*log << str << std::endl;
					log->setReportLevel(old);
				}
				
				switch(reportType)
				{
				case ME_CALLSTACK:
					m_reportBuffer = ExceptionReporter::stackDumpBuffer(ExceptionReporter::CALLSTACK);
					break;
				case ME_FULL:
					m_reportBuffer = ExceptionReporter::stackDumpBuffer(ExceptionReporter::FULL);
					break;
				case ME_CALLSTACKWRITE:
					ExceptionReporter::stackDump(ExceptionReporter::CALLSTACK);
					break;
				case ME_FULLWRITE:
					ExceptionReporter::stackDump(ExceptionReporter::FULL);
					break;
				}
			}
			
			~milk() throw()
			{ }

			void writeReport() const
			{
				if(!m_reportBuffer.empty())
					ExceptionReporter::formatDumpBuffer(m_reportBuffer);
			}

			static void setLog(CLog* l, Level lev)
			{ log=l; level=lev; }

			static CLog* getLog()
			{ return log; }

		private:
			static CLog *log;
			static Level level;
			std::string m_reportBuffer;
		};

		class scenegraph : public milk
		{
		public:
			scenegraph(const std::string& str=std::string())
				: milk(str) { } 
		};

		/// file error
		class file : public milk
		{
		public:
			file(const std::string& str=std::string())
				: milk(str) { } 
		};

			/// error on writing to file
			class file_write : public file
			{
			public:
				file_write(const std::string& str=std::string())
					: file(str) { }
			};

			/// error on reading from file
			class file_read : public file
			{
			public:
				file_read(const std::string& str=std::string())
					: file(str) { } 
			};

				/// error on missing file
				class file_not_found : public file_read
				{
				public:
					file_not_found(const std::string& str=std::string())
						: file_read(str) { } 
				};

				/// corrupt file
				class corrupt_file : public file_read
				{
				public:
					corrupt_file(const std::string& str=std::string())
						: file_read(str) { } 
				};

		/// Mathematical error
		class math : public milk
		{
		public:
			math(const std::string& str=std::string())
				: milk(str) { } 
		};

		/// Net error
		class net : public milk
		{
		public:
			net(const std::string& str=std::string())
				: milk(str) { } 
		};

			/// connection problems error
			class net_connect : public net
			{
			public:
				net_connect(const std::string& str=std::string())
					: net(str) { } 
			};

			/// sending-error
			class net_send : public net
			{
			public:
				net_send(const std::string& str=std::string())
					: net(str) { } 
			};

			/// receiving-error
			class net_recv : public net
			{
			public:
				net_recv(const std::string& str=std::string())
					: net(str) { } 
			};

			/// we got disconnected
			class net_disconnect : public net
			{
			public:
				net_disconnect(const std::string& str=std::string())
					: net(str) { } 
			};

			/// corrupt packet?
			class net_corrupt_packet : public net
			{
			public:
				net_corrupt_packet(const std::string& str=std::string())
					: net(str) { } 
			};

		/// something's wrong with the profiling
		class profiler : public milk
		{
		public:
			profiler(const std::string& str=std::string())
				: milk(str) { } 
		};

		/// rendering problem
		class rendering : public milk
		{
		public:
			rendering(const std::string& str=std::string())
				: milk(str, ME_CALLSTACK) { } 
		};

			/// opengl problem
			class opengl : public rendering
			{
			public:
				opengl(const std::string& str=std::string(), uint glerr=0)
					: rendering(str), m_glerr(glerr) { } 

					uint getGLError() const
					{ return m_glerr; }

			private:
				uint m_glerr;
			};

				/// problem with vertex-buffer
				class vertexbuffer : public opengl
				{
				public:
					vertexbuffer(const std::string& str=std::string())
						: opengl(str) { } 
				};

					/// problems with vertex-buffer format
					class vertexbuffer_format : public vertexbuffer
					{
					public:
						vertexbuffer_format(const std::string& str=std::string())
							: vertexbuffer(str) { } 
					};

		/// audio problems
		class sound : public milk
		{
		public:
			sound(const std::string& str=std::string())
				: milk(str, ME_CALLSTACK) { } 
		};

			/// problems with OpenAL
			class openal : public sound
			{
			public:
				openal(const std::string& str=std::string())
					: sound(str) { } 
			};

		/// problems with threads
		class thread : public milk
		{
		public:
			thread(const std::string& str=std::string())
				: milk(str, ME_CALLSTACK) { } 
		};

		/// problems parsing
		class parser : public milk
		{
		public:
			parser(const std::string& str=std::string())
				: milk(str) { } 
		};

		/// problems with xml
		class xml : public milk
		{
		public:
			xml(const std::string& str=std::string())
				: milk(str) { } 
		};

			/// problems parsing xml
			class xml_parse : public xml
			{
			public:
				xml_parse(const std::string& str=std::string())
					: xml(str) { } 
			};

			/// erroneous xml-item
			class xml_item : public xml
			{
			public:
				xml_item(const std::string& str=std::string())
					: xml(str) { } 
			};

		/// problem with cd
		class cd : public milk
		{
		public:
			cd(const std::string& str=std::string())
				: milk(str, ME_CALLSTACK) { } 
		};
	}
}

#endif
