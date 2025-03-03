#pragma once

#include "CoreMinimal.h"
#include "DamageTypes.generated.h"

UENUM()
enum class EDamageType : uint8
{
    Torso,
    Arm,
    Leg
};