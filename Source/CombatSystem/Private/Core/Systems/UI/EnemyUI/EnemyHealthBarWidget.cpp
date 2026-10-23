// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/UI/EnemyUI/EnemyHealthBarWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"

void UEnemyHealthBarWidget::InitFromASC(UAbilitySystemComponent* InASC)
{
	ASC = InASC;
	if (!InASC) return;

	InASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetHealthAttribute()).AddUObject(this, &UEnemyHealthBarWidget::OnHealthChanged);

	InASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UEnemyHealthBarWidget::OnOtherChanged);
	InASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStanceAttribute()).AddUObject(this, &UEnemyHealthBarWidget::OnOtherChanged);
	InASC->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxStanceAttribute()).AddUObject(this, &UEnemyHealthBarWidget::OnOtherChanged);

	RefreshBars();
	SetVisibility(ESlateVisibility::Collapsed);
}

void UEnemyHealthBarWidget::SetLockedOn(bool bInLockedOn)
{
	bLockedOn = bInLockedOn;
	
	if (bLockedOn)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else if (UWorld* World = GetWorld())
	{
		if (!World->GetTimerManager().IsTimerActive(HideTimerHandle))
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UEnemyHealthBarWidget::NativeDestruct()
{
	if (UAbilitySystemComponent* AbilitySystem = ASC.Get())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetHealthAttribute()).RemoveAll(this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStanceAttribute()).RemoveAll(this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxStanceAttribute()).RemoveAll(this);
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HideTimerHandle);
	}
	Super::NativeDestruct();
}

void UEnemyHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	RefreshBars();
	
	if(Data.NewValue < Data.OldValue)
	{
		ShowTemporarily();
	}
}

void UEnemyHealthBarWidget::OnOtherChanged(const FOnAttributeChangeData& Data)
{
	RefreshBars();
}

void UEnemyHealthBarWidget::RefreshBars()
{
	UAbilitySystemComponent* AbilitySystem = ASC.Get();
	if (!AbilitySystem) return;

	const float Health = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetHealthAttribute());
	const float MaxHealth = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetMaxHealthAttribute());
	HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);

	if (StanceBar)
	{
		const float Stance = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetStanceAttribute());
		const float MaxStance = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetMaxStanceAttribute());
		StanceBar->SetPercent(MaxStance > 0.f ? Stance / MaxStance : 0.f);
	}
}

void UEnemyHealthBarWidget::ShowTemporarily()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(HideTimerHandle, this, &UEnemyHealthBarWidget::HideIfIdle, ShowDuration, true);
	}
}

void UEnemyHealthBarWidget::HideIfIdle()
{
	if (!bLockedOn)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}
