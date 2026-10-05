// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "PlayerCombatCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class APlayerCombatCharacter : public ACombatCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCombatCharacter();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<UCameraComponent> FollowCamera;

	
};
