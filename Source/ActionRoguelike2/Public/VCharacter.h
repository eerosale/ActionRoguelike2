// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "VCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ACTIONROGUELIKE2_API AVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVCharacter();

protected:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* defaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* moveForwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* TurnAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* moveRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	void MoveRight(const FInputActionValue& Value);

	void Turn(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void moveForward(const FInputActionValue& value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
