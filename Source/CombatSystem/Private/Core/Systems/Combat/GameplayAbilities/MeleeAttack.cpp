// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/GameplayAbilities/MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UMeleeAttack::UMeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!AttackMontage || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage, 1.f);

	MontageTask->OnCompleted.AddDynamic(this, &UMeleeAttack::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &UMeleeAttack::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UMeleeAttack::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UMeleeAttack::OnMontageCancelled);

	MontageTask->ReadyForActivation(); // tasks don't start until you call this
}

void UMeleeAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UMeleeAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
