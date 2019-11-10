#pragma once

#pragma region UE4 include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#pragma endregion

#pragma region project include
#include "Chracter/Base/BaseCharacter.h"
#include "Friend.generated.h"
#pragma endregion

#pragma region forward decleration
class UCapsuleComponent;
#pragma endregion

UCLASS()
/// <summary>
/// friend class
/// </summary>
class WAVESHOOTERVR_API AFriend : public ABaseCharacter
{
	GENERATED_BODY()
	
public:	
#pragma region constructor
	/// <summary>
	/// constructor
	/// </summary>
	AFriend();
#pragma endregion

#pragma region public override function
	/// <summary>
	/// called after actor was destroyed
	/// </summary>
	virtual void Destroyed() override;
#pragma endregion
};