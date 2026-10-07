// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CombatAnimInstance.generated.h"


class UCharacterMovementComponent;
class ACharacter;

UCLASS()
class UCombatAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement") float GroundSpeed = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement") float Direction = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement") bool bShouldMove = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement") bool bIsFalling = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement") bool bIsSprinting = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement") bool bIsLockedOn = false;
	
private:
	UPROPERTY() TObjectPtr<ACharacter> Character;
	UPROPERTY() TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;
};
