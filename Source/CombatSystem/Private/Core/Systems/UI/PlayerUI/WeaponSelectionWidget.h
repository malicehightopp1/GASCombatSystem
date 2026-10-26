// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSelectionWidget.generated.h"

class UImage;
class UUWeaponData;
class UTextBlock;

UCLASS()
class UWeaponSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Open(const TArray<TObjectPtr<UUWeaponData>>& InWeapons, int32 CurrentIndex);
	void Close();

	void UpdateSelection(const FVector2D& Direction);
	int32 GetHighlightedIndex() const { return HighlightedIndex; }

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY(EditDefaultsOnly, Category = "Wheel") float DeadZone = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "Wheel") FLinearColor NormalColor = FLinearColor(1.f, 1.f, 1.f, 0.35f);

	UPROPERTY(EditDefaultsOnly, Category = "Wheel") FLinearColor HighlightColor = FLinearColor(1.f, 0.8f, 0.3f, 1.f);

private:
	void SetHighlighted(int32 Index);

	UPROPERTY() TArray<TObjectPtr<UImage>> SlotBackgrounds;
	UPROPERTY() TArray<TObjectPtr<UImage>> SlotIcons;
	UPROPERTY() TArray<TObjectPtr<UUWeaponData>> Weapons;

	int32 HighlightedIndex = INDEX_NONE;
};
