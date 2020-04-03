﻿#include "HpListViewSlot.h"

UHpListViewSlot::UHpListViewSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), Slot(nullptr)
{
	LayoutData.Offsets = FMargin(0, 0, 100,100);
	LayoutData.Anchors = FAnchors(0, 0);
	LayoutData.Alignment = FVector2D(0, 0);

	BAutoSize = false;

	ZOrder = 0;
}

#if WITH_EDITOR
/****    UObject  属性变化接口     ****/
void UHpListViewSlot::PreEditChange(UProperty* PropertyThatWillChange)
{
	Super::PreEditChange(PropertyThatWillChange);

	Slot = nullptr;
}

void UHpListViewSlot::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	SynchronizeProperties();

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif
/*********************/

void UHpListViewSlot::SynchronizeProperties()
{
	SetOffsetSize(FVector2D( LayoutData.Offsets.Right , LayoutData.Offsets.Bottom));
	//SetAnchors(LayoutData.Anchors);
	//SetAlignment(LayoutData.Alignment);
	SetBAutoSize(BAutoSize);
	SetZorder(ZOrder);
}

void UHpListViewSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}

void UHpListViewSlot::BuildSlot(TSharedRef<SHpListView> InHpListView)
{
	Slot = &InHpListView->AddSlot()
	[
		Content == NULL?SNullWidget::NullWidget:Content->TakeWidget()
	];

	SynchronizeProperties();
}


/*******      设置属性等相关       ********/
void UHpListViewSlot::SetBAutoSize(bool InAutoSize)
{
	BAutoSize = InAutoSize;
	if (Slot)
		Slot->AutoSize(InAutoSize);
}

bool UHpListViewSlot::GetBAutoSize()
{	
	return BAutoSize;
}

void UHpListViewSlot::SetOffsetSize(FVector2D InSize)
{
	LayoutData.Offsets.Right = InSize.X;
	LayoutData.Offsets.Bottom = InSize.Y;

	if(Slot)
		Slot->Size(InSize);
}

FMargin UHpListViewSlot::GetOffsetSize()
{
	if (Slot)
	{
		return Slot->SizeAttr.Get();
	}

	return LayoutData.Offsets;
}

void UHpListViewSlot::SetZorder(int32 InZOrder)
{
	ZOrder = InZOrder;

	//if (Slot)
	//{
	//	Slot->setz
	//}
}


/*******             当控件以及属性发生变化时调用                  *******/
#if WITH_EDITOR
/*	轻推时产生的效果，因为不支持移动位置，所以直接返回false
*/
bool UHpListViewSlot::NudgeByDesigner(const FVector2D& NudgeDirection, const TOptional<int32>& GridSnapSize)
{
	return true;
}

bool UHpListViewSlot::DragDropPreviewByDesigner(const FVector2D& LocalCursorPosition, const TOptional<int32>& XGridSnapSize, const TOptional<int32>& YGridSnapSize)
{
	return true;
}

void UHpListViewSlot::SynchronizeFromTemplate(const UPanelSlot* const TemplateSlot)
{
	return;
}

#endif

/* 测试新添加 测试能否开启拖动缩放等相关 */
void UHpListViewSlot::SetLayout(const FHPAnchorData& InLayoutData)
{
	LayoutData = InLayoutData;

	if (Slot)
	{
		Slot->Offset(LayoutData.Offsets);
		Slot->Anchors(LayoutData.Anchors);
		//Slot->Alignment(LayoutData.Alignment);
	}
}

FHPAnchorData UHpListViewSlot::GetLayout() const
{
	return LayoutData;
}

void UHpListViewSlot::SetSize(FVector2D InSize)
{
	LayoutData.Offsets.Right = InSize.X;
	LayoutData.Offsets.Bottom = InSize.Y;

	if (Slot)
	{
		Slot->Offset(LayoutData.Offsets);
	}
}

FVector2D UHpListViewSlot::GetSize() const
{
	if (Slot)
	{
		FMargin Offsets = Slot->AutoSizeAttr.Get();
		return FVector2D(Offsets.Right, Offsets.Bottom);
	}

	return FVector2D(LayoutData.Offsets.Right, LayoutData.Offsets.Bottom);
}
