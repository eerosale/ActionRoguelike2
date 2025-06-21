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
#include "DrawDebugHelpers.h"


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

    // -- Rotation Visualization -- //
    const float DrawScale = 100.0f;
    const float Thickness = 5.0f;

    FVector LineStart = GetActorLocation();
    // Offset to the right of pawn
    LineStart += GetActorRightVector() * 100.0f;
    // Set line end in direction of the actor's forward
    FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
    // Draw Actor's Direction
    DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

    FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
    // Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
    DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);

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

void AVCharacter::FireProjectile()
{
    if (!ProjectileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is NULL! Cannot fire projectile."));
        return;
    }

    // Offset forward from the character's eyes or chest
    FVector MuzzleOffset = FVector(100.0f, 0.0f, 50.0f); // forward + upward
    FVector MuzzleLocation = GetActorLocation() + GetControlRotation().RotateVector(MuzzleOffset);

    // Use the control rotation to shoot forward
    FRotator MuzzleRotation = GetControlRotation();

    // Setup spawn parameters
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    // Spawn the projectile
    AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

    if (SpawnedProjectile)
    {
        UE_LOG(LogTemp, Log, TEXT("Projectile spawned successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn projectile."));
    }
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

        if (FireProjectileAction)
            EnhancedInput->BindAction(FireProjectileAction, ETriggerEvent::Started, this, &AVCharacter::FireProjectile);
        else
            UE_LOG(LogTemp, Error, TEXT("FireProjectileAction is NULL!"));
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

