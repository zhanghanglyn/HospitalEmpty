#include "CreateWidget.h"

UCreateWidget::UCreateWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCreateWidget::AddToListView(UWidget* Content)
{
	if (ListView)
	{
		ListView->AddChildToList(Content);
	}
}