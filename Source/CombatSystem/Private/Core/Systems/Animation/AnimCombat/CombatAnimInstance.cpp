// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Animation/AnimCombat/CombatAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCombatAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Character = Cast<ACharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovementComponent = Character->GetCharacterMovement();
	}
}

void UCombatAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!Character)
	{	
		NativeInitializeAnimation();
		return;
	}
	if (!CharacterMovementComponent)
	{
		return;
	}
	const FVector Velocity = CharacterMovementComponent->Velocity;
	
	GroundSpeed = Velocity.Size2D();
	
	const bool bHasInput = !CharacterMovementComponent->GetCurrentAcceleration().IsNearlyZero();
	bShouldMove = GroundSpeed > 3.0f && bHasInput;
	
	bIsFalling = CharacterMovementComponent->IsFalling();
	
	const FVector LocalVelocity = Character->GetActorRotation().UnrotateVector(Velocity);
	Direction = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
	{
		bIsSprinting = ASC->HasMatchingGameplayTag(CombatTags::State_Sprinting.GetTag());
		bIsLockedOn = ASC->HasMatchingGameplayTag(CombatTags::State_LockedOn.GetTag());
	}
}
