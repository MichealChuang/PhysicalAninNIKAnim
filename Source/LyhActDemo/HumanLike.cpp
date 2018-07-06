// Fill out your copyright notice in the Description page of Project Settings.

#include "HumanLike.h"
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

// Sets default values
AHumanLike::AHumanLike()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void AHumanLike::AttackEnemy()
{
	if (bIsAttacking || GetMovementComponent()->IsFalling() || bIsDodging || bIsDefencing || bIsAttacked)
	{
		return;
	}
	bIsAttacking = true;
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("LeftButton"));
	bCanDamage = 0;
	InitialSwordState();
	switch (ComboNum)
	{
	case 0:
		if (Fast_One)
		{
			float Deruction = PlayAnimMontage(Fast_One);
			ComboNum++;
			GetWorldTimerManager().SetTimer(ComboHandle, this, &AHumanLike::OnAttackComplete, Deruction);
		}
		break;
	case 1:
		if (Fast_Two)
		{
			GetWorldTimerManager().ClearTimer(ComboHandle);
			float Deruction = PlayAnimMontage(Fast_Two);
			ComboNum++;
			GetWorldTimerManager().SetTimer(ComboHandle, this, &AHumanLike::OnAttackComplete, Deruction);
		}
		break;
	case 2:
		if (Fast_Three)
		{
			GetWorldTimerManager().ClearTimer(ComboHandle);
			PlayAnimMontage(Fast_Three);
			ComboNum = 0;
		}
		break;
	}

}

void AHumanLike::Dodge()
{
	if (bIsDefencing || GetMovementComponent()->IsFalling() || bIsDodging || Stats.Magic < 10)
	{
		return;
	}
	Stats.Magic -= 10;
	GetMesh()->Stop();
	if (bIsAttacking)
	{
		OnAttackComplete();
		bIsAttacking = false;
		bCanDamage = 0;
	}
	bIsAttacked = false;
	GetWorldTimerManager().ClearTimer(HurtHandle);
	bIsDodging = true;
	float Duration = 0;
	if (RightVextor > LeftVector)
	{
		if (Dodge_Right)
		{
			Duration = PlayAnimMontage(Dodge_Right);
		}
	}
	else if (RightVextor < LeftVector)
	{
		Duration = PlayAnimMontage(Dodge_Left);
	}
	else
	{
		Duration = PlayAnimMontage(Dodge_Behind);
	}
	GetWorldTimerManager().SetTimer(DodgeHandle, this, &AHumanLike::OnDodgeComplete, Duration - 0.5);
}

void AHumanLike::OnAttacked(FVector AttackPoint)
{
	if (bIsDodging)
	{
		return;
	}
	if (bIsDefencing)
	{
		if (Defence_Succeed)
		{
			PlayAnimMontage(Defence_Succeed);
		}
		Defence_End();
	}
	else
	{
		if (bIsAttacking)
		{
			bIsAttacking = false;
			bCanDamage = false;
		}
		bIsAttacked = true;
		GetWorldTimerManager().SetTimer(HurtHandle, this, &AHumanLike::OnHurtComplete, 1.5f);
		GetMesh()->Stop();
		FTransform CharacterTrans = GetActorTransform();
		FVector CharacterLocation = CharacterTrans.GetLocation();
		FVector Direction = UKismetMathLibrary::InverseTransformLocation(CharacterTrans, AttackPoint);;
		Stats.Blood -= 10;
		if (Stats.Blood <= 0)
		{
			Defeated();
		}
	}
}

void AHumanLike::OnAttackComplete()
{
	GetWorldTimerManager().ClearTimer(ComboHandle);
	ComboNum = 0;
}

void AHumanLike::OnDodgeComplete()
{
	bIsDodging = false;
	GetWorldTimerManager().ClearTimer(DodgeHandle);
}

void AHumanLike::Defence_Begin()
{
	if (bIsAttacking || GetMovementComponent()->IsFalling() || bIsDodging || bIsDefencing || bIsAttacked || Stats.Magic < 5)
	{
		return;
	}
	Stats.Magic -= 5;
	Stats.MagicRegain = 0;
	bIsDefencing = true;
	if (Defence_Start)
	{
		PlayAnimMontage(Defence_Start);
	}
	GetCharacterMovement()->MaxWalkSpeed = 200;
}

void AHumanLike::Defence_End()
{
	bIsDefencing = false;
	Stats.MagicRegain = 1;
	GetCharacterMovement()->MaxWalkSpeed = 600;
}

void AHumanLike::OnBounced()
{
	GetMesh()->Stop();
	bIsAttacking = false;
	//CustomTimeDilation = 0.35;
	if (Hit_Torso_Front)
	{
		PlayAnimMontage(Hit_Torso_Front);
	}
	OnAttackComplete();
}

void AHumanLike::OnHurtComplete()
{
	bIsAttacked = false;
	GetWorldTimerManager().ClearTimer(HurtHandle);
}



