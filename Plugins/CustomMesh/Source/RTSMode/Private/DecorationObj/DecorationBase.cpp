#include "DecorationBase.h"
#include "DecorationGridMgrComponent.h"
#include "GroundObj.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DtSceneComponentBase.h"
#include "Engine/StaticMesh.h"
#include "GroundUtil.h"

ADecorationBase::ADecorationBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	//if (RootSceneComponent == nullptr)
	//{
		//RootSceneComponent = ObjectInitializer.CreateDefaultSubobject<UDtSceneComponentBase>(this, TEXT("DecorateRootComponent"));
	//}
	RootSceneComponent = CreateDefaultSubobject<UDtSceneComponentBase>(TEXT("Root"));
	SetRootComponent(RootSceneComponent);
	GridMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridStaticMesh"));
	GridMeshComponent->SetupAttachment(RootSceneComponent);
	DecorationMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DecorationMesh"));
	DecorationMeshComponent->SetupAttachment(RootSceneComponent);
}

void ADecorationBase::OnConstruction(const FTransform& Transform)
{
	//添加格子MGR
	if (GridMgr == nullptr)
	{
		GridMgr = NewObject<UDecorationGridMgrComponent>(this, TEXT("GridMgr"));
		GridMgr->RegisterComponent();
		GridMgr->InitGridMgr( ParamData, this);
	}

	//添加格子模型
	if (GridMeshComponent != nullptr)
	{
		//GridMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("GridStaticMesh"));
		//GridMeshComponent->RegisterComponent();
		//FAttachmentTransformRules MeshRuls(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false);
		//GridMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		if (!GroundMeshPath.IsEmpty())
		{
			UStaticMesh* GroundMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *GroundMeshPath));
			if (GroundMesh)
			{
				GridMeshComponent->SetStaticMesh(GroundMesh);
			}
			/* 为格子模型设置材质 */
			if (ParamData.GridMaterial != nullptr)
			{
				if (GridMeshMaterial)
					GridMeshMaterial = nullptr;
				GridMeshMaterial = UMaterialInstanceDynamic::Create(ParamData.GridMaterial, GridMeshComponent);
				if (GridMeshMaterial)
					GridMeshComponent->SetMaterial(0, GridMeshMaterial);
			}
		}
	}

	/* 再创建家具模型 */
	if (DecorationMeshComponent != nullptr)
	{
		if (ParamData.DecorationMesh != nullptr)
		{
			DecorationMeshComponent->SetStaticMesh(ParamData.DecorationMesh);
		}
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
		float GridMeshWidth = ParamData.GridParamData.RowNum * FGroundUtil::GroundGridWidth;
		float GridMeshHeight = ParamData.GridParamData.ColumnNum * FGroundUtil::GroundGridHeight;

		FBoxSphereBounds MeshBounds = GridMeshComponent->GetStaticMesh()->GetBounds();
		FVector ScaleSize = FVector((GridMeshWidth / MeshBounds.GetBox().GetSize().X), (GridMeshHeight / MeshBounds.GetBox().GetSize().Y), 1);
		GridMeshComponent->SetWorldScale3D(ScaleSize);
	}
	/* 更新格子模型的材质 */
	if (GridMeshMaterial)
	{
		GridMeshMaterial->SetScalarParameterValue("RowNum", ParamData.GridParamData.RowNum);
		GridMeshMaterial->SetScalarParameterValue("Column", ParamData.GridParamData.ColumnNum);
		GridMeshMaterial->SetScalarParameterValue("BorderWidth", BorderWidth);
	}
}

//void ADecorationBase::UpdateGridByGround()
//{
// GridMgr->CreateInitGrid();
//}

void ADecorationBase::SetGround(AActorBase* InGround)
{
	Ground = Cast<AGroundObj>(InGround);
	if (Ground)
		GridMgr->SetGroundGridMgr(Ground->GetGridMgr());


}

void ADecorationBase::GetGridWidthHeight(float &Width, float &Height)
{
	FVector Scale = GridMeshComponent->GetComponentTransform().GetScale3D();
	FBoxSphereBounds MeshBounds = GridMeshComponent->GetStaticMesh()->GetBounds();
	Width = MeshBounds.GetBox().GetSize().X * Scale.X;
	Height = MeshBounds.GetBox().GetSize().Y * Scale.Y;
}

/* 计算出左上角坐标到Actor的中间距离，宽高/2 ,再设置位置 */
void ADecorationBase::SetLocationByTopRight(FVector TopRightLocation)
{
	float Width = 0;
	float Height = 0;
	GetGridWidthHeight(Width, Height);
	FVector FinalLocation = FVector(TopRightLocation.X + Width / 2, TopRightLocation.Y + Height / 2, GetActorLocation().Z);
	SetActorLocation(FinalLocation);
}

#pragma optimize("",off)
FVector ADecorationBase::GetTopRightLocation()
{
	float Width = 0;
	float Height = 0;
	GetGridWidthHeight(Width, Height);
	FVector DecorationLocation = GetActorLocation();

	return FVector(DecorationLocation.X - Width / 2, DecorationLocation.Y - Height / 2, DecorationLocation.Z);
}

FVector ADecorationBase::GetTopRightLocation(FVector InLocation)
{
	float Width = 0;
	float Height = 0;
	GetGridWidthHeight(Width, Height);

	return FVector(InLocation.X - Width / 2, InLocation.Y - Height / 2, InLocation.Z);
}

/*
	根据中间点InLocation取右上角位置，判断属于地面的哪一个格子中，
	根据右上角格子更新家具位置。
*/
void ADecorationBase::MoveTo(FVector InLocation)
{
	GridMgr->MoveTo(InLocation);
}

#pragma optimize("",on)