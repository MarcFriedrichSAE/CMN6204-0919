#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#pragma endregion

#pragma region project include
#include "SpawnPoint.generated.h"
#pragma endregion

#pragma region forward decleration
class USplineComponent;
#pragma endregion

UCLASS()
/// <summary>
/// spawn point class
/// </summary>
class WAVESHOOTERVR_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	ASpawnPoint();
#pragma endregion

#pragma region UPROPERTY
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Enemy")
	/// <summary>
	/// spline component
	/// </summary>
	USplineComponent* Spline = nullptr;
#pragma endregion
};