// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Targetable.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTargetable : public UInterface
{
	GENERATED_BODY()
};

class COMBATSYSTEM_API ITargetable
{
	GENERATED_BODY()

public:
	virtual bool CanBeTargeted() const = 0;
	virtual FVector GetLockOnLocation() const = 0;
	virtual void SetLockOnMarkerVisible(bool bVisible) = 0;
};