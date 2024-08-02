// Fill out your copyright notice in the Description page of Project Settings.


#include "JYS/EnemyBonnie.h"
#include "HJS/FreddyPlayer.h"
#include "EngineUtils.h"

// Sets default values
AEnemyBonnie::AEnemyBonnie()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Room0 
	RoomPositions[0] = FVector(-320.0f, 810.0f, 0.0f);
	// Room1
	RoomPositions[1] = FVector(-320.0f, 360.0f, 0.0f);
	// Room2
	RoomPositions[2] = FVector(-320.0f, -220.0f, 0.0f);
	// Room3
	RoomPositions[3] = FVector(200.0f, -280.0f, 0.0f);

	State = EBonnieState::Room1;

	Player = nullptr; // Initialize Player to nullptr
	TargetLocation = RoomPositions[0]; // Initialize TargetLocation
	// �̵� �ӵ� �ʱ�ȭ
	MoveSpeed = 500.0f;
	// �̵� �� ���� �ʱ�ȭ
	bIsMovingToRoom3 = false;
}

// Called when the game starts or when spawned
void AEnemyBonnie::BeginPlay()
{
	Super::BeginPlay();
	
	SetAILevel(7);

	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &AEnemyBonnie::AttemptMove, 4.97f, true);
}

// Called every frame
void AEnemyBonnie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Room1���� Room3�� �̵��ϱ�
	if (ShouldMoveToRoom3())
	{
		Move(EBonnieState::Room3);
	}
	
	// Room0���� Room2�� �̵�
	if (CloseDoorRoom0ToRoom2())
	{
		Move(EBonnieState::Room2);
	}

	if (bIsMovingToRoom3)
	{
		// ���� ��ġ�� ��ǥ ��ġ�� �����Ͽ� �̵�
		FVector Dir = TargetLocation - GetActorLocation();
		Dir.Normalize();
		FVector P0 = GetActorLocation();
		SetActorLocation(P0 + Dir * DeltaTime * MoveSpeed);

		// ��ǥ ��ġ�� �����ϸ� �̵� �� ���� ����
		if (FVector::Dist(GetActorLocation(), TargetLocation) < 1.0f)
		{
			bIsMovingToRoom3 = false;
		}
	}

	switch (State)
	{
		case EBonnieState::Room0:	
			TickRoom0(DeltaTime);	
			break;
		case EBonnieState::Room1:	
			TickRoom1(DeltaTime);	
			break;
		case EBonnieState::Room2:	
			TickRoom2(DeltaTime);	
			break;
		case EBonnieState::Room3:	
			TickRoom3(DeltaTime);	
			break;
	}
}

void AEnemyBonnie::Move(EBonnieState MoveState)
{
	if (State == EBonnieState::Room1 && MoveState == EBonnieState::Room3)
	{
		UE_LOG(LogTemp,Warning,TEXT("1234"));
		// 1���濡�� 3�������� �̵��� �� �̵�	
		bIsMovingToRoom3 = true;
		TargetLocation = RoomPositions[static_cast<int32>(MoveState)];
	}
	else
	{
		// �ٸ� ������ �̵��� ���� �����̵�
		bIsMovingToRoom3 = false;
		int32 Index = static_cast<int32>(MoveState);
		if (Index >= 0 && Index < 4)
		{
			State = MoveState;
			SetActorLocation(RoomPositions[Index]);
		}
	}
}

void AEnemyBonnie::TickRoom0(const float& DeltaTime)
{
	if (ShouldMoveToRoom3())
	{
		JumpScareBonnie();
	}
}

void AEnemyBonnie::TickRoom1(const float& DeltaTime)
{
}

void AEnemyBonnie::TickRoom2(const float& DeltaTime)
{
}

void AEnemyBonnie::TickRoom3(const float& DeltaTime)
{
}

void AEnemyBonnie::AttemptMove()
{
	// ���� 1~20 ���� �̱�
	int32 RandomNumber = GetRandomNumber();

	// ���� �ѹ��� ������ �������� ���� �� �̵�
	if (RandomNumber < Level)
	{
		EBonnieState NewState = State;
		
		switch (State)
		{
			// Room0�� Room1���θ� �̵��� �� �ִ�
			case EBonnieState::Room0:
				NewState = EBonnieState::Room1;
				break;
			// Room1�� Room0, Room2, Room3�� �̵��� �� �ִ�
			case EBonnieState::Room1:
			{
				int32 MoveChoice = FMath::RandRange(0, 2);
				switch (MoveChoice)
				{
				case 0:
					NewState = EBonnieState::Room0;
					break;
				case 1:
					NewState = EBonnieState::Room2;
					break;
				case 2:
					NewState = EBonnieState::Room3;
					break;
				}
			}	
				break;
			// Room2�� Room1�̶� Room3���� �̵��� �� �ִ�
			case EBonnieState::Room2:
				NewState = (FMath::RandBool()) ? EBonnieState::Room1 : EBonnieState::Room3;
				break;
			// Room3�� Room2�θ� �̵��� �� �ִ�
			case EBonnieState::Room3:
				NewState = EBonnieState::Room2;
				break;
		}
		Move(NewState);
	}
}

int32 AEnemyBonnie::GetRandomNumber()
{
	return FMath::RandRange(1, 20);
}

bool AEnemyBonnie::ShouldMoveToRoom3()
{
	// �÷��̾ ���ʿ��� Flash�� ���߰� Bonnie�� 1���濡 ���� ��
	if (Player)
	{
		return Player->GetFlash() && Player ->GetLookAtState() == AFreddyPlayer::LookAt::Left && State == EBonnieState::Room1;
	}
	return false;      
}

void AEnemyBonnie::JumpScareBonnie()
{
	FVector CameraLoc = Player->GetCameraTransform().GetLocation();
	CameraLoc.X += 100;
	SetActorLocation(CameraLoc);
}

bool AEnemyBonnie::CloseDoorRoom0ToRoom2()
{
	if (Player)
	{
		return Player->GetrCloseDoor() && Player->GetLookAtState() == AFreddyPlayer::LookAt::Left && State == EBonnieState::Room0;
	}
	return false;
}
