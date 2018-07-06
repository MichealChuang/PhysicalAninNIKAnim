// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "LyhActDemoCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "ConstructorHelpers.h"

ALyhActDemoCharacter::ALyhActDemoCharacter()
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	static ConstructorHelpers::FObjectFinder<UDataTable> StateObject(TEXT("/Game/Common/stats"));
	if (StateObject.Object)
	{
		Stats = *StateObject.Object->FindRow<FPlayerStats>(TEXT("Player"), TEXT(""));
	}
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void ALyhActDemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ALyhActDemoCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALyhActDemoCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALyhActDemoCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALyhActDemoCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALyhActDemoCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ALyhActDemoCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ALyhActDemoCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ALyhActDemoCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ALyhActDemoCharacter::AttackEnemy);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ALyhActDemoCharacter::Dodge);
	PlayerInputComponent->BindAction("Defence", IE_Pressed, this, &ALyhActDemoCharacter::Defence_Begin);
	PlayerInputComponent->BindAction("Defence", IE_Released, this, &ALyhActDemoCharacter::Defence_End);
}

void ALyhActDemoCharacter::BeginPlay()
{
	Super::BeginPlay();
	MaxMagic = Stats.Magic;
	if (MaxMagic != 0)
	{
		GetWorldTimerManager().SetTimer(RegainHandle, this, &ALyhActDemoCharacter::RegainMagic, 1, true);
	}
}

void ALyhActDemoCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorldTimerManager().ClearTimer(RegainHandle);
}

void ALyhActDemoCharacter::Jump()
{
	if (!bIsAttacking && !bIsAttacked && !bIsDodging && !bIsDefencing)
	{
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
	}
}

void ALyhActDemoCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ALyhActDemoCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ALyhActDemoCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ALyhActDemoCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALyhActDemoCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALyhActDemoCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ALyhActDemoCharacter::MoveRight(float Value)
{
	if (Value == 0)
	{
		LeftVector = 0;
		RightVextor = 0;
	}
	else if (Value > 0)
	{
		RightVextor = 1;
		LeftVector = 0;
	}
	else
	{
		RightVextor = 0;
		LeftVector = 1;
	}
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ALyhActDemoCharacter::RegainMagic()
{
	if (Stats.Magic < MaxMagic)
	{
		Stats.Magic += Stats.MagicRegain;
	}
}

AHumanLike* ALyhActDemoCharacter::CheckAI(float RotationRate, float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	TArray<FHitResult> OutHits;
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0, 0, 15);

	UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, 1000, ObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
	if (OutHits.Num() > 0)
	{
		for (TArray<FHitResult>::TIterator It = OutHits.CreateIterator(); It; ++It)
		{
			if (FMath::Abs(UKismetMathLibrary::FindLookAtRotation(Start, It->GetActor()->GetActorLocation()).Yaw - GetActorRotation().Yaw) >= RotationRate)
			{
				continue;
			}
			//检测与玩家之间有没有障碍物
			UKismetSystemLibrary::LineTraceSingle(this, Start, It->GetActor()->GetActorLocation(), ETraceTypeQuery::TraceTypeQuery2, 0, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
			if (AHumanLike* Target = Cast<AHumanLike>(OutHit.GetActor()))
			{
				return Target;
			}
		}
	}
	return nullptr;
}
