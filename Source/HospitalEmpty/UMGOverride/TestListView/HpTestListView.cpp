#include "HpTestListView.h"

UHpTestListView::UHpTestListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FText const UHpTestListView::GetPaletteCategory()
{
	return FText::FromString(FString(L"自定义UMG"));
}

void UHpTestListView::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

UClass* UHpTestListView::GetSlotClass() const
{
	return UPanelSlot::StaticClass();
}

void UHpTestListView::OnSlotAdded(UPanelSlot* InSlot)
{

}

void UHpTestListView::OnSlotRemoved(UPanelSlot* InSlot)
{

}

TSharedRef<SWidget> UHpTestListView::RebuildWidget()
{
	return Super::RebuildWidget();
}