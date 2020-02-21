#pragma once

#include "CoreMinimal.h"
#include "Widgets/SPanel.h"
#include "SlotBase.h"
#include "SUserWidget.h"
//#include <math.h>
#include "Layout/Children.h"
#include "Layout/Visibility.h"
#include "Styling/CoreStyle.h"
#include "FHpListViewStyle.h"
//#include "SConstraintCanvas.h"
#include "UMGOverride/OverridUMGUtil.h"

/*
	自定义一个简单的ListView，此为Slate部分
	后续涉及到拖动
*/
class SHpListView : public SPanel
{
public:
	/* 定义slot 底下写法FSlot& 因为用法一般都是 TSharedPtr XXX = XXX.Position(11).Size(11),所以最终需要返回自身 */
	class FSlot : public TSlotBase<FSlot>
	{
	public:
		FSlot& Position(const TAttribute<FVector2D>& InPosition)
		{
			PositionAttr = InPosition;
			return *this;
		}

		FSlot& Size(const TAttribute<FVector2D>& InSize)
		{
			SizeAttr = InSize;
			return *this;
		}

		FSlot& HAlign(EHorizontalAlignment InHAlignment)
		{
			HAlignment = InHAlignment;
			return *this;
		}

		FSlot& VAlign(EVerticalAlignment InVAlignment)
		{
			VAlignment = InVAlignment;
			return *this;
		}

		FSlot& Expose(FSlot*& OutVarToInit)
		{
			OutVarToInit = this;
			return *this;
		}

		/** Position */
		TAttribute<FVector2D> PositionAttr;

		/** Size */
		TAttribute<FVector2D> SizeAttr;

		EHorizontalAlignment HAlignment;

		EVerticalAlignment VAlignment;

		FSlot() : TSlotBase<FSlot>()
			, PositionAttr(FVector2D::ZeroVector)
			, SizeAttr(FVector2D(1.0f, 1.0f))
			, HAlignment(HAlign_Left)
			, VAlignment(VAlign_Top)
		{}
	};

	/* 定义自身Slate相关 */
	SLATE_BEGIN_ARGS(SHpListView)
		: _Style(nullptr)
		, _InBGImage(FCoreStyle::Get().GetBrush("Border"))
		, _ColorAndOpacity(FLinearColor::White)
		, _InRow(1)
		, _InColumn(1)
		, _InOffset(FVector2D::ZeroVector)
		, _InLayoutDirection(FLayoutDirection::Vertical)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}
	SLATE_SUPPORTS_SLOT(SHpListView::FSlot)  //会在自身注册一个Slots，所以在Construct中，可以获取Slots

	SLATE_STYLE_ARGUMENT(FHpListViewStyle, Style)
	SLATE_ATTRIBUTE( const FSlateBrush* , InBGImage)
	SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
	SLATE_ARGUMENT( int32, InRow)
	SLATE_ARGUMENT( int32, InColumn)
	SLATE_ARGUMENT(FVector2D, InOffset)
	SLATE_ARGUMENT(FLayoutDirection, InLayoutDirection)	//排列方向

	SLATE_END_ARGS()

	/************************************************************************/
	/*                          类功能相关函数                              */
	/************************************************************************/
public:
	SHpListView();
	void Construct(const FArguments& InArgs);

	/** virtual **/
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FChildren* GetChildren() override;
	//virtual bool IsInteractable() const override { return true; };
	//virtual bool SupportsKeyboardFocus() const override { return true; };
	//virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	//virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	/** virtual end **/

	static FSlot& Slot()
	{
		return *(new FSlot());
	}

	FSlot& AddSlot()
	{
		Invalidate(EInvalidateWidget::Layout);

		SHpListView::FSlot& NewSlot = *(new FSlot());
		this->Children.Add(&NewSlot);
		return NewSlot;
	}

	int32 RemoveSlot(const TSharedRef<SWidget>& SlotWidget);

	/*
	获取某个FSLot
	*/
	SHpListView::FSlot* GetSlot(const TSharedRef<SWidget>& SlotWidget);

	/**
	* Removes all slots from the panel.
	*/
	void ClearChildren();
	/* 设置Style */
	void SetStyle(const FHpListViewStyle* InStyle);
	/* 设置背景图片 */
	void SetBackGroundImage(const FSlateBrush* InBackgroundImage);
	/* 获取背景图片 */
	const FSlateBrush* GetBackgroundImage() const;
	/*设置Offset*/
	void SetItemOffSet( FVector2D InOffset);
	/* 设置Row */
	void SetRow(int32 InRow);
	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);
	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);
protected:
	/*先做一些初始化的计算，因为Arrange是每一帧都在跑的*/
	void Init();

protected:
	/*  */
	TPanelChildren<FSlot> Children;

	/* 当前Slate的style */
	const FHpListViewStyle* Style;

	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

protected:
	//背景图片
	TAttribute<const FSlateBrush*> BgImage;
	
	//设计行列数
	int32 Row = 1;
	int32 Column = 1;

	//Item间的空距
	FVector2D Offset;

	//排布方向
	FLayoutDirection LayoutDirection;

};