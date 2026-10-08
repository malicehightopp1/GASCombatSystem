// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Systems/Combat/Character/PlayerCombatCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Core/Systems/Combat/Tags/CombatGameplayTags.h"
#include "Core/Systems/UI/PlayerUI/PlayerHUDWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


APlayerCombatCharacter::APlayerCombatCharacter()
{
	//rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	
	//camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->TargetOffset = FVector(0.0f, 0.0f, 50.0f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bUseCameraLagSubstepping = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	
	//camera setup
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

void APlayerCombatCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				UE_LOG(LogTemp, Warning, TEXT("Mapping context was added"));
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
		
		//Creating the widget
		if (!HUDWidget && HUDWidgetClass && PC->IsLocalController())
		{
			HUDWidget = CreateWidget<UPlayerHUDWidget>(PC, HUDWidgetClass);
			UE_LOG(LogTemp, Warning, TEXT("HUDWidget was created"));
			if (HUDWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("HUDWidget added to viewport"));
				HUDWidget->InitFromAbilitySystemComponent(GetAbilitySystemComponent());
				HUDWidget->AddToViewport();
			}
		}
	}
}

void APlayerCombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Locomotion
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCombatCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCombatCharacter::Look);
		
		//Attacks
		Input->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnLightAttack);
		Input->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnHeavyAttack);
		Input->BindAction(Special1Action, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnSpecial1);
		Input->BindAction(Special2Action, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnSpecial2);
		Input->BindAction(Special3Action, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnSpecial3);
	}
}

void APlayerCombatCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	if (!Controller) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Input.Y);
	AddMovementInput(Right, Input.X);
}

void APlayerCombatCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

void APlayerCombatCharacter::ActivateAbilityByTag(const FGameplayTag& AbilityTag)
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ability system Component is NULL"))
		return;
	}
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));
	UE_LOG(LogTemp, Warning, TEXT("Ability was called"))
}

void APlayerCombatCharacter::OnLightAttack()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Light.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Light attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnHeavyAttack()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Heavy.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Heavy attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnSpecial1()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Special1.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Special 1 attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnSpecial2()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Special2.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Special 2 attack called Ability should of been called"))
}

void APlayerCombatCharacter::OnSpecial3()
{
	ActivateAbilityByTag(CombatTags::Ability_Attack_Special3.GetTag());
	UE_LOG(LogTemp, Warning, TEXT("Special 3 attack called Ability should of been called"))
}
