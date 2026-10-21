// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Systems/Combat/GameplayAbilities/CombatGameplayAbilities.h"
#include "DodgeAbility.generated.h"

class UAnimMontage;

UCLASS()
class UDodgeAbility : public UCombatGameplayAbilities
{
	GENERATED_BODY()

public:
	UDodgeAbility();

	// Can't dodge mid-swing, only during an attack's recovery (combo window).
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayTagContainer* SourceTags = nullptr,const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")TObjectPtr<UAnimMontage> RollMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dodge")TObjectPtr<UAnimMontage> BackstepMontage;

	UFUNCTION() void OnMontageFinished();

	UFUNCTION() void OnMontageCancelled();
};
