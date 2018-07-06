// Fill out your copyright notice in the Description page of Project Settings.

#include "Sword.h"
#include "LyhActDemoCharacter.h"
#include "AICharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ConstructorHelpers.h"
#include "Engine/Engine.h"

// Sets default values
ASword::ASword()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Asset"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SwordAsset(TEXT("/Game/Mesh/Sword/Sword"));
	if (SwordAsset.Object)
	{
		Sword->SetStaticMesh(SwordAsset.Object);
	}
	Collision->SetRelativeScale3D(FVector(0.04f, 0.1f, 1.72f));
	Collision->SetRelativeLocation(FVector(0, 0, 40.5f));
	RootComponent = Root;
	Sword->SetupAttachment(Root);
	Collision->SetupAttachment(Sword);
}

// Called when the game starts or when spawned
void ASword::BeginPlay()
{
	Super::BeginPlay();

	InitialOwner();
}

//void ASword::BeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (LyhRef)
//	{
//		if (LyhRef->bCanDamage)
//		{
//			EnemyAI = Cast<AAICharacter>(OtherActor);
//			PCInitialHUDAI(EnemyAI);
//			TArray<TEnumAsByte<EObjectTypeQuery> > type;
//		}
//	}
//}
	

//void ASword::InitialOwner()
//{
//	if (Cast<ALyhActDemoCharacter>(GetParentActor()))
//	{
//		LyhRef = Cast<ALyhActDemoCharacter>(GetParentActor());
//	}
//	else if(Cast<AAICharacter>(GetParentActor()))
//	{
//		AIRef = Cast<AAICharacter>(GetParentActor());
//	}
//	else
//	{
//		return;
//	}
//}

FVector ASword::CheckHitLocation(const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, ACharacter* OwnerRef, ACharacter* EnemyRef, EDrawDebugTrace::Type DrawDebugType)
{
	FVector Start, End;
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Empty();
		
	Start = OwnerRef->GetActorLocation();
	//End = GetActorRotation().RotateVector(FVector(0, 500, 0)) + Start;
	End = OwnerRef->GetMesh()->GetSocketLocation(FName("hand_rSocket")) + UKismetMathLibrary::GetRightVector(GetActorRotation()) * 200;
	ActorsToIgnore.Add(OwnerRef);
	if (UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, ObjectTypes, false, ActorsToIgnore, DrawDebugType, OutHit, true))
	{
		bHasSucceed = true;
		GetClostBone(OutHit.Location, EnemyRef);
	}
	return OutHit.Location;
}

void ASword::GetClostBone(FVector HitLocation, ACharacter* OtherActor)
{
	BoneName = FName("");
	PreviousBoneDistance = 10000;
	USkeletalMeshComponent* Mesh = OtherActor->GetMesh();
	TArray<FName> AllBone = Mesh->GetAllSocketNames();
	for (FName BoneX : AllBone)
	{
		if ((HitLocation - Mesh->GetSocketLocation(BoneX)).Size() < PreviousBoneDistance)
		{
			BoneName = BoneX;
			PreviousBoneDistance = (HitLocation - Mesh->GetSocketLocation(BoneX)).Size();
		}
	}
}

