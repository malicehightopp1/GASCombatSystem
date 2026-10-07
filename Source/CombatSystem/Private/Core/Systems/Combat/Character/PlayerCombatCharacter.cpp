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
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCombatCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCombatCharacter::Look);
		Input->BindAction(LightAttackAction, ETriggerEvent::Started, this, &APlayerCombatCharacter::OnLightAttack);
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

void APlayerCombatCharacter::OnLightAttack()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(CombatTags::Ability_Attack_Light.GetTag()));
}
