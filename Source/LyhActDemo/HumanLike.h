// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerStats.h"
#include "HumanLike.generated.h"

UCLASS()
class LYHACTDEMO_API AHumanLike : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHumanLike();

public:
	int32 ComboNum = 0;
	int8 LeftVector = 0;
	int8 RightVextor = 0;
	UPROPERTY(BlueprintReadWrite)
	FPlayerStats Stats;

	/***********************state********************/
	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsDodging = false;
	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsDefencing = false;
	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsDeath = false;
	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsAttacked = false;
	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsAttacking = false;
	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bCanDamage = 0;
	/***********************state********************/

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Fast_One;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Fast_Two;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Fast_Three;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Hit_Torso_Front;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Dodge_Left;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Dodge_Right;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Dodge_Behind;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Defence_Start;
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* Defence_Succeed;

	FTimerHandle ComboHandle;
	FTimerHandle DodgeHandle;
	FTimerHandle HurtHandle;
public:
	UFUNCTION(BlueprintCallable)
	void AttackEnemy();
	UFUNCTION(BlueprintCallable)
	void Dodge();
	UFUNCTION(BlueprintCallable)
	void OnAttacked(FVector AttackPoint);
	UFUNCTION()
	void OnAttackComplete();
	UFUNCTION()
	void OnDodgeComplete();
	UFUNCTION()
	void OnHurtComplete();
	UFUNCTION()
	void Defence_Begin();
	UFUNCTION()
	void Defence_End();
	UFUNCTION(BlueprintCallable)
	void OnBounced();
	//初始化剑的状态,比如是否已经检测到敌人
	UFUNCTION(BlueprintImplementableEvent)
	void InitialSwordState();
	//死亡后操作
	UFUNCTION(BlueprintImplementableEvent)
	void Defeated();
};
