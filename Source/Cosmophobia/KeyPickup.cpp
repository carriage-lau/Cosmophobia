// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MazeGenerator.h"

// Sets default values
AKeyPickup::AKeyPickup()
{
 	// perf
    PrimaryActorTick.bCanEverTick = false;

    // Create and configure collision sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->InitSphereRadius(50.f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Bind overlap event
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AKeyPickup::OnOverlapBegin);

    // Optional: add a visible key mesh
    KeyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("KeyMesh"));
    KeyMesh->SetupAttachment(RootComponent);
    KeyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AKeyPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKeyPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKeyPickup::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    // Only respond if the player overlaps
    if (OtherActor && OtherActor->IsA(APawn::StaticClass()))
    {
        // Find the maze generator in the world
        TArray<AActor*> FoundGenerators;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMazeGenerator::StaticClass(), FoundGenerators);

        if (FoundGenerators.Num() > 0)
        {
            // Assume the first one is the one you want
            AMazeGenerator* MazeGen = Cast<AMazeGenerator>(FoundGenerators[0]);
            if (MazeGen)
            {
                MazeGen->GenerateMaze();
            }
        }

        // Destroy
        Destroy();
    }
}