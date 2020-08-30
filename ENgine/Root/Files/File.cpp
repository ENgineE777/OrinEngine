#include "Root/Root.h"
#include "File.h"
#include "stdarg.h"
#include <stdlib.h> 

namespace Oak
{
	bool File::Load(const char* name, File::ModeType mode)
	{
		Release();

		const char* modeStr[] = { "rb", "w", "wb", "a", "ab" };

		FILE* file = root.files.FileOpen(name, modeStr[(int)mode]);

		if (file)
		{
			Seek(0, SeekType::seekEnd);
			size = GetCurrentPos();

			Seek(0, SeekType::seekSet);

			return true;
		}

		return false;
	}

	File::~File()
	{
		Release();
	}

	int File::GetSize()
	{
		return size;
	}

	void File::Write(const void* data, int size)
	{
		fwrite(data, size, 1, file);
	}

	void File::Print(const char *format, ...)
	{
		static char* buf = nullptr;
		static int bufLen = 0;
	
		if (!buf)
		{
			bufLen = 1024;
			buf = (char*)malloc(bufLen);
		}

		va_list args;
		va_start( args, format );

		int len = vsnprintf(0, 0, format, args);

		if (len > bufLen + 1)
		{
			bufLen = len + 10;
			buf = (char*)realloc(buf, bufLen);
		}

		buf[0] = 0;

		vsnprintf( buf, bufLen, format, args );
		va_end( args );
	
		fprintf(file,"%s",buf);
		fprintf(file,"\n");
	}

	uint32_t File::Read(void* data, int size)
	{
		return (uint32_t)fread(data, 1, size, file);
	}

	void File::Read(char* val)
	{
		char ch = '7';

		int index = 0;
		while(ch != 0)
		{
			if (!Read(&ch,1))
			{
				break;
			}

			val[index] = ch;
			index++;
		}

		val[index] = 0;
	}

	uint32_t File::Seek(uint32_t fl_offset, File::SeekType seek)
	{
		return file ? fseek(file, fl_offset, (seek == SeekType::seekSet) ? SEEK_SET : ((seek == SeekType::seekCur) ? SEEK_CUR : SEEK_END) ) : -1;
	}

	uint32_t File::GetCurrentPos()
	{
		return file ? ftell(file) : -1;
	}

	void File::Release()
	{
		if (file)
		{
			fclose(file);
		}
	}
}