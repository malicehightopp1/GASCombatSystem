// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CombatGameplayAbilities.generated.h"


UCLASS()
class UCombatGameplayAbilities : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UCombatGameplayAbilities();
	
	//Checking the cost for my weapons AKA stamina cost or if an ability need to take health to use
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	//Actually applies the cost for the weapons
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
protected:
	virtual float GetStaminaCost(const FGameplayAbilityActorInfo* ActorInfo) const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Costs") float StaminaCost = 0.0f;
};
