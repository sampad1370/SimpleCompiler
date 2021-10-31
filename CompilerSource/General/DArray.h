#pragma once
#include <map>
#include <vector>
#include <unordered_map>

namespace BaseEngine
{
	namespace Core
	{
		namespace Array
		{
#define PStlFind(P,T) ((P)->find(T)!=(P)->end())
#define StlFind(P,T) ((P).find(T)!=(P).end())
#define StlInsert(Array,Data) Array.pushback(Data);
			template<typename T, typename M>
			using _mapType = std::map < T, M, std::less < T > 
#if defined(DGE_Memory_Allocator)&&DGE_Memory_Allocator == DGE_Memory_NedMalloc
				, nedalloc::nedallocator < T* >
#endif
			> ; //Array of Pointer Ready For Use In Engine;
			#pragma region DArray Implementation
			//Dynamic Array Optimized
			template<typename T>
			class DArray : public std::vector < T
#if defined(DGE_Memory_Allocator)&&DGE_Memory_Allocator==DGE_Memory_NedMalloc
				, nedalloc::nedallocator<T>
#endif
			>//Array Ready For Use In Engine
			{
			public:
				//get last element data from array.
				T GetLast()
				{
					return *(--this->end());
				}

				//get first element data from array.
				T GetFirst()
				{
					return *(this->first());
				}
				virtual ~DArray()
				{
					/*for (auto p : *this)
						SafeDelete(p);*/
					this->clear();
				}
			};
			#pragma endregion


			//Dynamic Array of pointer that Optimized
			template<typename T>
			class DArrayPointer : public std::vector < T*
#if defined(DGE_Memory_Allocator)&&DGE_Memory_Allocator == DGE_Memory_NedMalloc
				,nedalloc::nedallocator<T*>
#endif
			> //Array of Pointer Ready For Use In Engine
			{
			public:

				//get last element data from array.
				T* GetLast()
				{
					return *(--this->end());
				}

				//get first element data from array.
				T* GetFirst()
				{
					return *(this->first());
				}

				const_iterator Find(T* data)
				{
					for (auto ptr = this->cbegin(); ptr != this->cend();ptr++)
						if (*ptr == data)
							return ptr;
					return this->end();
				}

				inline const_iterator find(T* data)
				{
					return Find(data);
				}

				virtual ~DArrayPointer()
				{
					this->clear();
				}

			};

			template<typename T, typename M>
			class HashTable : public std::unordered_map<T, M, std::hash<T>, std::equal_to<T>
#if defined(DGE_Memory_Allocator)&&DGE_Memory_Allocator == DGE_Memory_NedMalloc
				, nedalloc::nedallocator<T*>
#endif
			> //Array of Pointer Ready For Use In Engine
			{
			public:

				//get last element data from array.
				T* GetLast()
				{
					return *(--this->end());
				}

				//get first element data from array.
				T* GetFirst()
				{
					return *(this->first());
				}

				const_iterator Find(T* data)
				{
					for (auto ptr = this->cbegin(); ptr != this->cend(); ptr++)
					if (*ptr == data)
						return ptr;
					return this->end();
				}

				virtual ~HashTable()
				{
					this->clear();
				}

			};

			template<typename T, typename M>
			class Map : public _mapType<T,M> //Array of Pointer Ready For Use In Engine
			{
			public:

				//get last element data from array.
				T* GetLast()
				{
					return *(--this->end());
				}

				//get first element data from array.
				T* GetFirst()
				{
					return *(this->first());
				}

				virtual ~Map()
				{
					this->clear();
				}

				template<class type>
				type& Get(T&& key)
				{
					return (type&) _mapType<T, M>::operator[](key);
				}

			};
		}
	}
}