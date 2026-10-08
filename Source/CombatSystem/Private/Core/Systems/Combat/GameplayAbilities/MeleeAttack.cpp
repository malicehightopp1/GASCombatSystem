// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/GameplayAbilities/MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"
#include "Core/Systems/Combat/Weapons/WeaponComponent.h"

UMeleeAttack::UMeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	const ACombatCharacter* Character = Cast<ACombatCharacter>(ActorInfo->AvatarActor.Get());
	UWeaponComponent* WeaponComp = Character ? Character->GetWeaponComponent() : nullptr;
	UUWeaponData* Weapon = WeaponComp ? WeaponComp->GetCurrentWeapon() : nullptr;

	if (!Weapon || !Weapon->GetAttack(AttackSlot, 0, CurrentAttack))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: equipped weapon has no attack for this slot"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float AttackSpeed = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UCombatAttributeSet::GetAttackSpeedAttribute());
	const float PlayRate = AttackSpeed > 0.f ? AttackSpeed : 1.f;

	UAbilityTask_PlayMontageAndWait* MontageTask =UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CurrentAttack.Montage, PlayRate);

	MontageTask->OnCompleted.AddDynamic(this, &UMeleeAttack::OnMontageFinished);
	MontageTask->OnBlendOut.AddDynamic(this, &UMeleeAttack::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UMeleeAttack::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &UMeleeAttack::OnMontageCancelled);

	MontageTask->ReadyForActivation();
}

float UMeleeAttack::GetStaminaCost(const FGameplayAbilityActorInfo* ActorInfo) const
{
	const ACombatCharacter* Character = ActorInfo ? Cast<ACombatCharacter>(ActorInfo->AvatarActor.Get()) : nullptr;
	const UWeaponComponent* WeaponComp = Character ? Character->GetWeaponComponent() : nullptr;
	const UUWeaponData* Weapon = WeaponComp ? WeaponComp->GetCurrentWeapon() : nullptr;
	
	//this is deciding the stamina cost per ability
	return Weapon ? Weapon->StaminaCost * CurrentAttack.StaminaCostMultiplier : 0.f;
	
}

void UMeleeAttack::OnMontageFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	UE_LOG(LogTemp, Warning, TEXT("Montage finished"))
}

void UMeleeAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	UE_LOG(LogTemp, Warning, TEXT("Montage Cancelled"))
}
