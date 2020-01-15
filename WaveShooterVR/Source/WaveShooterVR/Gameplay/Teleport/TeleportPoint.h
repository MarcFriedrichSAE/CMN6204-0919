#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#pragma endregion

#pragma region project include
#include "TeleportPoint.generated.h"
#pragma endregion

UCLASS()
/// <summary>
/// teleport point class
/// </summary>
class WAVESHOOTERVR_API ATeleportPoint : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	ATeleportPoint();
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, VisibleDefaultsOnly, Category = "Teleport")
	/// <summary>
	/// static mesh component
	/// </summary>
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(Replicated)
	/// <summary>
	/// if teleport is in use
	/// </summary>
	bool InUse = false;
#pragma endregion
};