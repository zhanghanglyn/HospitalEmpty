#include "SHpListView.h"
#include "Layout/ArrangedChildren.h"

void SHpListView::Construct(const FArguments& InArgs)
{
	BgImage = InArgs._InBGImage;
	Row = InArgs._InRow.Get();
	Column = InArgs._InColumn.Get();
	OffsetX = InArgs._InOffsetX.Get();
	OffsetY = InArgs._InOffsetY.Get();
	LayoutDirection = InArgs._InLayoutDirection.Get();



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
	Children.Empty();
}
/********************************增删查改部分END*******************************/

void SHpListView::Init()
{

}

void SHpListView::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	//记录每一行的最大高度，以最大高度来设置下一行的显示位置
	float CurRowMaxHeight = 0;
	FVector2D StartLocation = FVector2D::ZeroVector;
	int32 LineCount = 1;

	for (int32 ChildIndex = 0; ChildIndex < Children.Num() ; ChildIndex++)
	{
		const SHpListView::FSlot& CurSlot = Children[ChildIndex];
		const FVector2D CurWidgetSize = CurSlot.GetWidget()->GetDesiredSize();

		FVector2D Pos = FVector2D::ZeroVector;

		//计算当前Child所在的行和列
		//int32 ArrangeColumn = Children.Num() % Column;
		//int32 ArrangeRow = floor(Children.Num() / Column);
		
		//根据行列计算位置
		StartLocation.X += CurWidgetSize.X ;
		if (LineCount > 3)
		{
			StartLocation.Y += CurWidgetSize.Y;
			LineCount = 1;
		}

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
	}

}

FChildren* SHpListView::GetChildren()
{
	return &Children;
}