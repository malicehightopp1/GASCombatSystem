// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Systems/Combat/GameplayAbilities/CombatGameplayAbilities.h"
#include "BlockAbility.generated.h"

UCLASS()
class UBlockAbility : public UCombatGameplayAbilities
{
	GENERATED_BODY()
	
public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayTagContainer* SourceTags = nullptr,const FGameplayTagContainer* TargetTags = nullptr,FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Block") TSubclassOf<UGameplayEffect> BlockingEffect;

private:
	FActiveGameplayEffectHandle BlockingEffectHandle;
};
