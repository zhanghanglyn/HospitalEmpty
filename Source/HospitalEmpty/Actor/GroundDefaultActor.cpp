#include "GroundDefaultActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
//#include "GroundGridMgrComponent.h"

AGroundDefaultActor::AGroundDefaultActor(FString InActorName) : Super(InActorName)
{
	UE_LOG(LogTemp, Warning, TEXT("Is This Worked?"));
}

AGroundDefaultActor::AGroundDefaultActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 创建一个初始地面
	GroundMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GroundDefaultMesh"));
	SetRootComponent(GroundMeshComponent);
	
	if (!GroundMeshPath.IsEmpty())
	{
		UStaticMesh* GroundMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *GroundMeshPath));
		if (GroundMesh)
		{
			GroundMeshComponent->SetStaticMesh(GroundMesh);
			FBoxSphereBounds MeshBounds = GroundMesh->GetBounds();
			FBox MeshBox = MeshBounds.GetBox();
			FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X , GroundWidthHeight.Y / MeshBox.GetSize().Y , 1);
			GroundMeshComponent->SetWorldScale3D(Scale);
		}
	}
}

void AGroundDefaultActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//为地面格子创建一个材质实例
	CreateMaterialInstance();

}

void AGroundDefaultActor::BeginPlay()
{
	Super::BeginPlay();
	
}

#pragma optimize("",off)
void AGroundDefaultActor::StartTouch(FVector TouchLocation)
{

}
#pragma optimize("",on)

void AGroundDefaultActor::TouchHold(FVector TouchLocation)
{

}

void AGroundDefaultActor::TouchEnd(FVector TouchLocation)
{

}

#pragma optimize("",off)
void AGroundDefaultActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//UE_LOG(LogTemp, Warning, TEXT("Property changed!!!!!!!!"));
	if (PropertyChangedEvent.MemberProperty == nullptr)
		return;

	if (PropertyChangedEvent.MemberProperty->GetName() == "GroundMeshPath")
	{
		if (!GroundMeshPath.IsEmpty())
		{
			UStaticMesh* GroundMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *GroundMeshPath));
			if (GroundMesh)
			{
				GroundMeshComponent->SetStaticMesh(GroundMesh);
				FBoxSphereBounds MeshBounds = GroundMesh->GetBounds();
				FBox MeshBox = MeshBounds.GetBox();
				FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X, GroundWidthHeight.Y / MeshBox.GetSize().Y, 1);
				GroundMeshComponent->SetWorldScale3D(Scale);
			}
		}
	}
	//在地面宽高更新的时候，去更新格子数据并且更新Mesh的材质参数
	else if(PropertyChangedEvent.MemberProperty->GetName() == "GroundWidthHeight")
	{
		if (GroundMeshComponent)
		{
			if (UStaticMesh* TempStaticMesh = GroundMeshComponent->GetStaticMesh())
			{
				FBoxSphereBounds MeshBounds = TempStaticMesh->GetBounds();
				FBox MeshBox = MeshBounds.GetBox();
				FVector Scale = FVector(GroundWidthHeight.X / MeshBox.GetSize().X, GroundWidthHeight.Y / MeshBox.GetSize().Y, 1);
				GroundMeshComponent->SetWorldScale3D(Scale);
			}
		}
	}
}
#pragma optimize("",on)
void AGroundDefaultActor::CreateMaterialInstance()
{
	if (!GroundMeshComponent)
		return;
	UMaterialInterface* MeshMaterial = GroundMeshComponent->GetMaterial(0);

	if (GridDynamicMaterial)
	{
		GridDynamicMaterial = nullptr;
	}
	if (GridMaterial)
	{
		GridDynamicMaterial = UMaterialInstanceDynamic::Create(GridMaterial, GroundMeshComponent);
		GroundMeshComponent->SetMaterial(0, GridDynamicMaterial);
	}

}