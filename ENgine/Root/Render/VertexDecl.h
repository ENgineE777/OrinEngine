
#pragma once

#include "Support/Support.h"
#include "Support/PointerRef.h"

namespace Oak
{
	/**
	\ingroup gr_code_root_render
	*/

	enum class ElementType
	{
		Float /*!< 1 component float */,
		Float2 /*!< 2 component float */,
		Float3 /*!< 3 component float */,
		Float4 /*!< 4 component float */,
		Int /*!< 1 component integer */,
		Int2 /*!< 2 component integer */,
		Int3 /*!< 3 component integer */,
		Int4 /*!< 4 component integer */,
		UInt /*!< 1 component unsigned integer */,
		UInt2 /*!< 2 component unsigned integer */,
		UInt3 /*!< 3 component unsigned integer */,
		UInt4 /*!< 4 component unsigned integer */,
		Ubyte4 /*!< 4 component unsigned byte */
	};

	/**
	\ingroup gr_code_root_render
	*/

	enum class ElementSemantic
	{
		Position /*!< Will be used as position */,
		Texcoord /*!< Will be used as texture coordinates */,
		Color /*!< Will be used as color */
	};

	/**
	\ingroup gr_code_root_render
	*/

	/**
	\brief VertexDecl

	This class holds decalration of a vertex.

	*/

	class VertexDecl
	{
		friend class PointerRef<VertexDecl>;
		int refCounter = 0;

		virtual void Release() = 0;
	public:

		struct ElemDesc
		{
			/** \brief data type of an element */
			ElementType type;

			/** \brief semantic of an element */
			ElementSemantic semantic;

			/** \brief index of an element*/
			int index;
		};
	};

	typedef PointerRef<VertexDecl> VertexDeclRef;
}
