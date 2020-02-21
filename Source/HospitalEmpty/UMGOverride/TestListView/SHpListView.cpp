#include "SHpListView.h"
#include "Layout/ArrangedChildren.h"
#include "GenericPlatform/GenericPlatformMath.h"

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
	LayoutDirection = InArgs._InLayoutDirection;



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

	for (int32 ChildIndex = 0; ChildIndex > Children.Num(); ChildIndex++)
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
	for (int32 ChildIndex = 0; ChildIndex > Children.Num(); ChildIndex++)
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

FVector2D SHpListView::ComputeDesiredSize(float) const
{
	FVector2D FinalDesiredSize(0, 0);

	FVector2D StartLocation = Offset;
	int32 LineCount = 1;

	//上一个Item的坐标
	FVector2D LastItemLocation = StartLocation;
	//当前行的最大高度
	float MaxHeight = 0;

	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ChildIndex++)
	{
		const SHpListView::FSlot& CurSlot = Children[ChildIndex];
		const EVisibility ChildVisibilty = CurSlot.GetWidget()->GetVisibility();
		if (ChildVisibilty != EVisibility::Collapsed)
		{
			const FVector2D CurWidgetSize = CurSlot.GetWidget()->GetDesiredSize();

			//根据行列计算位置
			int32 RowOff = ChildIndex % Row;
			//StartLocation.X = Offset.X + (RowOff * (CurWidgetSize.X + Offset.X ));
			StartLocation.X = LastItemLocation.X + (RowOff * CurWidgetSize.X + Offset.X);
			//记录下当前行的最大距离
			MaxHeight = FGenericPlatformMath::Max(CurWidgetSize.Y, MaxHeight);

			if (LineCount > Row)
			{
				//返回成下一行第一个
				StartLocation.Y += (MaxHeight + Offset.Y);//( CurWidgetSize.Y + Offset.Y);

				//换行的时候，才会判断最大X和最大Y
				FinalDesiredSize.X = FGenericPlatformMath::Max(FinalDesiredSize.X, MaxHeight);
				FinalDesiredSize.Y = FGenericPlatformMath::Max(FinalDesiredSize.Y, StartLocation.Y);

				StartLocation.X = Offset.X;
				LineCount = 1;
			}

			LastItemLocation.X = StartLocation.X;
			LastItemLocation.Y = StartLocation.Y; //FGenericPlatformMath::Max(LastItemLocation.Y, )

			LineCount++;

		}
	}

	return FinalDesiredSize;
}
#pragma optimize("",off)
void SHpListView::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	//记录每一行的最大高度，以最大高度来设置下一行的显示位置
	FVector2D StartLocation = Offset;
	int32 LineCount = 1;

	//上一个Item的坐标
	FVector2D LastItemLocation = StartLocation;
	//当前行的最大高度
	float MaxHeight = 0;

	for (int32 ChildIndex = 0; ChildIndex < Children.Num() ; ChildIndex++)
	{
		const SHpListView::FSlot& CurSlot = Children[ChildIndex];
		const EVisibility ChildVisibilty = CurSlot.GetWidget()->GetVisibility();
		if (ChildVisibilty != EVisibility::Collapsed)
		{
			const FVector2D CurWidgetSize = CurSlot.GetWidget()->GetDesiredSize();

			//根据行列计算位置
			int32 RowOff = ChildIndex % Row;
			//StartLocation.X = Offset.X + (RowOff * (CurWidgetSize.X + Offset.X ));
			StartLocation.X = LastItemLocation.X + (RowOff * (CurWidgetSize.X + Offset.X));
			//记录下当前行的最大距离
			MaxHeight = FGenericPlatformMath::Max(CurWidgetSize.Y, MaxHeight);

			if (LineCount > Row)
			{
				//返回成下一行第一个
				StartLocation.Y += (MaxHeight + Offset.Y);//( CurWidgetSize.Y + Offset.Y);
				StartLocation.X = Offset.X;
				LineCount = 1;
			}

			LastItemLocation.X = StartLocation.X;
			LastItemLocation.Y = StartLocation.Y; //FGenericPlatformMath::Max(LastItemLocation.Y, )

			LineCount++;

			//添加绘制
			ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(
				// The child widget being arranged
				CurSlot.GetWidget(),
				// Child's local position (i.e. position within parent)
				StartLocation,
				// Child's size
				CurWidgetSize
			));

			//每换一行，重置一下MaxHeight
			if (LineCount > Row)
				MaxHeight = 0;
		}
	}

}
#pragma optimize("",on)
int32 SHpListView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	/* 在底下画一个背景图 */
	/*const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(1, 1)));
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		FlippedGeometry.ToPaintGeometry(),
		GetBackgroundImage(),
		ESlateDrawEffect::DisabledEffect,
		GetBackgroundImage()->GetTint(InWidgetStyle)
		//GetBackgroundImage()->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint() * FLinearColor::White
	);*/
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