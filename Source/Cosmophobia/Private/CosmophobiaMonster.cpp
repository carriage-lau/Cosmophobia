// Fill out your copyright notice in the Description page of Project Settings.


#include "CosmophobiaMonster.h"
#include "../CosmophobiaCharacter.h"
#include "MapNode.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "AIController.h"
#include <random>
#include <queue>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <ctime>
#include <chrono>
#include <Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h>

//helper random number generator
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int get(int l, int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

//global variables
TArray<AMapNode*> NodesList;
ACosmophobiaCharacter* Player = nullptr;

void ACosmophobiaMonster::PopulateNodesList() {
    NodesList.Empty();

    for (TActorIterator<AMapNode> It(GetWorld()); It; ++It) {
        NodesList.Add(*It);
    }
}

// Sets default values
ACosmophobiaMonster::ACosmophobiaMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    TargetNode = nullptr;

    //test params
    DetectionRadius = 500.0f;
    state = "idle";
    offset = FVector::ZeroVector;
    GetCharacterMovement()->MaxWalkSpeed = 750.0f;
    GetCharacterMovement()->SetWalkableFloorAngle(60.0f); // Correct function
    GetCharacterMovement()->MaxStepHeight = 45.0f;

    // Create and attach the Sphere Component
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetupAttachment(GetCapsuleComponent());

    // Set default radius for the sphere
    SphereComponent->InitSphereRadius(50.0f); // Adjust the radius as needed

    // Optional: Enable collision
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SphereComponent->SetCollisionResponseToAllChannels(ECR_Block);
}

bool ACosmophobiaMonster::CheckForWall() {
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * 100;
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams)) {
        return true;
    }
    return false;
}

TArray<AMapNode*> ACosmophobiaMonster::SelectShortestPath(AMapNode* start, AMapNode* target) {
    using namespace std;

    // Priority queue to store nodes based on cost
    priority_queue<FNodeCost, vector<FNodeCost>, greater<FNodeCost>> PriorityQueue;

    // Maps to store optimal costs and previous nodes
    TMap<AMapNode*, float> OptimalCosts;
    TMap<AMapNode*, AMapNode*> PriorNode;

    // Initialize start node
    OptimalCosts[start] = 0.0f;
    PriorityQueue.push(FNodeCost{ start, 0.0f });

    while (!PriorityQueue.empty()) {
        // Get the node with the lowest cost
        AMapNode* Current = PriorityQueue.top().Node; // Use a different name for the local variable
        float CurrentCost = PriorityQueue.top().Cost;
        PriorityQueue.pop();

        // If we reach the target, reconstruct the path
        if (Current == target) {
            TArray<AMapNode*> Path;
            AMapNode* PathNode = target;

            while (PathNode != start) {
                Path.Insert(PathNode, 0); // Insert at the beginning
                PathNode = PriorNode[PathNode];
            }
            Path.Insert(start, 0); // Add the start node
            return Path;
        }

        // Explore connected nodes
        for (AMapNode* ConnectedNode : Current->ConnectedNodes) {
            float EdgeCost = FVector::Dist(Current->GetActorLocation(), ConnectedNode->GetActorLocation());
            float NetCost = CurrentCost + EdgeCost;

            // Update cost if a better path is found
            if (!OptimalCosts.Contains(ConnectedNode) || NetCost < OptimalCosts[ConnectedNode]) {
                OptimalCosts[ConnectedNode] = NetCost;
                PriorNode[ConnectedNode] = Current;
                PriorityQueue.push(FNodeCost{ ConnectedNode, NetCost });
            }
        }
    }

    // If no path is found, return an empty array
    return TArray<AMapNode*>();
}

void ACosmophobiaMonster::TraverseGraph() {
    if (CurrentNode && CurrentNode->ConnectedNodes.Num() > 0) {
        int32 RandomNode = FMath::RandRange(0, CurrentNode->ConnectedNodes.Num() - 1);
        TargetNode = CurrentNode->ConnectedNodes[RandomNode];
    }
}

void ACosmophobiaMonster::HandleState() {
    if (!Player) return;

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

    if (DistanceToPlayer > DetectionRadius) {
        state = "idle";
    }
    else {
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            GetActorLocation(),
            Player->GetActorLocation(),
            ECC_Visibility,
            QueryParams
        );

        if (!bHit) {
            state = "direct chase";
        }
        else {
            state = "node chase";
        }
    }
}

FVector ACosmophobiaMonster::Move(AMapNode* Target, float DeltaTime, float Offset) {
    if (!Target) return FVector::ZeroVector;

    FVector TargetLocation = Target->GetActorLocation() + FVector(Offset, Offset, 0);
    FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Direction, VelocityMultiplier);
    return GetActorLocation();
}

// Idle state code
void ACosmophobiaMonster::IdleState(float DeltaTime)
{
    if (TargetNode) {
        FVector TargetLocation = TargetNode->GetActorLocation();
        if (FVector::Dist(SphereComponent->GetComponentLocation(), TargetLocation) <= offset.Size() + 50.0f) {
            CurrentNode = TargetNode;
            GetWorld()->GetTimerManager().SetTimer(
                TraverseTimerHandle,
                this,
                &ACosmophobiaMonster::TraverseGraph,
                3.0f,
                false
            );
            offset = FMath::VRand() * (get(0, 200) * 1.0f);
        }
        Move(TargetNode, DeltaTime);
    }
    else {
        AddMovementInput(FMath::VRand(), VelocityMultiplier);
    }
}

// Node chase state code
void ACosmophobiaMonster::NodeChaseState(float DeltaTime) {
    FVector TargetLocation = TargetNode->GetActorLocation();
    // gets the closest node to the player
    AMapNode* PlayerNode = NodesList[0];
    for (AMapNode* Node : NodesList) {
        float NewDist = FVector::Dist(Node->GetActorLocation(), Player->GetActorLocation());
        if (FVector::Dist(PlayerNode->GetActorLocation(), Player->GetActorLocation()) > NewDist) {
            PlayerNode = Node;
        }
    }

    // performs a shortest path algorithm.
    TArray<AMapNode*> Path = SelectShortestPath(TargetNode, PlayerNode);
    if (FVector::Dist(GetActorLocation(), TargetLocation) <= offset.Size() + 50.0) {
        if (Path.Num() > 0) {
            TargetNode = Path[0];
            Path.RemoveAt(0);
            offset = FMath::VRand() * (get(0, 200) * 1.0f);
        }
        else {
            state = "direct chase";
            return;
        }
    }
    Move(TargetNode, DeltaTime);
}

// Direct chase state code
void ACosmophobiaMonster::DirectChaseState(float DeltaTime) {
    if (!Player) return;

    FVector PlayerLocation = Player->GetActorLocation();
    FVector Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Direction, VelocityMultiplier);
}

//starts to traverse the graph
void ACosmophobiaMonster::BeginPlay()
{
    Super::BeginPlay();

    // Initialize Player here, where GetWorld() is valid
    Player = Cast<ACosmophobiaCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    PopulateNodesList();

    // Find the closest node to the monster's starting location
    if (NodesList.Num() > 0) {
        TargetNode = NodesList[0];
        for (AMapNode* Node : NodesList) {
            if (FVector::Dist(Node->GetActorLocation(), GetActorLocation()) < FVector::Dist(TargetNode->GetActorLocation(), GetActorLocation())) {
                TargetNode = Node;
            }
        }
    }

    if (Controller == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Spawning AI Controller for Monster..."));
        AAIController* AIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass());
        if (AIController)
        {
            AIController->Possess(this);
            UE_LOG(LogTemp, Warning, TEXT("AI Controller successfully possessed the Monster."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn AI Controller!"));
        }
    }
}


void ACosmophobiaMonster::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // State handling
    if (state != "direct chase") {
        if (state == "idle") {
            UE_LOG(LogTemp, Warning, TEXT("Idling"));
            IdleState(DeltaTime);
        }
        else if (state == "node chase") {
            if (!TargetNode) {
                UE_LOG(LogTemp, Warning, TEXT("No Target"));
                return; // Fix: Use TargetNode instead of Target
            }
            UE_LOG(LogTemp, Warning, TEXT("Node Chasing"));
            NodeChaseState(DeltaTime);
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("Direct Chasing"));
        DirectChaseState(DeltaTime);
    }

    HandleState();
}

