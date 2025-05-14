// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPrimitiveComponent;
struct FHitResult;

UCLASS()
class COSMOPHOBIA_API AKeyPickup : public AActor
{
    GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    // Collision volume
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    USphereComponent* CollisionSphere;

    // mesh so the key is visible in-game
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    UStaticMeshComponent* KeyMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
