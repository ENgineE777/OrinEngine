
#pragma once

#include "Support/Support.h"

namespace Oak
{
	enum class TextureFormat
	{
		FMT_A8R8G8B8 = 0,
		FMT_A8R8,
		FMT_A8,
		FMT_R16_FLOAT,
		FMT_D16
	};

	enum class TextureFilter
	{
		Point = 0,
		Linear
	};

	enum class TextureAddress
	{
		Wrap = 0,
		Mirror,
		Clamp,
		Border
	};

	enum class TextureType
	{
		Tex2D = 0,
		Array,
		Cube,
		Volume
	};

	class Texture
	{
	public:

		eastl::string name;

		Texture(int w, int h, TextureFormat f, int l, TextureType tp)
		{
			width = w;
			height = h;
			format = f;
			lods = l;
			type = tp;

			magminf = TextureFilter::Linear;
			mipmapf = TextureFilter::Linear;

			adressU = TextureAddress::Wrap;
			adressV = TextureAddress::Wrap;
			adressW = TextureAddress::Wrap;
		};

		static void NextMip(int& width, int& height, int block_size);
		static int GetLevels(int width, int height, int block_size);

		virtual ~Texture() {};

		virtual void Apply(int slot) = 0;
		virtual void Update(int level, int layer, uint8_t* data, int stride) = 0;

		virtual int GetWidth() { return width; };
		virtual int GetHeight() { return height; };
		virtual TextureFormat GetFormat() { return format; };
		virtual int GetLods() { return lods; };
		virtual TextureType GetType() { return type; };
		virtual void SetFilters(TextureFilter magmin, TextureFilter mipmap)
		{
			magminf = magmin;
			mipmapf = mipmap;
		};

		virtual void SetAdress(TextureAddress adress)
		{
			adressU = adress;
			adressV = adress;
			adressW = adress;
		};

		virtual void SetAdressU(TextureAddress adress)
		{
			adressU = adress;
		};

		virtual void SetAdressV(TextureAddress adress)
		{
			adressV = adress;
		};

		virtual void SetAdressW(TextureAddress adress)
		{
			adressW = adress;
		};

		virtual void GenerateMips() = 0;

		virtual void Release() = 0;

	protected:

		int width;
		int height;
		TextureFormat format;
		int lods;

		TextureFilter magminf;
		TextureFilter mipmapf;
		TextureAddress adressU;
		TextureAddress adressV;
		TextureAddress adressW;
		TextureType type;
	};
}