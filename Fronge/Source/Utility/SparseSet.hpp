// Written based on Michele "skypjack" Caini's
// blog posts about ECS (https://skypjack.github.io/)
// as well as his EnTT library (https://github.com/skypjack/entt)

#if not defined SPARSE_SET_HPP
#define SPARSE_SET_HPP

#include "froch.hpp"

#include "Core.hpp"

namespace fro
{
	template<std::swappable DataType>
	class SparseSet final
	{
	public:
		using Key = std::size_t;
		using DataIndex = std::size_t;
		using DenseType = std::pair<Key, DataType>;

		static DataIndex constexpr UNUSED_DATA_INDEX{ std::numeric_limits<DataIndex>::max() };

		SparseSet() = default;
		SparseSet(SparseSet const&) = default;
		SparseSet(SparseSet&&) noexcept = default;

		~SparseSet() = default;

		SparseSet& operator=(SparseSet const&) = default;
		SparseSet& operator=(SparseSet&&) noexcept = default;
		FRO_NODISCARD std::pair<DataType&, bool> operator[](Key const key)
			requires std::default_initializable<DataType>
		{
			if (not inSparseRange(key))
				mSparse.resize(key + 1, UNUSED_DATA_INDEX);

			else if (naiveContains(key))
				return { naiveFind(key).second, false };

			return { naiveInsert(key, {}).second, true };
		}

		DataType* insert(Key const key, DataType data)
		{
			if (not inSparseRange(key))
				mSparse.resize(key + 1, UNUSED_DATA_INDEX);

			else if (naiveContains(key))
				return nullptr;

			return &naiveInsert(key, std::move(data)).second;
		}

		bool erase(Key const key)
		{
			if (not contains(key))
				return false;

			naiveMove(key, mDense.size() - 1);
			mSparse[key] = UNUSED_DATA_INDEX;
			mDense.pop_back();

			return true;
		}

		FRO_NODISCARD DataType* find(Key const key)
		{
			if (not contains(key))
				return nullptr;

			return &naiveFind(key).second;
		}

		void clear()
		{
			mSparse.clear();
			mDense.clear();
		}

		void shrinkSparse()
		{
			auto const lastUsedKey
			{
				std::find_if(mSparse.rbegin(), mSparse.rend(),
				[](Key const key)
				{
					return key not_eq UNUSED_DATA_INDEX;
				}).base()
			};

			mSparse.erase(lastUsedKey, mSparse.end());
			mSparse.shrink_to_fit();
		}

		void shrinkDense()
		{
			mDense.shrink_to_fit();
		}

		void shrink()
		{
			shrinkSparse();
			shrinkDense();
		}

		bool move(Key const key, DataIndex const where)
		{
			if (not inDenseRange(where) or not contains(key))
				return false;

			naiveMove(key, where);
			return true;
		}

		FRO_NODISCARD bool contains(Key const key) const
		{
			return inSparseRange(key) and naiveContains(key);
		}

		FRO_NODISCARD std::size_t sparseSize() const
		{
			return mSparse.size();
		}

		FRO_NODISCARD std::size_t sparseCapacity() const
		{
			return mSparse.capacity();
		}

		FRO_NODISCARD std::size_t denseSize() const
		{
			return mDense.size();
		}

		FRO_NODISCARD std::size_t denseCapacity() const
		{
			return mDense.capacity();
		}

	private:
		FRO_NODISCARD bool inSparseRange(Key const key) const
		{
			return key < mSparse.size();
		}

		FRO_NODISCARD bool inDenseRange(DataIndex const dataIndex) const
		{
			return dataIndex < mDense.size();
		}

		FRO_NODISCARD bool naiveContains(Key const key) const
		{
			return mSparse[key] not_eq UNUSED_DATA_INDEX;
		}

		FRO_NODISCARD DenseType& naiveFind(Key const key)
		{
			return mDense[mSparse[key]];
		}

		DenseType& naiveInsert(Key const key, DataType&& data)
		{
			mSparse[key] = mDense.size();
			return mDense.emplace_back(key, std::move(data));
		}

		void naiveMove(Key const key, DataIndex const where)
		{
			Key const otherKey{ mDense[where].first };
			std::swap(naiveFind(key), mDense[where]);
			std::swap(mSparse[key], mSparse[otherKey]);
		}

		std::vector<DataIndex> mSparse{};
		std::vector<DenseType> mDense{};
	};
}

#endif