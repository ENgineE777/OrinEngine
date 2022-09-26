
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class CLASS_DECLSPEC ContainerWidget : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(ContainerWidget)

		enum class Align
		{
			alignLeft = 0 /*!< Aligment to the left */,
			alignCenter = 1 /*!< Aligment at the center */,
			alignRight = 2 /*!< Aligment to the right */,
			alignTop = 3 /*!< Aligment to the top */,
			alignBottom = 4 /*!< Aligment to the bottom */
		};

		enum class Size
		{
			fixed = 0 /*!< Size set up directly via properties of a widget */,
			fillParent = 1 /*!< Size equals to size of a parent */,
			wrapContext = 2 /*!< Size depends from a widget context */
		};

		/**
			\brief Horizontal aligment of a widget
		*/
		Align horzAlign = Align::alignLeft;

		/**
		\brief Vertical aligment of a widget
		*/
		Align vertAlign = Align::alignTop;

		/**
		\brief Type of width of a widget
		*/
		Size horzSize = Size::fixed;

		/**
		\brief Type of height of a widget
		*/
		Size vertSize = Size::fixed;

		/**
		\brief Color a widget
		*/
		Color color = COLOR_WHITE;

		/**
		\brief Left-Top padding of a widget
		*/
		Math::Vector2 leftPadding = 0.0f;

		/**
		\brief Right-Bottom padding of a widget
		*/
		Math::Vector2 rightPadding = 0.0f;

		Color curColor;

	#ifndef DOXYGEN_SKIP

		ContainerWidget() = default;;
		virtual ~ContainerWidget() = default;

		void Init() override;
		void CalcState();
		void FullDraw(float dt);
		void DrawSelfWithChilds(float dt);

		virtual void Draw(float dt);
	#endif
	};
}