#pragma once

#include "CoreMinimal.h"
#include "PriorityQueue.h"

class AssetCacheBase;

class ResourceManager
{
public:
	ResourceManager(size_t maxMemorySize);

	template<typename AssetType>
	UE_NODISCARD TFuture<TSharedPtr<AssetType>> LoadAssetAsync(const FString& assetPath, int priority = 0);

	template<typename AssetType>
	void UnloadAsset(const FString& assetPath);

	void Tick(float DeltaTime);

private:

	size_t m_MaxMemory;

	TMap<TSubclassOf<UObject>, TUniquePtr<AssetCacheBase>> m_AssetCaches;

	FPriorityQueue m_PriorityQueue;

	FCriticalSection m_cacheMutex;
};

