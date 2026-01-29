#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class ENGINE_DLL CRunTimeBucket 
{
public:
	
public:
	template<typename Type>
	class TPersistStore
	{
	public:
		void Set(PersistScope scope, const string& key, const Type& value)
		{
			store[ENUM(scope)][key] = value;
		}

		_bool TryGet(PersistScope scope, const string& key, Type& outValue) const
		{
			const auto& bucket = store[ENUM(scope)];
			auto iter = bucket.find(key);
			if (iter == bucket.end())
				return false;

			outValue = iter->second;
			return true;
		}

		bool Remove(PersistScope scope, const string& key)
		{
			auto& bucket = store[ENUM(scope)];
			auto iter = bucket.find(key);
			if (iter == bucket.end())
				return false;

			bucket.erase(iter);
			return true;
		}

		void ClearScope(PersistScope scope)
		{
			store[ENUM(scope)].clear();
		}

		bool Has(PersistScope scope, const string& key) const
		{
			const auto& bucket = store[ENUM(scope)];
			return bucket.find(key) != bucket.end();
		}

	public:
		class ENGINE_DLL ScopedOverride
		{
		public:
			ScopedOverride(TPersistStore& owner, PersistScope scope, string key, const Type& tempValue)
				: ownerStore(owner), scopeValue(scope), keyValue(move(key))
			{
				hadPrev = ownerStore.TryGet(scopeValue, keyValue, prevValue);
				ownerStore.Set(scopeValue, keyValue, tempValue);
			}

			~ScopedOverride()
			{
				if (hadPrev) ownerStore.Set(scopeValue, keyValue, prevValue);
				else ownerStore.Remove(scopeValue, keyValue);
			}

			ScopedOverride(const ScopedOverride&) = delete;
			ScopedOverride& operator=(const ScopedOverride&) = delete;

		private:
			TPersistStore& ownerStore;
			PersistScope scopeValue;
			string keyValue;
			_bool hadPrev = false;
			Type prevValue{};
		};

		ScopedOverride PushScoped(PersistScope scope, const string& key, const Type& tempValue)
		{
			return ScopedOverride(*this, scope, key, tempValue);
		}
	private:
		using Bucket = unordered_map<string, Type>;
		Bucket store[ENUM(PersistScope::END)];
	};

public:
	TPersistStore<_bool>   Bool;
	TPersistStore<_uint>   Int64;
	TPersistStore<_int>    Double;
	TPersistStore<string>  String;
};

NS_END
