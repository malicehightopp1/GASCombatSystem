// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/AI/BTTask_ActivateAbilityByTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"

UBTTask_ActivateAbilityByTag::UBTTask_ActivateAbilityByTag()
{
	NodeName = TEXT("Activate Ability By Tag");
	bNotifyTick = true;   
	bCreateNodeInstance = true;  
}

static UAbilitySystemComponent* GetPawnASC(UBehaviorTreeComponent& OwnerComp)
{
	const AAIController* AIC = OwnerComp.GetAIOwner();
	return AIC ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AIC->GetPawn()) : nullptr;
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UAbilitySystemComponent* ASC = GetPawnASC(OwnerComp);
	if (!ASC || AbilityTags.Num() == 0) return EBTNodeResult::Failed;

	ActiveTag = AbilityTags[FMath::RandRange(0, AbilityTags.Num() - 1)];

	if (!ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(ActiveTag)))
	{
		return EBTNodeResult::Failed;  
	}

	return EBTNodeResult::InProgress;  
}

void UBTTask_ActivateAbilityByTag::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const UAbilitySystemComponent* ASC = GetPawnASC(OwnerComp);

	if (!ASC || !IsAbilityActive(ASC))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_ActivateAbilityByTag::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UAbilitySystemComponent* ASC = GetPawnASC(OwnerComp))
	{
		FGameplayTagContainer Tags(ActiveTag);
		ASC->CancelAbilities(&Tags);
	}
	return EBTNodeResult::Aborted;
}

bool UBTTask_ActivateAbilityByTag::IsAbilityActive(const UAbilitySystemComponent* ASC) const
{
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.IsActive() && Spec.Ability->GetAssetTags().HasTag(ActiveTag))
		{
			return true;
		}
	}
	return false;
}

FString UBTTask_ActivateAbilityByTag::GetStaticDescription() const
{
	FString Desc = TEXT("Activate: ");
	for (const FGameplayTag& Tag : AbilityTags)
	{
		Desc += Tag.ToString() + TEXT("  ");
	}
	return Desc;
}