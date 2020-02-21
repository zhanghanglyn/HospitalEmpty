#include "HpListViewSlot.h"

UHpListViewSlot::UHpListViewSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UHpListViewSlot::SynchronizeProperties()
{

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
}