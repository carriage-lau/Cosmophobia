#include "MapNode.h" //nobody cares, you can ignore


AMapNode::AMapNode() {
    PrimaryActorTick.bCanEverTick = true;
}

//runs when player spawns
void AMapNode::BeginPlay() {
    Super::BeginPlay();

    //sets the position of a node if not set already
    if (position == FVector::ZeroVector) {
        position = GetActorLocation();
    }

    // TArray<FHitResult> HitResults;
    // FVector NodeLocation = position;
    // float searchRadius = 800.0f;

    // FCollisionShape Sphere = FCollisionShape::MakeSphere(searchRadius); // generates sphere of radius searchRadius
    // bool bHasHit = GetWorld().SweepMultiByChannel(
    //  HitResults,
    //     NodeLocation,
    //     NodeLocation,
    //     FQuat::Identity,
    //     ECC_WorldStatic,
    //     Sphere
    // ); //returns all hits within a 500m sweep around the node

    // //adds the nodes inside the list of connected nodes. must check if node is self (monster cannot move to self)
    // for(auto& Hit : HitResults){
    //  AGraphNode* HitNode = Cast<AGraphNode>(Hit.GetActor());
    //  if(HitNode != this){
    //      ConnectedNodes.Add(HitNode);
    //  }
    // }
}

void AMapNode::FillConnectedNodes(TArray<AMapNode *> ConnectedNodesList){
    this -> ConnectedNodes = ConnectedNodesList;
}

void AMapNode::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
}
