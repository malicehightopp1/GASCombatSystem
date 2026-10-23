// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"


UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Targeting")void ToggleLockOn();
	UFUNCTION(BlueprintCallable, Category = "Targeting")void ClearTarget();

	void HandleSwitchInput(float AxisX);

	UFUNCTION(BlueprintPure, Category = "Targeting")bool IsLockedOn() const { return CurrentTarget != nullptr; }
	UFUNCTION(BlueprintPure, Category = "Targeting")AActor* GetCurrentTarget() const { return CurrentTarget; }

protected:
	UPROPERTY(EditAnywhere, Category = "Targeting") float MaxLockDistance = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Targeting") float BreakLockDistance = 2000.f;

	UPROPERTY(EditAnywhere, Category = "Targeting")float PriorityConeAngle = 30.f;
	UPROPERTY
	(EditAnywhere, Category = "Targeting|Camera")float CameraInterpSpeed = 10.f;
	UPROPERTY(EditAnywhere, Category = "Targeting|Camera")float CameraPitchOffset = -10.f;
	
	UPROPERTY(EditAnywhere, Category = "Targeting|Switching")float SwitchInputThreshold = 2.f;
	UPROPERTY(EditAnywhere, Category = "Targeting|Switching")float SwitchCooldown = 0.4f;

private:
	void LockOnTo(AActor* NewTarget);
	AActor* FindBestTarget() const;
	void SwitchTarget(float Direction);
	void GatherCandidates(TArray<AActor*>& OutCandidates) const;
	bool IsValidTarget(AActor* Target) const;
	bool GetCameraView(FVector& OutLocation, FRotator& OutRotation) const;

	UPROPERTY()TObjectPtr<AActor> CurrentTarget;
	float LastSwitchTime = -100.f;
};
