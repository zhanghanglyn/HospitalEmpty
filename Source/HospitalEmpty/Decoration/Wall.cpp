﻿#include "Wall.h"
/*
#include "DecorationGridMgrComponent.h"
#include "GroundObj.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DtSceneComponentBase.h"
#include "Engine/StaticMesh.h"
#include "GroundUtil.h"*/

AWall::AWall(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	
}

void AWall::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void AWall::BeginPlay()
{
	Super::BeginPlay();
}

void AWall::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
