
#include "JsonWriter.h"
#include <stdarg.h>
#include "Root/Root.h"
#include "stb_sprintf.h"

namespace Orin
{
	JsonWriter::JsonWriter()
	{
		block_started[0] = true;
	}

	JsonWriter::~JsonWriter()
	{
		Close();
	}

	bool JsonWriter::Start(const char* name)
	{
		Close();

		file = root.files.FileOpen(name, "wb");

		if (file)
		{
			uint8_t bom[3];
			bom[0] = 0xEF;
			bom[1] = 0xBB;
			bom[2] = 0xBF;

			fwrite(bom, 3, 1, file);

			WriteFormatedStr(false, "{\n");
			offset = 1;

			return true;
		}

		return false;
	}

	void JsonWriter::StartArray(const char* name)
	{
		WriteFormatedStr(true, "\"%s\" : [\n", name);
		offset++;

		depth++;
		block_started[depth] = true;
	}

	void JsonWriter::FinishArray()
	{
		WriteStr("\n");

		offset--;
		WriteFormatedStr(false, "]");

		depth--;
	}

	void JsonWriter::StartBlock(const char* name)
	{
		AddComma();

		if (name)
		{
			WriteFormatedStr(false, "\"%s\" :\n", name);
		}

		WriteFormatedStr(false, "{\n");

		offset++;

		depth++;
		block_started[depth] = true;
	}

	void JsonWriter::FinishBlock()
	{
		WriteStr("\n"); 

		offset--;
		WriteFormatedStr(false, "}");

		depth--;
	}

	void JsonWriter::Write(const char* name, eastl::string& val)
	{
		Write(name, val.c_str());
	}

	void JsonWriter::Write(const char* name, const char* val)
	{
		StringUtils::FixSlashes((char*)val);

		if (name)
		{
			WriteFormatedStr(true, "\"%s\" : \"%s\"", name, val);
		}
		else
		{
			WriteFormatedStr(true, "\"%s\"", val);
		}
	}

	void JsonWriter::Write(const char* name, bool val)
	{
		if (name)
		{
			if (val)
			{
				WriteFormatedStr(true, "\"%s\" : true", name);
			}
			else
			{
				WriteFormatedStr(true, "\"%s\" : false", name);
			}
		}
		else
		{
			if (val)
			{
				WriteFormatedStr(true, "true");
			}
			else
			{
				WriteFormatedStr(true, "false");
			}
		}
	}

	void JsonWriter::Write(const char* name, float val)
	{
		if (name)
		{
			WriteFormatedStr(true, "\"%s\" : %4.6f", name, val);
		}
		else
		{
			WriteFormatedStr(true, "%4.6f", val);
		}
	}

	void JsonWriter::Write(const char* name, uint16_t& val)
	{
		if (name)
		{
			WriteFormatedStr(true, "\"%s\" : %i", name, val);
		}
		else
		{
			WriteFormatedStr(true, "%i", name);
		}
	}

	void JsonWriter::Write(const char* name, uint32_t val)
	{
		if (name)
		{
			WriteFormatedStr(true, "\"%s\" : %i", name, val);
		}
		else
		{
			WriteFormatedStr(true, "%i", name);
		}
	}

	void JsonWriter::Write(const char* name, int64_t val)
	{
		if (name)
		{
			WriteFormatedStr(true, "\"%s\" : %i", name, val);
		}
		else
		{
			WriteFormatedStr(true, "%i", name);
		}
	}

	void JsonWriter::Write(const char* name, int val)
	{
		if (name)
		{
			WriteFormatedStr(true, "\"%s\" : %li", name, val);
		}
		else
		{
			WriteFormatedStr(true, "%li", name);
		}
	}

	void JsonWriter::Write(const char* name, Math::Vector2& val)
	{
		StartBlock(name);
		Write("x", val.x);
		Write("y", val.y);
		FinishBlock();
	}

	void JsonWriter::Write(const char* name, Math::Vector3& val)
	{
		StartBlock(name);
		Write("x", val.x);
		Write("y", val.y);
		Write("z", val.z);
		FinishBlock();
	}

	void JsonWriter::Write(const char* name, Math::Vector4& val)
	{
		StartBlock(name);
		Write("x", val.x);
		Write("y", val.y);
		Write("z", val.z);
		Write("w", val.w);
		FinishBlock();
	}

	void JsonWriter::Write(const char* name, Math::Matrix& val)
	{
		StartBlock(name);

		for (int i = 0; i < 16; i++)
		{
			char tmp[16];
			StringUtils::Printf(tmp, 16, "m%i", i);
			Write(tmp, val.matrix[i]);
		}

		FinishBlock();
	}

	void JsonWriter::Write(const char* name, Color& val)
	{
		StartBlock(name);
		Write("r", val.r);
		Write("g", val.g);
		Write("b", val.b);
		Write("a", val.a);
		FinishBlock();
	}

	void JsonWriter::Write(const char* name, Math::Quaternion& val)
	{
		StartBlock(name);
		Write("x", val.x);
		Write("y", val.y);
		Write("z", val.z);
		Write("w", val.w);
		FinishBlock();
	}

	void JsonWriter::AddComma()
	{
		if (block_started[depth])
		{
			block_started[depth] = false;
		}
		else
		{
			WriteStr(",\n");
		}
	}

	void JsonWriter::WriteFormatedStr(bool addComa, const char* format, ...)
	{
		if (file)
		{
			if (addComa)
			{
				AddComma();
			}

			char dest[1024];
			va_list argptr;
			va_start(argptr, format);
			stbsp_vsnprintf(dest, 1024, format, argptr);
			va_end(argptr);

			for (int i=0; i<offset; i++)
			{
				fwrite("\t", 1, 1, file);
			}

			fwrite(dest, strlen(dest), 1, file);
		}
	}

	void JsonWriter::WriteStr(const char* str)
	{
		if (file)
		{
			fwrite(str, strlen(str), 1, file);
		}
	}

	void JsonWriter::Close()
	{
		block_started[0] = true;
		offset = 0;

		if (file)
		{
			WriteStr("\n}");

			fclose(file);
			file = nullptr;
		}
	}
}
