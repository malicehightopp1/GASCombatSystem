// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Systems/Combat/GameplayAbilities/CombatGameplayAbilities.h"
#include "HitReactionAbility.generated.h"

/**
 * 
 */
UCLASS()
class UHitReactionAbility : public UCombatGameplayAbilities
{
	GENERATED_BODY()

public:
	UHitReactionAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> FrontMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> BackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> LeftMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Hit React")
	TObjectPtr<UAnimMontage> RightMontage;

	UAnimMontage* PickMontage(const AActor* Victim, const AActor* Attacker) const;

	UFUNCTION()
	void OnMontageFinished();

	UFUNCTION()
	void OnMontageCancelled();
	
};
