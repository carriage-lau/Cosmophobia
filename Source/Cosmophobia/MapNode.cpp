#include "MapNode.h"
#include "UObject/ConstructorHelpers.h"


AMapNode::AMapNode() {
    PrimaryActorTick.bCanEverTick = true;
    
    // Creates the mesh as a sphere -- but solely for debugging
    NodeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NodeMesh"));
        RootComponent = NodeMesh;

        static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
        if (SphereMesh.Succeeded()) {
            NodeMesh->SetStaticMesh(SphereMesh.Object);
            NodeMesh->SetRelativeScale3D(FVector(0.3f));
            NodeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

        NodeMesh->SetVisibility(true);
}

/** Runs when the player spawns. */
void AMapNode::BeginPlay() {
    Super::BeginPlay();
    
    SetActorLocation(GetActorLocation());

    // For redundancy's sake
    if (Position == FVector::ZeroVector) {
        Position = GetActorLocation();
    }
    
    if(NodeMesh){
        NodeMesh -> SetVisibility(false);
    }
}

/** Unnecessary, kept here purely beacuse I might use it in the future*/
void AMapNode::FillConnectedNodes(TArray<AMapNode *> ConnectedNodesList){
    this -> ConnectedNodes = ConnectedNodesList;
}

void AMapNode::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}
