#include "SHpListView.h"
#include "Layout/ArrangedChildren.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Runtime/Engine/Public/Engine.h"
#include "Base/HptGameInstance.h"

SHpListView::SHpListView() : Children(this)
{

}

void SHpListView::Construct(const FArguments& InArgs)
{
	BgImage = InArgs._InBGImage.Get();
	Row = InArgs._InRow;
	Column = InArgs._InColumn;
	Offset = InArgs._InOffset;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	BStartNotOffset = InArgs._InBStartNotOffset;
	LayoutDirection = InArgs._InLayoutDirection;
	//DelegateMouseBtnDownCall = InArgs._CMouseBtnDownCallBack;
	BClipping = InArgs._InBClipping;
	ExtremOffset = InArgs._InExtremOffset;

	/* 初始化Slot */  
	const int32 NumSlots = InArgs.Slots.Num();
	for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
	{
		Children.Add(InArgs.Slots[SlotIndex]);
	}

	Init();
}

/********************************增删查改部分*******************************/
int32 SHpListView::RemoveSlot(const TSharedRef<SWidget>& SlotWidget)
{
	Invalidate(EInvalidateWidget::Layout);

	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ChildIndex++)
	{
		if (SlotWidget == Children.GetChildAt(ChildIndex))
		{
			Children.RemoveAt(ChildIndex);
			return ChildIndex;
		}
	}

	return -1;
}

SHpListView::FSlot* SHpListView::GetSlot(const TSharedRef<SWidget>& SlotWidget)
{
	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ChildIndex++)
	{
		if (SlotWidget == Children.GetChildAt(ChildIndex))
		{
			return &Children[ChildIndex];
		}
	}

	return nullptr;
}

void SHpListView::ClearChildren()
{
	if (Children.Num())
	{
		Invalidate(EInvalidateWidget::Layout);
		Children.Empty();
	}
}
/********************************增删查改部分END*******************************/

void SHpListView::Init()
{

}
#pragma optimize("",off)
/* 但其实这个函数，在Scanvas.cpp里面，是返回的Zero */
FVector2D SHpListView::ComputeDesiredSize(float) const
{

	//如果当前还未计算过总大小，计算一次
	//if (FinalDesiredSize.IsZero())
	//	FinalDesiredSize = FVector2D(ComputeAllItemSize());

	if (BSetListViewSizeWithContent)
		return ComputeAllItemSize();

	return DesignSize;
}

void SHpListView::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	//记录每一行的最大高度，以最大高度来设置下一行的显示位置
	FVector2D StartLocation = FVector2D::ZeroVector;//Offset;
	if (!BStartNotOffset)
		StartLocation.Y = Offset.Y;
	int32 LineCount = 1;

	//上一个Item的坐标
	FVector2D LastItemLocation = FVector2D::ZeroVector;
	float LastWidth = 0;
	//当前行的最大高度
	float MaxHeight = 0;

	for (int32 ChildIndex = 0; ChildIndex < Children.Num() ; ChildIndex++)
	{
		const SHpListView::FSlot& CurSlot = Children[ChildIndex];
		const EVisibility ChildVisibilty = CurSlot.GetWidget()->GetVisibility();
		if (ArrangedChildren.Accepts(ChildVisibilty) && ChildVisibilty != EVisibility::Collapsed)
		{
			bool BAutoSize = CurSlot.AutoSizeAttr.Get();
			const FVector2D CurWidgetSize = BAutoSize? CurSlot.GetWidget()->GetDesiredSize() : CurSlot.SizeAttr.Get(); //.GetWidget()->GetDesiredSize(); //20.2.27 修改
			
			/* 如果是纵向的排版 */
			//if (LayoutDirection == FLayoutDirection::Vertical)
			{
				//根据行列计算位置
				//int32 RowOff = ChildIndex % Row;
				StartLocation.X = LastItemLocation.X + (LastWidth + Offset.X);//(CurWidgetSize.X + Offset.X);
				LastWidth = CurWidgetSize.X;
				if (BStartNotOffset && ChildIndex == 0)
					StartLocation.X -= Offset.X;

				if (LineCount > Row)
				{
					//返回成下一行第一个
					StartLocation.Y += (MaxHeight + Offset.Y);//( CurWidgetSize.Y + Offset.Y);
					StartLocation.X = Offset.X;
					LineCount = 1;
					// 20.2.28 换行后，当前行的最大距离要清空
					MaxHeight = 0;
					//换行后，清除当前行的X Offset
					if (BStartNotOffset)
						StartLocation.X -= Offset.X;
				}

				//记录下当前行的最大距离
				MaxHeight = FGenericPlatformMath::Max(CurWidgetSize.Y, MaxHeight);

				LastItemLocation.X = StartLocation.X;
				LastItemLocation.Y = StartLocation.Y; //FGenericPlatformMath::Max(LastItemLocation.Y, )

				LineCount++;

				/* 4.3 如果要裁切，且当前ITEM已经位于显示区域之外了，直接不加入显示 */
				if (BClipping)
				{
					//判断是不是超过了上面显示
					if ((((StartLocation + DragMoveOffset) + Offset).Y + CurWidgetSize.Y) < 0)
						continue;
					//判断是不是超过了下面显示
					if (((StartLocation + DragMoveOffset - Offset).Y) > DesignSize.Y)
						continue;
				}
			}
			
			//添加绘制
			ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
				CurSlot.GetWidget(),
				// Child's local position (i.e. position within parent)
				StartLocation + DragMoveOffset,
				// Child's size
				CurWidgetSize
			));
		}
	}

}

#pragma optimize("",on)
int32 SHpListView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if(BClipping)
		OutDrawElements.PushClip(FSlateClippingZone(AllottedGeometry));

	const FSlateBrush* ImageBrush = GetBackgroundImage();
	//画背景图2.0 
	if ((ImageBrush != nullptr) && (ImageBrush->DrawAs != ESlateBrushDrawType::NoDrawType))
	{
		const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
		const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint() * ColorAndOpacity.Get().GetColor(InWidgetStyle) * ImageBrush->GetTint(InWidgetStyle));

		FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), ImageBrush, DrawEffects, FinalColorAndOpacity);
	}


	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	this->ArrangeChildren(AllottedGeometry, ArrangedChildren);

	// Because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
	// wants to an overlay for all of its contents.
	int32 MaxLayerId = LayerId;

	const bool bForwardedEnabled = ShouldBeEnabled(bParentEnabled);

	const FPaintArgs NewArgs = Args.WithNewParent(this);

	for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
	{
		FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];

		if (!IsChildWidgetCulled(MyCullingRect, CurWidget))
		{
			const int32 CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect, OutDrawElements, MaxLayerId + 1, InWidgetStyle, bForwardedEnabled);

			MaxLayerId = FMath::Max(MaxLayerId, CurWidgetsMaxLayerId);
		}
	}

	if (BClipping)
		OutDrawElements.PopClip();

	return MaxLayerId;
}

FChildren* SHpListView::GetChildren()
{
	return &Children;
}

/************************************************************************/
/*                      设置Style等绘制笔刷相关                          */
/************************************************************************/

const FSlateBrush* SHpListView::GetBackgroundImage() const
{
	return &Style->BackgroundImage ? &Style->BackgroundImage : BgImage.Get();
}


void SHpListView::SetBackGroundImage(const FSlateBrush* InBackgroundImage)
{
	BgImage = InBackgroundImage;
	Invalidate(EInvalidateWidget::Layout);
}

void SHpListView::SetStyle(const FHpListViewStyle* InStyle)
{
	Style = InStyle;

	if (Style == nullptr)
	{
		FArguments Defaults;
		Style = Defaults._Style;
	}

	check(Style);

	Invalidate(EInvalidateWidget::Layout);
}

void SHpListView::SetItemOffSet(FVector2D InOffset)
{
	Offset = InOffset;
	Invalidate(EInvalidateWidget::Layout);
}

void SHpListView::SetRow(int32 InRow)
{
	Row = InRow;
	Invalidate(EInvalidateWidget::Layout);
}

void SHpListView::SetBStartNotOffset(bool InBStartNotOffset)
{
	BStartNotOffset = InBStartNotOffset;
	Invalidate(EInvalidateWidget::Layout);
}

void SHpListView::SetColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	if (!ColorAndOpacity.IdenticalTo(InColorAndOpacity))
	{
		ColorAndOpacity = InColorAndOpacity;
		Invalidate(EInvalidateWidget::PaintAndVolatility);
	}
}

void SHpListView::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	if (!ColorAndOpacity.IdenticalTo(InColorAndOpacity))
	{
		ColorAndOpacity = InColorAndOpacity;
		Invalidate(EInvalidateWidget::PaintAndVolatility);
	}
}

void SHpListView::SetDesignSize(FVector2D InDesignSize)
{
	DesignSize = InDesignSize;
}

/************************************************************************/
/*                           点击ListView相关                           */
/************************************************************************/

/* 并不拦截，还是可以往下传递 */
FReply SHpListView::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();

	if (IsEnabled() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("SHpListView::OnMouseButtonDown"));

		//如果当前还未计算过总大小，计算一次
		//if (FinalDesiredSize.IsZero())
		FinalDesiredSize = ComputeAllItemSize();

		Reply = FReply::Handled();

		BMouseButtonDown = true;
		FVector2D MouseScreenPos = MouseEvent.GetScreenSpacePosition();  //鼠标指针在屏幕上的位置
		FVector2D absolutePos = MyGeometry.GetAbsolutePosition();	//该控件在屏幕上的位置，相减就是鼠标在该控件上的相对位置
		DragLastPos = MouseScreenPos;

		DragClickPos = MouseScreenPos;

		/* 为列表计算一个加速度 */
		{
			BStartCountTime = true;
			ASpeed = 0;
		}
	}

	return Reply;
}

FReply SHpListView::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();

	//UE_LOG(LogTemp, Warning, TEXT("SHpListView::OnMouseMove"));
	if (IsEnabled() && BMouseButtonDown)
	{
		Reply = FReply::Handled();

		/* 设置移动距离 */
		//if (BMouseButtonDown)
		{
			FVector2D MouseScreenPos = MouseEvent.GetScreenSpacePosition();

			if (LayoutDirection == FLayoutDirection::Horizontal)
				DragMoveOffset.X += (MouseScreenPos.X - DragLastPos.X);
			else if(LayoutDirection == FLayoutDirection::Vertical)
				DragMoveOffset.Y += (MouseScreenPos.Y - DragLastPos.Y);

			DragLastPos = MouseScreenPos;

			CalculateDragMoveOffset(DragMoveOffset);
		}
	}

	return Reply;
}

FReply SHpListView::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();

	if (IsEnabled() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("SHpListView::OnMouseButtonUp"));

		Reply = FReply::Handled();

		BMouseButtonDown = false;

		ASpeed = 0;
		CalculateASpeed();
		BStartCountTime = false;
	}

	return Reply;
}

void SHpListView::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();

	if (IsEnabled() && BMouseButtonDown)//MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("SHpListView::OnMouseLeave"));

		Reply = FReply::Handled();
		BMouseButtonDown = false;

		ASpeed = 0;
		CalculateASpeed();
		BStartCountTime = false;
	}

}

FReply SHpListView::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("SHpListView::OnFocusReceived"))
	FReply Reply = FReply::Handled();

	return Reply;
}

void SHpListView::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("SHpListView::OnFocusLost"))
	
}

/************************************************************************/
/*                           拖动 相关                                  */
/************************************************************************/
FVector2D SHpListView::ComputeAllItemSize() const 
{
	FVector2D TFinalDesiredSize = FVector2D::ZeroVector;

	FVector2D StartLocation = FVector2D::ZeroVector;//Offset;
	if (!BStartNotOffset)
		StartLocation.Y = Offset.Y;
	int32 LineCount = 1;

	//上一个Item的坐标
	FVector2D LastItemLocation = FVector2D::ZeroVector;
	//当前行的最大宽度
	float LastWidth = 0;
	float MaxHeight = 0;
	float MaxWidth = 0;
	/* 4.3 最后一个记录的ITEM宽度的Width */
	float LastItemWidth = 0;

	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ChildIndex++)
	{
		const SHpListView::FSlot& CurSlot = Children[ChildIndex];
		const EVisibility ChildVisibilty = CurSlot.GetWidget()->GetVisibility();
		if (ChildVisibilty != EVisibility::Collapsed)
		{
			//const FVector2D CurWidgetSize = CurSlot.GetWidget()->GetDesiredSize();
			bool BAutoSize = CurSlot.AutoSizeAttr.Get();
			const FVector2D CurWidgetSize = BAutoSize ? CurSlot.GetWidget()->GetDesiredSize() : CurSlot.SizeAttr.Get(); //.GetWidget()->GetDesiredSize(); //20.2.27 修改

			//根据行列计算位置
			//int32 RowOff = ChildIndex % Row;
			StartLocation.X = LastItemLocation.X + (LastWidth + Offset.X);//(CurWidgetSize.X + Offset.X);
			LastWidth = CurWidgetSize.X;
			MaxWidth = StartLocation.X + CurWidgetSize.X;
			if (BStartNotOffset && ChildIndex == 0)
				StartLocation.X -= Offset.X;

			if (LineCount > Row)
			{
				//返回成下一行第一个
				StartLocation.Y += (MaxHeight + Offset.Y);//( CurWidgetSize.Y + Offset.Y);
				StartLocation.X = Offset.X;
				LineCount = 1;
				// 20.2.28 换行后，当前行的最大距离要清空
				MaxHeight = 0;
				//换行后，清除当前行的X Offset
				if (BStartNotOffset)
					StartLocation.X -= Offset.X;
			}

			//记录下当前行的最大距离
			MaxHeight = FGenericPlatformMath::Max(CurWidgetSize.Y, MaxHeight);
			//换行的时候，才会判断最大X和最大Y
			/* 4.3 如果是最后把Size撑到最大的width，则记录下该Item的size */
			if (MaxWidth > TFinalDesiredSize.X)
				LastItemWidth = CurWidgetSize.X;
			TFinalDesiredSize.X = FGenericPlatformMath::Max(TFinalDesiredSize.X, MaxWidth);
			TFinalDesiredSize.Y = FGenericPlatformMath::Max(TFinalDesiredSize.Y, (StartLocation.Y + CurWidgetSize.Y));

			LastItemLocation.X = StartLocation.X;
			LastItemLocation.Y = StartLocation.Y; //FGenericPlatformMath::Max(LastItemLocation.Y, )

			LineCount++;

		}
	}

	TFinalDesiredSize.X -= LastItemWidth;

	return TFinalDesiredSize;
}

void SHpListView::CalculateDragMoveOffset(FVector2D &InDragOffset)
{
	if (BSetListViewSizeWithContent)
		DesignSize = ComputeDesiredSize(1);
	
	//可以往上拖的最小值
	if (InDragOffset.Y < (-FinalDesiredSize.Y /*- ExtremOffset*/ + DesignSize.Y - Offset.Y))
		InDragOffset.Y = (-FinalDesiredSize.Y + DesignSize.Y - Offset.Y);
		//设置可拖动的最大值
	if (InDragOffset.Y > 0)//ExtremOffset)
		InDragOffset.Y = 0;
	
	if (InDragOffset.X < (-FinalDesiredSize.X + DesignSize.X))///*- ExtremOffset */ + Offset.X))
		InDragOffset.X = (-FinalDesiredSize.X + DesignSize.X);//+ Offset.X);
	if (InDragOffset.X > 0)
		InDragOffset.X = 0;
		
}

void SHpListView::CalculateASpeed()
{
	//根据加速度计算一个递减的移动
	//if (FMath::Abs(DragLastPos.Y - DragClickPos.Y) > 0)
	//{
	DragTime *= 1000;
	if (DragTime > 0)
	{
		if (LayoutDirection == FLayoutDirection::Vertical)
		{
			if (DragLastPos.Y - DragClickPos.Y == 0)
				ASpeed = 0;
			else
				ASpeed = (DragLastPos.Y - DragClickPos.Y) / DragTime * BaseASpeed;
			
		}
			
		else if (LayoutDirection == FLayoutDirection::Horizontal)
		{
			if(DragLastPos.X - DragClickPos.X == 0)
				ASpeed = 0;
			else
				ASpeed = (DragLastPos.X - DragClickPos.X) / DragTime * BaseASpeed;
		}
			
	}
	//}
	DragTime = 0;
}

/************************************************************************/
/*                           Tick 相关                                  */
/************************************************************************/
void SHpListView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	
	/* 递减加速度 */
	if (FMath::Abs(ASpeed) > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT(" ASpeed : %f"), ASpeed);

		//DragMoveOffset = FVector2D(DragMoveOffset.X + ASpeed, DragMoveOffset.Y + ASpeed);
		if (LayoutDirection == FLayoutDirection::Horizontal)
			DragMoveOffset.X = DragMoveOffset.X + ASpeed;
		else if(LayoutDirection == FLayoutDirection::Vertical)
			DragMoveOffset.Y = DragMoveOffset.Y + ASpeed;

		/* 计算一个加速度的递减 */
		float TempAbs = FMath::Abs(ASpeed);
		ASpeed = (TempAbs - DownASpeed) * (ASpeed / TempAbs);  //值乘上符号
		if (TempAbs - DownASpeed < 0)
			ASpeed = 0;

		CalculateDragMoveOffset(DragMoveOffset);
	}

	/* 计算一个拖动时间 */
	if (BStartCountTime)
		DragTime++;
	else
		DragTime = 0;
}