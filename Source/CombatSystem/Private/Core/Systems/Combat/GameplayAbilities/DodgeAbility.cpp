// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/GameplayAbilities/DodgeAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "GameFramework/Character.h"

UDodgeAbility::UDodgeAbility()
{
	StaminaCost = 15.f;   // flat cost, handled by the base class 
}

bool UDodgeAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayTagContainer* SourceTags,const FGameplayTagContainer* TargetTags,FGameplayTagContainer* OptionalRelevantTags) const
{
	const UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

	if (ASC && ASC->HasMatchingGameplayTag(CombatTags::State_Attacking.GetTag()) && !ASC->HasMatchingGameplayTag(CombatTags::State_ComboWindow.GetTag()))
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UDodgeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());

	// 1) Which direction, and which animation?
	const FVector InputDir = Character ? Character->GetLastMovementInputVector().GetSafeNormal2D() : FVector::ZeroVector;

	UAnimMontage* MontageToPlay = RollMontage;
	if (InputDir.IsNearlyZero() && BackstepMontage)
	{
		MontageToPlay = BackstepMontage; 
	}

	if (!Character || !MontageToPlay)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!InputDir.IsNearlyZero())
	{
		Character->SetActorRotation(InputDir.Rotation());
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay, 1.f);

	MontageTask->OnCompleted.AddDynamic(this, &UDodgeAbility::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &UDodgeAbility::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UDodgeAbility::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UDodgeAbility::OnMontageCancelled);
	MontageTask->ReadyForActivation();
}

void UDodgeAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->SetLooseGameplayTagCount(CombatTags::State_Invulnerable.GetTag(), 0);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDodgeAbility::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDodgeAbility::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}