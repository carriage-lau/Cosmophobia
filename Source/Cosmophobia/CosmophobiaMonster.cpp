// Fill out your copyright notice in the Description page of Project Settings.


#include "CosmophobiaMonster.h"
#include "CosmophobiaCharacter.h"
#include "MapNode.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h" // for mesh-based detection
#include "Engine/SkeletalMeshSocket.h" // For bone-based detection
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "AIController.h"
#include "TimerManager.h"
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

ACosmophobiaCharacter* ACosmophobiaMonster::GetPlayerCharacter() const
{
    // Return cached player if valid
    if (CachedPlayer && CachedPlayer->IsValidLowLevelFast())
    {
        return CachedPlayer;
    }

    // Cache is invalid, find player fresh
    CachedPlayer = nullptr;

    // Method 1: Preferred - Iterate through player controllers
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PlayerController = It->Get())
        {
            if (ACosmophobiaCharacter* PlayerChar = Cast<ACosmophobiaCharacter>(PlayerController->GetPawn()))
            {
                CachedPlayer = PlayerChar;
                return CachedPlayer;
            }
        }
    }
    return CachedPlayer;
}

//global variables
TArray<AMapNode*> NodesList;

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
    GetCapsuleComponent()->InitCapsuleSize(30.f, 80.f);
    GetCapsuleComponent()->SetCollisionProfileName("Pawn");
    GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    // 2. Configure mesh (already exists as GetMesh())
    GetMesh()->SetRelativeLocation(FVector(0, 0, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0)); // Fix forward rotation
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // Critical line
    GetMesh()->SetGenerateOverlapEvents(true);
    GetMesh()->SetNotifyRigidBodyCollision(true);
    GetMesh()->SetCollisionObjectType(ECC_Pawn); // Critical line

    // 3. Configure movement (using existing component)
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0, 500.f, 0);
    GetCharacterMovement()->MaxWalkSpeed = 750.0f;
    GetCharacterMovement()->SetWalkableFloorAngle(60.0f);
    GetCharacterMovement()->MaxStepHeight = 45.0f;
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = false;
    GetCharacterMovement()->SetPlaneConstraintEnabled(false); // Ensure no plane constraints

    GetMesh()->OnComponentHit.AddDynamic(this, &ACosmophobiaMonster::OnMonsterHit);

    // Initialize other variables
    DetectionRadius = 500.0f;
    state = "idle";
    offset = FVector::ZeroVector;
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

void ACosmophobiaMonster::HandleState()
{
    if (ACosmophobiaCharacter* PlayerChar = GetPlayerCharacter())
    {
        FVector TraceStart = GetActorLocation() + FVector(0, 0, 90); // Eye level
        FVector TraceEnd = PlayerChar->GetActorLocation() + FVector(0, 0, 90);
        float DistanceToPlayer = FVector::Distance(TraceStart, TraceEnd);

        FCollisionQueryParams TraceParams;
        TraceParams.AddIgnoredActor(this);
        TraceParams.bTraceComplex = true;
        TraceParams.bReturnPhysicalMaterial = false;

        bool bHasDirectLOS = false;

        TArray<FVector> TracePoints = {
            FVector(0,0,90),    // Eye level
            FVector(0,0,50),    // Chest level
            FVector(0,0,20)     // Foot level
        };

        for (const FVector& Offset : TracePoints)
        {
            FVector Start = GetActorLocation() + Offset;
            FVector End = PlayerChar->GetActorLocation() + Offset;

            FHitResult Hit;
            if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams))
            {
                bHasDirectLOS = true;
                // DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.5f);
                break;
            }
            else
            {
                // DrawDebugLine(GetWorld(), Start, Hit.Location, FColor::Red, false, 0.5f);
                // DrawDebugSphere(GetWorld(), Hit.Location, 10.f, 8, FColor::Red, false, 0.5f);
                // UE_LOG(LogTemp, Warning, TEXT("Blocked by: %s"), *Hit.GetActor()->GetName());
            }
        }

        static float LastLOSTime = 0.f;
        if (bHasDirectLOS)
        {
            LastLOSTime = GetWorld()->GetTimeSeconds();
            state = "direct chase";
        }
        else if (GetWorld()->GetTimeSeconds() - LastLOSTime < 2.0f) // Grace period
        {
            state = "direct chase";
        }
        else
        {
            state = (DistanceToPlayer <= DetectionRadius) ? "node chase" : "idle"; // broken rn
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
        if (FVector::Dist(GetCapsuleComponent()->GetComponentLocation(), TargetLocation) <= offset.Size() + 50.0f) {
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
    ACosmophobiaCharacter* PlayerChar = GetPlayerCharacter();
    if (!PlayerChar || !TargetNode) return;
    FVector TargetLocation = TargetNode->GetActorLocation();

    // Find closest node to player
    AMapNode* PlayerNode = NodesList[0];
    for (AMapNode* Node : NodesList) {
        float NewDist = FVector::Dist(Node->GetActorLocation(), PlayerChar->GetActorLocation());
        if (FVector::Dist(PlayerNode->GetActorLocation(), PlayerChar->GetActorLocation()) > NewDist) {
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
void ACosmophobiaMonster::DirectChaseState(float DeltaTime)
{
    if (ACosmophobiaCharacter* PlayerChar = GetPlayerCharacter()) {
        FVector PlayerLocation = PlayerChar->GetActorLocation();
        FVector Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Direction, VelocityMultiplier);
    }
}

//starts to traverse the graph
void ACosmophobiaMonster::BeginPlay()
{
    Super::BeginPlay();

    GetPlayerCharacter();

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            PlayerCheckTimerHandle,
            this,
            &ACosmophobiaMonster::ValidatePlayerReference,
            1.0f,  // Check every second
            true   // Loop
        );
    }

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

void ACosmophobiaMonster::ValidatePlayerReference()
{
    if (!IsValid(CachedPlayer))
    {
        CachedPlayer = nullptr;
        GetPlayerCharacter(); // Refresh the reference
    }
}

void ACosmophobiaMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Clean up timer
    if (PlayerCheckTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(PlayerCheckTimerHandle);
    }
}


void ACosmophobiaMonster::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    // Handle state first
    HandleState();

    // Then process the current state
    if (state == "idle") {
        UE_LOG(LogTemp, Warning, TEXT("Idling"));
        IdleState(DeltaTime);
    }
    else if (state == "node chase") {
        if (!TargetNode) {
            UE_LOG(LogTemp, Warning, TEXT("No Target"));
            return;
        }
        UE_LOG(LogTemp, Warning, TEXT("Node Chasing"));
        NodeChaseState(DeltaTime);
    }
    else if (state == "direct chase") {
        UE_LOG(LogTemp, Warning, TEXT("Direct Chasing"));
        DirectChaseState(DeltaTime);
    }
}

void ACosmophobiaMonster::OnMonsterHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherComp || !OtherActor || OtherActor == this) return;

    if (ACosmophobiaCharacter* PlayerChar = Cast<ACosmophobiaCharacter>(OtherActor))
    {
        // Debug which component was hit
        FString HitComponentName = OtherComp ? OtherComp->GetName() : "NULL";
        UE_LOG(LogTemp, Warning, TEXT("Hit detected on component: %s"), *HitComponentName);

        if (OtherComp) {
            UE_LOG(LogTemp, Warning, TEXT("Hit component: %s"), *OtherComp->GetName());
            UE_LOG(LogTemp, Warning, TEXT("Component tags:"));
            for (const FName& Tag : OtherComp->ComponentTags) {
                UE_LOG(LogTemp, Warning, TEXT("- %s"), *Tag.ToString());
            }
        }

        // Tag-based detection (most reliable)
        if (OtherComp->ComponentTags.Contains("Head"))
        {
            PlayerChar->DamageHandler(EDamageType::Head);
            UE_LOG(LogTemp, Warning, TEXT("HEAD SHOT!"));
        }
        else if (OtherComp->ComponentTags.Contains("Torso"))
        {
            PlayerChar->DamageHandler(EDamageType::Torso);
            UE_LOG(LogTemp, Warning, TEXT("TORSO HIT!"));
        }
        else if (OtherComp->ComponentTags.Contains("Arm")) {
            PlayerChar->DamageHandler(EDamageType::Arm);
            UE_LOG(LogTemp, Warning, TEXT("ARM HIT!"));
        }
        else if (OtherComp->ComponentTags.Contains("Leg")) {
            PlayerChar->DamageHandler(EDamageType::Leg);
            UE_LOG(LogTemp, Warning, TEXT("LEG HIT!"));
        }
    }
}