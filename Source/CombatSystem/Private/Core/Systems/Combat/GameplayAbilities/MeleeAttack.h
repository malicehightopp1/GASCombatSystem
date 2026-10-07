// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MeleeAttack.generated.h"


UCLASS()
class UMeleeAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMeleeAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack") TObjectPtr<UAnimMontage> AttackMontage;
	UFUNCTION() void OnMontageFinished();
	UFUNCTION() void OnMontageCancelled();
};
