// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBarWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitFromASC(UAbilitySystemComponent* InASC);
	void SetLockedOn(bool bInLockedOn);
	
protected:
	virtual void NativeDestruct() override;
	
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> StanceBar;

	UPROPERTY(EditDefaultsOnly, Category = "HealthBar") float ShowDuration = 4.f;
	
private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnOtherChanged(const FOnAttributeChangeData& Data);

	void RefreshBars();
	void ShowTemporarily();
	void HideIfIdle();
	
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	bool bLockedOn = false;
	FTimerHandle HideTimerHandle;
};
