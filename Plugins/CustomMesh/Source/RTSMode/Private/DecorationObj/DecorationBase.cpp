#include "DecorationBase.h"
#include "DecorationGridMgrComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GroundUtil.h"

ADecorationBase::ADecorationBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	if (GridMeshComponent == nullptr)
	{
		GridMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GridStaticMesh"));
		if (!GroundMeshPath.IsEmpty())
		{
			UStaticMesh* GroundMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *GroundMeshPath));
			if (GroundMesh)
			{
				GridMeshComponent->SetStaticMesh(GroundMesh);
				/*FBoxSphereBounds MeshBounds = GroundMesh->GetBounds();
				FBox MeshBox = MeshBounds.GetBox();
				FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X, GroundWidthHeight.Y / MeshBox.GetSize().Y, 1);
				GridMeshComponent->SetWorldScale3D(Scale)*/
			}
		}
	}
}

void ADecorationBase::OnConstruction(const FTransform& Transform)
{
	//添加格子MGR
	if (GridMgr == nullptr)
	{
		GridMgr = NewObject<UDecorationGridMgrComponent>(this, TEXT("GridMgr"));
		GridMgr->RegisterComponent();
	}

}

void ADecorationBase::BeginPlay()
{
	UpdateGridInfo();
}

void ADecorationBase::UpdateGridInfo()
{
	/* 更新格子数据，占多少个格子 */


	/* 根据格子数据，更新格子模型 */
	if(GridMeshComponent)
	{
		float GridMeshWidth = GridParamData.RowNum * FGroundUtil::GroundGridWidth;
		float GridMeshHeight = GridParamData.ColumnNum * FGroundUtil::GroundGridHeight;

		FBoxSphereBounds MeshBounds = GridMeshComponent->GetStaticMesh()->GetBounds();
		FVector ScaleSize = FVector((GridMeshWidth / MeshBounds.GetBox().GetSize().X), (GridMeshHeight / MeshBounds.GetBox().GetSize().Y), 1);
		GridMeshComponent->SetWorldScale3D(ScaleSize);
	}
}