// Fill out your copyright notice in the Description page of Project Settings.

//x is forward (RED), y (GREEN) is right, z is up (BLUE)

#include "VCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AVCharacter::AVCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Create spring arm and attach to root (capsule)
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    // Create camera and attach to spring arm
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComp->SetupAttachment(SpringArmComp);
    CameraComp->bUsePawnControlRotation = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;

}

// Called when the game starts or when spawned
void AVCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (defaultMappingContext)
                {
                    Subsystem->AddMappingContext(defaultMappingContext, 0);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("DefaultMappingContext is NULL in BeginPlay"));
                }
            }
        }
    }
}

void AVCharacter::moveForward(const FInputActionValue& Value)
{
    float AxisValue = Value.Get<float>();

    FRotator ControlRot = GetControlRotation();
    ControlRot.Pitch = 0.0f;
    ControlRot.Roll = 0.0f;

    AddMovementInput(ControlRot.Vector(), AxisValue);
}

// Called every frame
void AVCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVCharacter::Turn(const FInputActionValue& Value)
{
    AddControllerYawInput(Value.Get<float>());
}

void AVCharacter::MoveRight(const FInputActionValue& Value)
{
    FRotator ControlRot = GetControlRotation();
    ControlRot.Pitch = 0.0f;
    ControlRot.Roll = 0.0f;

    FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

    AddMovementInput(RightVector, Value.Get<float>());
}

// Called to bind functionality to input
void AVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (moveForwardAction)
            EnhancedInput->BindAction(moveForwardAction, ETriggerEvent::Triggered, this, &AVCharacter::moveForward);
        else
            UE_LOG(LogTemp, Error, TEXT("MoveForwardAction is NULL!"));

        if (TurnAction)
            EnhancedInput->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AVCharacter::Turn);
        else
            UE_LOG(LogTemp, Error, TEXT("TurnAction is NULL!"));

        if (moveRightAction)
            EnhancedInput->BindAction(moveRightAction, ETriggerEvent::Triggered, this, &AVCharacter::MoveRight);
        else
            UE_LOG(LogTemp, Error, TEXT("moveRightAction is NULL!"));

        if (LookAction)
            EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVCharacter::Look);
        else
            UE_LOG(LogTemp, Error, TEXT("LookAction is NULL!"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast PlayerInputComponent to EnhancedInputComponent"));
    }
}

void AVCharacter::Look(const FInputActionValue& Value)
{
    AddControllerPitchInput(Value.Get<float>());
}

