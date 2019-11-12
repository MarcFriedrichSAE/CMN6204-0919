#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#pragma endregion

#pragma region project include
#include "SpawnPlayer.generated.h"
#pragma endregion

UCLASS()
/// <summary>
/// player spawn class
/// </summary>
class WAVESHOOTERVR_API ASpawnPlayer : public AActor
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	ASpawnPlayer();
#pragma endregion

#pragma region public inline function
	/// <summary>
	/// if spawn locations is allready in use
	/// </summary>
	/// <returns>spawn location is allready in use</returns>
	inline bool GetInUse() { return m_inUse; }

	/// <summary>
	/// set spawn location is allready in use
	/// </summary>
	/// <param name="_inUse">spawn location allready in use</param>
	inline void SetInUse(bool _inUse) { m_inUse = _inUse; }
#pragma endregion

private:
#pragma region private primitive variable
	/// <summary>
	/// spawn location is allready in use
	/// </summary>
	bool m_inUse = false;
#pragma endregion
};