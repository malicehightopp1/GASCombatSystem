// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Animation/AnimNotifies/ANS_GiveTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

static UAbilitySystemComponent* GetOwnerASC(USkeletalMeshComponent* MeshComp)
{
	// In the montage editor preview there's no real character, so this returns null. That's fine.
	return MeshComp ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()) : nullptr;
}

void UANS_GiveTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UAbilitySystemComponent* ASC = GetOwnerASC(MeshComp))
	{
		ASC->SetLooseGameplayTagCount(Tag, 1);   // tag ON
	}
}

void UANS_GiveTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (UAbilitySystemComponent* ASC = GetOwnerASC(MeshComp))
	{
		ASC->SetLooseGameplayTagCount(Tag, 0);   // tag OFF
	}

	Super::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UANS_GiveTag::GetNotifyName_Implementation() const
{
	return Tag.IsValid() ? Tag.ToString() : TEXT("Grant Tag");
}