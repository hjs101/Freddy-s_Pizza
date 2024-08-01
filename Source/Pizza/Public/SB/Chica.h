#pragma once

#include "CoreMinimal.h"
#include "LocationState.h"
#include "GameFramework/Character.h"
#include "AILevel.h"
#include "Chica.generated.h"

UCLASS()
class PIZZA_API AChica : public ACharacter
{
	GENERATED_BODY()

public:
	AChica();

	void SetUpLocation(ELocationState State, float DeltaTime);
	void UpdateState(float DeltaTime);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	class UAILevel* AILevelComp;

	TArray<FVector> TagArr;
	int32 RoomNum = 1;

	// �� ��ġ �±� Ȯ��
	// FName GetCurrentLocationTag();
	
	void Idle(float DeltaTime);
	void Move();
	void Attack();
	void Cupcake();

private:
	UPROPERTY(EditDefaultsOnly)
	ELocationState CurrentState;

	// �� 4.98�ʸ��� �̵� ����
	float CurrentTime = 0.f;
	float MovableTime = 4.98f;

	FVector FindActorsWithTag(FName Tag);
	void MoveToTaggedLocation(int32 room);
	void CanMove();
	FTimerHandle Handle;

	// ��Ʈ�� Ű �Է� �Լ� -> ������ ON/OFF
	void FlashOn();
	// Shift Ű �Է� �Լ� -> ��/���� Open/Close
	void DoorOpen();
	bool bIsFlashlightOn = false;
	bool bIsDoorClose = false;



	//bool bIsAtDoor = false; // �׽�Ʈ�� ����

	UPROPERTY()
	class AFreddyPlayer* player;
};
