#include "ResourceManager.h"

#include "AssetCache.h"

ResourceManager::ResourceManager(size_t maxMemorySize)
	: m_MaxMemory(maxMemorySize)
{
}

template <typename AssetType>
TFuture<TSharedPtr<AssetType>> ResourceManager::LoadAssetAsync(const FString& assetPath, int priority)
{
	static_assert(TIsDerivedFrom<AssetType, UObject>::IsDerived, "AssetType must be derived from UObject");
	// Lock the cache mutex so we can safely access the cache
	m_cacheMutex.Lock();

	// Get the asset class from the asset type
	const TSubclassOf<UObject> AssetClass = AssetType::StaticClass();
	if (!m_AssetCaches.Contains(AssetClass))
	{
		m_AssetCaches.Add(AssetClass, MakeUnique<AssetCache<AssetType>>(m_MaxMemory));
	}

	TUniquePtr<AssetCacheBase>& Cache = m_AssetCaches[AssetClass];
	AssetCache<AssetType>* TypedCache = static_cast<AssetCache<AssetType>*>(Cache.Get());

	m_cacheMutex.Unlock();

	TFuture<TSharedPtr<AssetType>> AssetFuture = TypedCache->LoadAsset(assetPath);
	m_PriorityQueue.Insert(assetPath, priority);
	
	return AssetFuture;
}

template <typename AssetType>
void ResourceManager::UnloadAsset(const FString& assetPath)
{
	static_assert(TIsDerivedFrom<AssetType, UObject>::IsDerived, "AssetType must be derived from UObject");

	m_cacheMutex.Lock();

	TSubclassOf<UObject> AssetClass = AssetType::StaticClass();

	if (m_AssetCaches.Contains(AssetClass))
	{
		TUniquePtr<AssetCacheBase>& Cache = m_AssetCaches[AssetClass];
		Cache->UnloadAsset(assetPath);
		m_PriorityQueue.Remove(assetPath);
	}

	m_cacheMutex.Unlock();
}

void ResourceManager::Tick(float DeltaTime)
{
}
