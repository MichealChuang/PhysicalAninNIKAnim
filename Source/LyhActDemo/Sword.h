// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sword.generated.h"

UCLASS()
class LYHACTDEMO_API ASword : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASword();
	UPROPERTY(EditAnywhere, Category = Common, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Common, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Sword;
	UPROPERTY(EditAnywhere, Category = Common, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Collision;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	UFUNCTION(BlueprintImplementableEvent)
	void InitialOwner();
	UFUNCTION(BlueprintCallable)
	FVector CheckHitLocation(const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, ACharacter* OwnerRef, ACharacter* EnemyRef, EDrawDebugTrace::Type DrawDebugType);
	UFUNCTION(BlueprintCallable)
	void GetClostBone(FVector HitLocation, ACharacter* OtherActor);
public:
	UPROPERTY(BlueprintReadWrite)
	bool bHasSucceed = false;

	UPROPERTY(BlueprintReadOnly)
	FName BoneName;
	float PreviousBoneDistance;
};
