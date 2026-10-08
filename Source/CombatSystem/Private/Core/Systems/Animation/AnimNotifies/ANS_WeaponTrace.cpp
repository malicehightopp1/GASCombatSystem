// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Animation/AnimNotifies/ANS_WeaponTrace.h"

#include "Core/Systems/Combat/Weapons/WeaponComponent.h"

static UWeaponComponent* GetWeaponcomp(USkeletalMeshComponent* MeshComp)
{
	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	return Owner ? Owner->FindComponentByClass<UWeaponComponent>() : nullptr;
}
void UANS_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	if (UWeaponComponent* Weapon = GetWeaponcomp(MeshComp))
	{
		Weapon->BeginAttackTrace(Weapon->TraceRadius); // blade goes live
	}
}

void UANS_WeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	
	
	if (UWeaponComponent* Weapon = GetWeaponcomp(MeshComp))
	{
		Weapon->UpdateAttackTrace();
	}
}

void UANS_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (UWeaponComponent* Weapon = GetWeaponcomp(MeshComp))
	{
		Weapon->EndAttackTrace();
	}
	
	Super::NotifyEnd(MeshComp, Animation);
}
