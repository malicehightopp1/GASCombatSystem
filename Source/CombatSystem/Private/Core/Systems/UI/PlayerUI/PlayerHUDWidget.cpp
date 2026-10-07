// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/UI/PlayerUI/PlayerHUDWidget.h"

#include "Components/ProgressBar.h"

void UPlayerHUDWidget::InitFromAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	AbilitySystemComponent = InAbilitySystemComponent;
	
	if (!InAbilitySystemComponent) //check the in ability because its the reference being passed to the variable
	{
		return;
	}
	
	InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::OnAttributeChanged);
	InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UPlayerHUDWidget::OnAttributeChanged);
	InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStaminaAttribute()).AddUObject(this, &UPlayerHUDWidget::OnAttributeChanged);
	InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxStaminaAttribute()).AddUObject(this, &UPlayerHUDWidget::OnAttributeChanged);
	
	RefreshUI(); //call on start
}

void UPlayerHUDWidget::NativeDestruct()
{
	
	//remove all the delegates before destroying
	if (UAbilitySystemComponent* AbilitySystem = AbilitySystemComponent.Get())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetHealthAttribute()).RemoveAll(this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxHealthAttribute()).RemoveAll(this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetStaminaAttribute()).RemoveAll(this);
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxStaminaAttribute()).RemoveAll(this);
	}
	
	Super::NativeDestruct();
}

void UPlayerHUDWidget::OnAttributeChanged(const FOnAttributeChangeData& Data)
{
	RefreshUI();
}

void UPlayerHUDWidget::RefreshUI()
{
	UAbilitySystemComponent* AbilitySystem = AbilitySystemComponent.Get();
	
	if (!AbilitySystem)
	{
		return;
	}
	
	const float Health = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetHealthAttribute());
	const float MaxHealth = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetMaxHealthAttribute());
	const float Stamina = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetStaminaAttribute());
	const float MaxStamina = AbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetMaxStaminaAttribute());
	
	HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	StaminaBar->SetPercent(MaxStamina > 0.f ? Stamina / MaxStamina : 0.f);
}
