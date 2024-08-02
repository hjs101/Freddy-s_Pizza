// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocationState.h"
#include "GameFramework/Character.h"
#include "AILevel.h"
#include "Foxy.generated.h"

UCLASS()
class PIZZA_API AFoxy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFoxy();
	void SetUpLocation(ELocationState State, float DeltaTime);
	void UpdateState(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
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


private:
	UPROPERTY(EditDefaultsOnly)
	ELocationState CurrentState;

	// �� 5.01�ʸ��� �̵� ����
	float CurrentTime = 0.f;
	float MovableTime = 5.01f;

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

	bool bClosetAnim = false; // ���� �ִϸ��̼� �� ���� ����ǰ� �ϴ� �÷���
	bool bCTtoZero = false; // currentTime�� �� ���� 0�� �ǰ� �ϴ� �÷���

	//bool bIsAtDoor = false; // �׽�Ʈ�� ����
	void ShowFoxyDoll(AActor* actor, bool bShow);


	// �߼Ҹ�
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* FootStepsSFX;
	// ���Ҹ�
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* BreathSFX;
	// �������ɾ� �Ҹ�
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* JumpScareSFX;

	UPROPERTY()
	class AFreddyPlayer* player;
};
