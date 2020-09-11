
#include "Root/Root.h"

namespace Oak
{
	void Texture::NextMip(int& width, int& height, int block_size)
	{
		width = (width > block_size) ? (width >> 1) : block_size;
		height = (height > block_size) ? (height >> 1) : block_size;
	}

	int Texture::GetLevels(int width, int height, int block_size)
	{
		int w = width;
		int h = height;

		int levels = 0;

		while (w != 1 && h != 1)
		{
			NextMip(w, h, block_size);
			levels++;
		}

		return levels;
	}

	TextureRef::TextureRef(const TextureRef& ref)
	{
		texture = ref.texture;

		if (texture)
		{
			texture->refCounter++;
		}

		file = ref.file;
		line = ref.line;
		flMarker = new(file, line) TextureRef();
	}

	TextureRef::~TextureRef()
	{
		ReleaseRef();
	}

	Texture* TextureRef::Get()
	{
		return texture;
	}

	Texture* TextureRef::operator->() const
	{
		return texture;
	}

	TextureRef& TextureRef::operator=(const TextureRef& ref)
	{
		texture = ref.texture;

		if (texture)
		{
			texture->refCounter++;
		}

		file = ref.file;
		line = ref.line;
		flMarker = new(file, line) TextureRef();

		return *this;
	}

	void TextureRef::ReleaseRef()
	{
		DELETE_PTR(flMarker)

		if (texture)
		{
			texture->refCounter--;

			if (texture->refCounter == 0)
			{
				texture->Release();
			}

			texture = nullptr;
		}
	}
}