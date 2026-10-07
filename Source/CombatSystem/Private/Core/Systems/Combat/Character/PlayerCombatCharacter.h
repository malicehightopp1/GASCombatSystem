// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Systems/Combat/Character/CombatCharacter.h"
#include "PlayerCombatCharacter.generated.h"

class UPlayerHUDWidget;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class APlayerCombatCharacter : public ACombatCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCombatCharacter();
	
	//============================================//
	//                UI                          //
	//============================================//
	
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UPlayerHUDWidget> HUDWidgetClass;
	UPROPERTY() TObjectPtr<UPlayerHUDWidget> HUDWidget; 
	
protected:
	//============================================//
	//                Camera                      //
	//============================================//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera") TObjectPtr<UCameraComponent> FollowCamera;
	
	
	//============================================//
	//                Inputs                      //
	//============================================//
	
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> LookAction;
	
	//============================================//
	//                AttackingInput              //
	//============================================//
	
	UPROPERTY(EditDefaultsOnly, Category = "Input") TObjectPtr<UInputAction> LightAttackAction;
	
	void OnLightAttack();
};
