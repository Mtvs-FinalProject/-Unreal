// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTP_Actor.h"

// Sets default values
AHTTP_Actor::AHTTP_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHTTP_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHTTP_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

