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
#include "UMGOverride/OverrideUMGUtil.h"
#include "UMGOverride/OverrideUMGInterface.h"

/*
	自定义一个简单的ListView，此为Slate部分
	后续涉及到拖动   20.2.28 再继承一个InterFace接口来实现回调等
	4.1 其实list的大小不应该由内部多少个Child来决定，而是应该由外部设置的宽高来决定
*/
class SHpListView : public SPanel , public UMGInterFace
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

		FSlot& AutoSize(const TAttribute<bool>& InAutoSize)
		{
			AutoSizeAttr = InAutoSize;
			return *this;
		}

		FSlot& Offset(const TAttribute<bool>& InAutoSize)
		{
			AutoSizeAttr = InAutoSize;
			return *this;
		}

		/** Position */
		TAttribute<FVector2D> PositionAttr;

		/** Size */
		TAttribute<FVector2D> SizeAttr;

		EHorizontalAlignment HAlignment;

		EVerticalAlignment VAlignment;

		TAttribute<bool> AutoSizeAttr;

		FSlot() : TSlotBase<FSlot>()
			, PositionAttr(FVector2D::ZeroVector)
			, SizeAttr(FVector2D(1.0f, 1.0f))
			, HAlignment(HAlign_Left)
			, VAlignment(VAlign_Top)
			, AutoSizeAttr(false)
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
		, _InBStartNotOffset(false)
		, _InLayoutDirection(FLayoutDirection::Vertical)
		, _InBClipping(true)
		, _InExtremOffset(0)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}
	SLATE_SUPPORTS_SLOT(SHpListView::FSlot)  //会在自身注册一个Slots，所以在Construct中，可以获取Slots

	SLATE_STYLE_ARGUMENT(FHpListViewStyle, Style)
	//SLATE_EVENT(DMouseBtnDownCall , CMouseBtnDownCallBack)
	SLATE_ATTRIBUTE( const FSlateBrush* , InBGImage)
	SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
	SLATE_ARGUMENT( int32, InRow)
	SLATE_ARGUMENT( int32, InColumn)
	SLATE_ARGUMENT(FVector2D, InOffset)
	SLATE_ARGUMENT(bool, InBStartNotOffset)
	SLATE_ARGUMENT(FLayoutDirection, InLayoutDirection)	//排列方向
	SLATE_ARGUMENT(bool , InBClipping)
	SLATE_ARGUMENT(float , InExtremOffset)
	//SLATE_EVENT(DMouseBtnDownCall, InMouseBtnDownCall)

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
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
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
	/* 设置是否第一个ITEM启用Offset */
	void SetBStartNotOffset(bool InBStartNotOffset);
	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);
	/** See the ColorAndOpacity attribute */
	void SetColorAndOpacity(FLinearColor InColorAndOpacity);
	/* 设置ComputeDesiredSize的Size,该Size以外部拖动的大小为主 */
	void SetDesignSize( FVector2D InDesignSize );
	/* 设置点击回调的参数 */
	void SetMouseDownParam(UUMGParamBase* InClcikParam)
	{
		MouseDownParamBase = InClcikParam;
	};
	/* 设置是否裁切 */
	void SetBClipping(bool InBClipping)
	{
		BClipping = InBClipping;
	}
	/* 设置可继续拖动的距离 */
	void SetExtremOffset(float InExtremOffset)
	{
		ExtremOffset = InExtremOffset;
	}
	/* 设置基础移动加速度 */
	void SetBaseASpeed(float InBaseASpeed) { BaseASpeed = InBaseASpeed; };
	/* 设置加速度递减值 */
	void SetDownASpeed(float InDownASpeed) {
		DownASpeed = InDownASpeed;
	};
	/* 设置LayOut */
	void SetLayOut(FLayoutDirection InLayOut) { LayoutDirection = InLayOut; };

	/* 是否用内容大小设置ListView */
	void SetBSetListViewSizeWithContent(bool InBSetListViewSizeWithContent) { BSetListViewSizeWithContent = InBSetListViewSizeWithContent; };
protected:
	/*先做一些初始化的计算，因为Arrange是每一帧都在跑的*/
	void Init();

	/* 根据当前拖动的DragMoveOffset计算一下，拖动的最大距离和最小距离等 */
	void CalculateDragMoveOffset(FVector2D &InDragOffset );

	/* 计算一个当前所有ITEM的总大小(计算一次就好) */
	FVector2D ComputeAllItemSize() const;

	/* 根据时间计算加速度 */
	void CalculateASpeed();

public:
	//开始点击的委托
	//DMouseBtnDownCall DelegateMouseBtnDownCall;

protected:
	/*  */
	TPanelChildren<FSlot> Children;

	/* 当前Slate的style */
	const FHpListViewStyle* Style;

	/** Color and opacity scale for this image */
	TAttribute<FSlateColor> ColorAndOpacity;

	/* 设计大小 */
	FVector2D DesignSize;

	/* 鼠标是否按下 */
	bool BMouseButtonDown = false;

	//背景图片
	TAttribute<const FSlateBrush*> BgImage;

	TAttribute<UUMGParamBase*> MouseDownParamBase;
	
	//设计行列数
	int32 Row = 1;
	int32 Column = 1;

	//Item间的空距
	FVector2D Offset;

	//是否起始Item不需要Offset
	bool BStartNotOffset = false;

	//排布方向
	FLayoutDirection LayoutDirection;

	/* 是否开启裁剪 */
	bool BClipping;

	/* 是否使用内容大小来设置ListView的大小 */
	bool BSetListViewSizeWithContent = false;

	/*
			********  以下为新添加，对于拖动相关的变量  ********
	*/
	/* 对应方向上的移动距离，鼠标拖动内部时，根据一个移动的加速度来设置Offset，该Offset设置最终的List内部位置 */
	FVector2D DragMoveOffset = FVector2D::ZeroVector;
	/* 当拖动开始时的鼠标位置 */
	FVector2D DragLastPos = FVector2D::ZeroVector;
	/* 当前拖动的鼠标位置 */
	FVector2D DragCurPos = FVector2D::ZeroVector;
	/* 用来计算加速度的点击位置 */
	FVector2D DragClickPos = FVector2D::ZeroVector;
	/* 加速度,通过一个递减的加速度，可以来实现List拖动完毕后继续滑行的功能 */
	float ASpeed = 0;
	/* 加速度的基数 */
	float BaseASpeed = 100;
	float DownASpeed = 1;

	/* 当到达拖动边界时，还可以拖动的一段距离 */
	float ExtremOffset = 0;
	/* 最大以及最小可以拖动的距离 */
	float MaxUp = 0;
	float MaxDown = 0;
	/* 当前所有Item的最大设计大小 */
	FVector2D FinalDesiredSize = FVector2D::ZeroVector;
	/* 本次拖动所使用的时间 */
	int64 DragTimestamp = 0 ;
	/* 因为用FTime获取的时间有问题，所以自己在Tick中增加 */
	float DragTime = 0;
	bool BStartCountTime = false;
	/*************************	END	***********************/
};