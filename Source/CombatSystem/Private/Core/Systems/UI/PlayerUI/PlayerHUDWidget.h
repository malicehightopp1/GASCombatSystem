// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/Systems/Combat/Sets/CombatAttributeSet.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void InitFromAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent);
	
protected:
	virtual void NativeDestruct() override; 
	
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> StaminaBar;
	
private:
	void OnAttributeChanged(const FOnAttributeChangeData& Data);
	void RefreshUI();
	
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
