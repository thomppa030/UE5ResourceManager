#pragma once

#include "CoreMinimal.h"
#include <mutex>

class AssetCacheBase
{
public:
	virtual ~AssetCacheBase() = default;

	virtual void UnloadAsset(FString assetPath) = 0;

	virtual void UpdateAssetPriority(FString assetPath, int newPriority) = 0;

	virtual void EvictAsset() = 0;

protected:
	std::mutex m_cacheMutex;
};
