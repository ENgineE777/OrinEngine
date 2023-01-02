
#include "JsonReader.h"
#include "Support/StringUtils.h"
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include "Root/Root.h"

namespace Orin
{
	JsonReader::JsonReader() : allocator(1 << 10)
	{
	}

	JsonReader::~JsonReader()
	{
		allocator.free();
	}

	bool JsonReader::ParseString(const char* source)
	{
		char* errorPos = 0;
		char* errorDesc = 0;
		int errorLine = 0;

		root = json_parse((char*)source, &errorPos, &errorDesc, &errorLine, &allocator);

		if (root)
		{
			nodes[curDepth] = root;
			curNode = nodes[curDepth];

			return true;
		}

		return false;
	}

	bool JsonReader::ParseFile(const char* name)
	{
		if (file.Load(name))
		{
			uint8_t* data = file.GetData();
			
			if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
			{
				data += 3;
			}

			return ParseString((const char*)data);
		}

		return false;
	}

	bool JsonReader::EnterBlock(const char* name)
	{
		nodes[curDepth +1] = FindValue(name);

		if (nodes[curDepth +1])
		{
			if (nodes[curDepth +1]->type == JSON_OBJECT)
			{
				curDepth++;
				curNode = nodes[curDepth];

				return true;
			}
			else
			if (nodes[curDepth +1]->type == JSON_ARRAY)
			{
				if (nodes[curDepth +1]->first_child)
				{
					curDepth++;
					nodes[curDepth] = nodes[curDepth]->first_child;
					curNode = nodes[curDepth];

					return true;
				}
			}
		}

		return false;
	}

	void JsonReader::LeaveBlock()
	{
		if (curDepth == 0) return;

		if (nodes[curDepth]->parent->type == JSON_ARRAY)
		{
			nodes[curDepth]->parent->first_child = nodes[curDepth]->next_sibling;
		}
	
		curDepth--;
		curNode = nodes[curDepth];
	}

	bool JsonReader::IsString(const char* name)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_STRING)
			{
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, char* val, int valLen)
	{
		json_value* node = FindValue(name);
	
		if (node)
		{
			if (node->type == JSON_STRING)
			{
				StringUtils::Copy(val, valLen, node->string_value);
				return true;
			}
			else
			if (node->type == JSON_BOOL || node->type == JSON_INT)
			{
				StringUtils::Printf(val, valLen, "%i", node->int_value);
				return true;
			}
			else
			if (node->type == JSON_FLOAT)
			{
				StringUtils::Printf(val, valLen, "%4.5f", node->float_value);
				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val[0] = 0;
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, eastl::string& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_STRING)
			{
				val = node->string_value;
				return true;
			}
			else
			if (node->type == JSON_BOOL || node->type == JSON_INT)
			{
				char tmp[128];
				StringUtils::Printf(tmp, 128, "%i", node->int_value);
				val = tmp;

				return true;
			}
			else
			if (node->type == JSON_FLOAT)
			{
				char tmp[128];
				StringUtils::Printf(tmp, 128, "%4.5f", node->float_value);
				val = tmp;

				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val = "";
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, bool& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_BOOL)
			{
				if (node->int_value > 0)
				{
					val = true;
				}
				else
				{
					val = false;
				}

				return true;
			}
			else
			if (node->type == JSON_STRING)
			{
				if (atoi(node->string_value) > 0)
				{
					val = true;
				}
				else
				{
					val = false;
				}

				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val = false;

				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, float& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_FLOAT)
			{
				val = node->float_value;
				return true;
			}
			else
			if (node->type == JSON_INT)
			{
				val = (float)node->int_value;
				return true;
			}
			else
			if (node->type == JSON_STRING)
			{
				val = (float)atof(node->string_value);
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, uint16_t& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_INT)
			{
				val = node->int_value;

				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val = 0;
			
				return true;
			}
			else
			if (node->type == JSON_STRING)
			{
				val = atoi(node->string_value);
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, int& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_INT)
			{
				val = node->int_value;

				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val = 0;

				return true;
			}
			else
			if (node->type == JSON_STRING)
			{
				val = atoi(node->string_value);
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, uint32_t& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_INT)
			{
				val = node->int_value;

				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val = 0;
			
				return true;
			}
			else
			if (node->type == JSON_STRING)
			{
				val = atoi(node->string_value);
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, int64_t& val)
	{
		json_value* node = FindValue(name);

		if (node)
		{
			if (node->type == JSON_INT)
			{
				val = node->int64_value;
				return true;
			}
			else
			if (node->type == JSON_NULL)
			{
				val = 0;

				return true;
			}
			else
			if (node->type == JSON_STRING)
			{
				val = atol(node->string_value);
				return true;
			}
		}

		return false;
	}

	bool JsonReader::Read(const char* name, Math::Vector2& val)
	{
		if (EnterBlock(name))
		{
			Read("x", val.x);
			Read("y", val.y);

			LeaveBlock();

			return true;
		}

		return false;
	}

	bool JsonReader::Read(const char* name, Math::Vector3& val)
	{
		if (EnterBlock(name))
		{
			Read("x", val.x);
			Read("y", val.y);
			Read("z", val.z);

			LeaveBlock();

			return true;
		}

		return false;
	}

	bool JsonReader::Read(const char* name, Math::Vector4& val)
	{
		if (EnterBlock(name))
		{
			Read("x", val.x);
			Read("y", val.y);
			Read("z", val.z);
			Read("w", val.w);

			LeaveBlock();

			return true;
		}

		return false;
	}

	bool JsonReader::Read(const char* name, Math::Matrix& val)
	{
		if (EnterBlock(name))
		{
			for (int i = 0; i < 16; i++)
			{
				char tmp[16];
				StringUtils::Printf(tmp, 16, "m%i", i);
				Read(tmp, val.matrix[i]);
			}
		
			LeaveBlock();

			return true;
		}

		return false;
	}

	bool JsonReader::Read(const char* name, Color& val)
	{
		if (EnterBlock(name))
		{
			Read("r", val.r);
			Read("g", val.g);
			Read("b", val.b);
			Read("a", val.a);

			LeaveBlock();

			return true;
		}

		return false;
	}

	bool JsonReader::Read(const char* name, Math::Quaternion& val)
	{
		if (EnterBlock(name))
		{
			Read("x", val.x);
			Read("y", val.y);
			Read("z", val.z);
			Read("w", val.w);

			LeaveBlock();

			return true;
		}

		return false;
	}

	json_value* JsonReader::FindValue(const char* name)
	{
		if (!root)
		{
			return nullptr;
		}

		if (!name[0])
		{
			return curNode;
		}

		for (json_value *it = curNode->first_child; it; it = it->next_sibling)
		{
			if (StringUtils::IsEqual(name, it->name))
			{
				return it;
			}
		}

		return nullptr;
	}
}
