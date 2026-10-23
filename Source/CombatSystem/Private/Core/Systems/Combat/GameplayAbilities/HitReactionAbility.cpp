// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/GameplayAbilities/HitReactionAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UHitReactionAbility::UHitReactionAbility()
{
	bRetriggerInstancedAbility = true;
}

UAnimMontage* UHitReactionAbility::PickMontage(const AActor* Victim, const AActor* Attacker) const
{
	if (!Victim || !Attacker) return FrontMontage.Get();

	const FVector ToAttacker = (Attacker->GetActorLocation() - Victim->GetActorLocation()).GetSafeNormal2D();
	const float ForwardDot = FVector::DotProduct(Victim->GetActorForwardVector(), ToAttacker);   // +1 front, -1 behind
	const float RightDot   = FVector::DotProduct(Victim->GetActorRightVector(),   ToAttacker);   // +1 right, -1 left

	UAnimMontage* Picked = nullptr;
	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		Picked = ForwardDot >= 0.f ? FrontMontage.Get() : BackMontage.Get();
	}
	else
	{
		Picked = RightDot >= 0.f ? RightMontage.Get() : LeftMontage.Get();
	}

	return Picked ? Picked : FrontMontage.Get(); //do the front react if no direction is found - default
}

void UHitReactionAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	// TriggerEventData is the payload sent by the attribute set
	const AActor* Victim   = ActorInfo->AvatarActor.Get();
	const AActor* Attacker = TriggerEventData ? TriggerEventData->Instigator.Get() : nullptr;

	UAnimMontage* Montage = PickMontage(Victim, Attacker);

	if (!Montage || !CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Montage, 1.f);

	MontageTask->OnCompleted.AddDynamic(this, &UHitReactionAbility::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &UHitReactionAbility::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UHitReactionAbility::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UHitReactionAbility::OnMontageCancelled);
	MontageTask->ReadyForActivation();
}

void UHitReactionAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHitReactionAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}