// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/UI/PlayerUI/WeaponSelectionWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/Systems/Combat/Weapons/UWeaponData.h" 

void UWeaponSelectionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	for (int32 i = 0; i < 16; ++i)
	{
		UImage* WeaponSlot = Cast<UImage>(GetWidgetFromName(FName(*FString::Printf(TEXT("Slot%d"), i))));
		if (!WeaponSlot) break;

		SlotBackgrounds.Add(WeaponSlot);
		SlotIcons.Add(Cast<UImage>(GetWidgetFromName(FName(*FString::Printf(TEXT("Icon%d"), i)))));  
	}

	SetVisibility(ESlateVisibility::Collapsed);   // hidden until opened
	UE_LOG(LogTemp, Warning, TEXT("WHEEL found %d slots"), SlotBackgrounds.Num());   // DEBUG
}

void UWeaponSelectionWidget::Open(const TArray<TObjectPtr<UUWeaponData>>& InWeapons, int32 CurrentIndex)
{
	Weapons = InWeapons;

	for (int32 i = 0; i < SlotBackgrounds.Num(); ++i)
	{
		const bool bHasWeapon = Weapons.IsValidIndex(i) && Weapons[i];
		SlotBackgrounds[i]->SetVisibility(bHasWeapon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

		if (UImage* Icon = SlotIcons[i])
		{
			if (bHasWeapon && Weapons[i]->Icon)
			{
				Icon->SetBrushFromTexture(Weapons[i]->Icon);
				Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
			else
			{
				Icon->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	SetHighlighted(CurrentIndex);  
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UWeaponSelectionWidget::Close()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWeaponSelectionWidget::UpdateSelection(const FVector2D& Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("WHEEL dir=%s  size=%.2f"), *Direction.ToString(), Direction.Size());   // DEBUG
	const int32 SlotCount = SlotBackgrounds.Num();
	if (SlotCount == 0 || Direction.Size() < DeadZone) return;   // not pushed far enough: keep current

	float Angle = FMath::RadiansToDegrees(FMath::Atan2(Direction.X, Direction.Y));
	if (Angle < 0.f) Angle += 360.f;

	const float Slice = 360.f / SlotCount;
	const int32 Index = FMath::FloorToInt((Angle + Slice * 0.5f) / Slice) % SlotCount;

	if (Weapons.IsValidIndex(Index) && Weapons[Index])   // ignore empty slots
	{
		SetHighlighted(Index);
	}
}

void UWeaponSelectionWidget::SetHighlighted(int32 Index)
{
	HighlightedIndex = Index;

	for (int32 i = 0; i < SlotBackgrounds.Num(); ++i)
	{
		const bool bSelected = (i == Index);
		SlotBackgrounds[i]->SetColorAndOpacity(bSelected ? HighlightColor : NormalColor);
		SlotBackgrounds[i]->SetRenderScale(bSelected ? FVector2D(1.15f) : FVector2D(1.f));   // pop the chosen one
	}

	if (WeaponNameText)
	{
		const UUWeaponData* Weapon = Weapons.IsValidIndex(Index) ? Weapons[Index].Get() : nullptr;
		WeaponNameText->SetText(!Weapon ? FText::GetEmpty() : Weapon->WeaponName.IsEmpty() ? FText::FromString(Weapon->GetName()) : Weapon->WeaponName);
	}
}