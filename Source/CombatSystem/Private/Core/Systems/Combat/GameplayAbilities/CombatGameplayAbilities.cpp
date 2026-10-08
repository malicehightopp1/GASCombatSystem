// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/GameplayAbilities/CombatGameplayAbilities.h"

#include "AbilitySystemComponent.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"

UCombatGameplayAbilities::UCombatGameplayAbilities()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UCombatGameplayAbilities::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!CostGameplayEffectClass)
	{
		//if ability is free return tue
		return true;
	}
	
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC)
	{
		return false;
	}
	
	//this is for setting the system like elden ring where you can still do an action with stamina left but if it hits 0 cant do anything
	return ASC->GetNumericAttribute(UCombatAttributeSet::GetStaminaAttribute()) > 1.0f;
}

void UCombatGameplayAbilities::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const float Cost = GetStaminaCost(ActorInfo);
	if (!CostGameplayEffectClass || Cost <= 0.0f)
	{
		return;
	}
	
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(Handle, ActorInfo, ActivationInfo, CostGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo));
	
	if (Spec.IsValid())
	{
		Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_StaminaCost.GetTag(), -Cost);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
	}
	
}

float UCombatGameplayAbilities::GetStaminaCost(const FGameplayAbilityActorInfo* ActorInfo) const
{
	return StaminaCost;
}
