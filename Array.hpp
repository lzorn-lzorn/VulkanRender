#ifndef ARRAY_HPP
#define ARRAY_HPP
#if _cplusplus > 202002L 

#include <memory>
#include <new>
#include <compare>
#include <type_traits>
#include <initializer_list>
#include <iterator>

namespace Potato{

namespace TypeTools{
template <typename ValueType>
struct SimpleType {
	using value_type      = ValueType;
	using size_type       = std::size_t;
	using difference_type = std::ptrdiff_t;
	using pointer         = ValueType*;
	using const_pointer   = const ValueType*;
	using reference       = ValueType&;
	using const_reference = const ValueType&;
};

template <typename Alloc>
struct IsSimpleAlloc = 
	std::is_same_v <
		typename allocator_traits<Alloc>::size_type, 
		std::size_t
	> &&,
	std::is_same_v <
		typename allocator_traits<Alloc>::difference_type, 
		std::ptrdiff_t
	> &&,
	std::is_same_v <
		typename allocator_traits<Alloc>::pointer, 
		std::allocator_traits<Alloc>::value_type*
	> &&,
	std::is_same_v <
		typename allocator_traits<Alloc>::const_pointer, 
		const std::allocator_traits<Alloc>::value_type*
	>&&,
	std::is_same_v <
		typename allocator_traits<Alloc>::reference, 
		std::allocator_traits<Alloc>::value_type&
	> &&,
	std::is_same_v <
		typename allocator_traits<Alloc>::const_reference, 
		const std::allocator_traits<Alloc>::value_type&
	>&&
};

namespace MemoryTools{

template <typename Container>
struct [[nodiscard]] TidyGuard {
	Container* container;
	constexpr ~TidyGuard() {
		if (container) {
			container->M_Clear();
		} 
	}
};

struct ZeroConstructCompressedTag{
	explicit ZeroConstructCompressedTag() = default;
};
struct OneConstructCompressedTag{
	explicit OneConstructCompressedTag() = default;
};
template <typename Compressed, typename Content, bool = std::is_empty_v<Compressed> && !std::is_final_v<Compressed>>
struct CompressedPair final : private Compressed {
public:
	Content data;

	using Base = Compressed;

	template <typename ... Other2>
	constexpr explicit CompressedPair(ZeroConstructCompressedTag, Other2&& ... args)
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<Compressed>, std::is_nothrow_constructible<Content, Other2...>>)
		: Base(), data(std::forward<Other2>(args)...) {}

	template <typename Other1, typename ... Other2>
	constexpr explicit CompressedPair(OneConstructCompressedTag, Other1&& arg1 Other2&& ... args2)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<Compressed, Other1>, std::is_nothrow_constructible<Content, Other2...>>)
		: Base(std::forward<Other1>(arg1)), data(std::forward<Other2>(args2)...) {}

	constexpr Compressed& GetFirst() noexcept {
		return *this;
	}
	constexpr Compressed& GetFirst() const noexcept {
		return *this;
	}

};

template <typename Compressed, typename Content>
struct CompressedPair final {
public:
	Content data;
	Compressed first;
	template <typename ... Other2>
	constexpr explicit CompressedPair(ZeroConstructCompressedTag, Other2&& ... args)
		noexcept(std::conjunction_v<std::is_nothrow_default_constructible<Compressed>, std::is_nothrow_constructible<Content, Other2...>>)
		: Base(), data(std::forward<Other2>(args)...) {}

	template <typename Other1, typename ... Other2>
	constexpr explicit CompressedPair(OneConstructCompressedTag, Other1&& arg1 Other2&& ... args2)
		noexcept(std::conjunction_v<std::is_nothrow_constructible<Compressed, Other1>, std::is_nothrow_constructible<Content, Other2...>>)
		: Base(std::forward<Other1>(arg1)), data(std::forward<Other2>(args2)...) {}

	constexpr Compressed& GetFirst() noexcept {
		return first;
	}
	constexpr Compressed& GetFirst() const noexcept {
		return first;
	}
};

template <typename Alloc>
using AllocPointer = typename std::allocator_traits<Alloc>::pointer;

template <typename Ptr>
constexpr auto Unfancy(Ptr ptr) noexcept -> decltype(std::addressof(*ptr)){
	return std::addressof(*ptr);
}

template <typename Ty>
constexpr Ty* Unfancy(Ty* ptr) noexcept {
	return ptr;
}

template <typename Alloc, typename = void>
struct HasMemberDestroy : std::false_type{};

template <typename Alloc>
struct HasMemberDestroy <
	Alloc,
	std::void_t<
	/* std::declval<Alloc&>() 产生一个 Alloc 的左值引用类型, 能检测到对左值限定（&）的成员函数 */
	/* std::declval<Alloc>() 产生一个 Alloc 的右值引用类型 */
	/* 但是allocator_traits::destroy 在真实调用时接受 Alloc& */
	/* 检测 Alloc 是否有自定义的 destroy(pointer) */
		decltype(std::declval<Alloc&>().destroy(
			std::declval<typename std::allocator_traits<Alloc>::pointer>()
		))
	>
> : std::true_type{};


template <typename Alloc>
constexpr bool UsesDefaultDeatroy = (!HasMemberDestroy<Alloc>::value) ||
	std::is_same_v<Alloc, std::allocator<typename Alloc::value_type>>

template <typename Alloc>
constexpr void DestroyRange(AllocPointer<Alloc> first, AllocPointer<Alloc> last, Alloc& allocator) noexcept {
	using value_type = typename Alloc::value_type
	if constexpr (!(std::is_trivially_destructible<value_type> && UsesDefaultDestroyVal<Alloc>)){
		for (; first != last; ++first){
			std::allocator_traits<Alloc>::destroy(allocator, std::addressof(*first));
		}
	}
}
};

template <typename ElementTypeWapper>
struct ArrayData {
	using value_type      = typename ElementTypeWapper::value_type;
	using size_type       = typename ElementTypeWapper::size_type;
	using difference_type = typename ElementTypeWapper::difference_type;
	using pointer         = typename ElementTypeWapper::pointer;
	using const_pointer   = typename ElementTypeWapper::const_pointer;
	using reference       = typename ElementTypeWapper::reference;
	using const_reference = typename ElementTypeWapper::const_reference;

	constexpr ArrayData() noexcept
		: M_start()
		, M_finish()
		, M_End_Of_Storage(){}

	constexpr ArrayData(pointer start, pointer finish, pointer end) noexcept
		: M_start(start)
		, M_finish(finish)
		, M_End_Of_Storage(end){}

	constexpr void CopyData(ArrayData const& other) noexpect() {
		M_start = other.M_start;
		M_finish = other.M_finish;
		M_End_Of_Storage = other.M_End_Of_Storage;
	}

	constexpr void SwapData(ArrayData& other) noexcept {
		std::swap(M_start, other.M_start);
		std::swap(M_finish, other.M_finish);
		std::swap(M_End_Of_Storage, other.M_End_Of_Storage);
	}

	constexpr void StealData(ArrayData& other) noexcept {
		M_start = other.M_start;
		M_finish = other.M_finish;
		M_End_Of_Storage = other.M_End_Of_Storage;

		other.M_start = nullptr;
		other.M_finish = nullptr;
		other.M_End_Of_Storage = nullptr;
	}

	pointer M_start;
	pointer M_finish;
	pointer M_End_Of_Storage;
};

template <typename ValueType, typename SizeType, typename DifferenceType, typename Pointer, typename ConstPointer, typename Reference, typename ConstReference>
struct GenerateElementWapper {
	using value_type      = ValueType;
	using size_type       = SizeType;
	using difference_type = DifferenceType;
	using pointer         = Pointer;
	using const_pointer   = ConstPointer;
	using reference       = Reference;
	using const_reference = ConstReference;
};

template <typename Array>
struct ArrayConstIterator {};

template <typename Array>
struct ArrayIterator : public ArrayConstIterator<Array>{};


template <typename ElementType, class AllocatorType=std::allocator<ElementType>>
class Array {
private:
	using M_AllocatorType = 
		typename std::allocator_traits<typename DerivedClass::allocator_type>>
			::template rebind_alloc<typename DerivedClass::value_type>;
	using M_AllocatorTraits = std::allocator_traits<M_AllocatorType>;

	using M_DateType = ArrayData<
		std::conditional_t<TypeTools::IsSampleAlloc<M_AllocatorType>,
			TypeTools::SampleType<ElementType>,
			GenerateElementWapper<
				ElementType,
				size_type,
				difference_type,
				pointer,
				const_pointer,
				reference,
				const_reference
			>
		>
	>

	using M_RealValueType = MemoryTools::CompressedPair<M_AllocatorType, M_DateType>;
public:
	using value_type             = ElementType;
	using allocator_type         = AllocatorType;
	using size_type              = M_AllocatorTraits::size_type;
	using difference_type        = M_AllocatorTraits::difference_type;
	using pointer                = M_AllocatorTraits::pointer;
	using const_pointer          = M_AllocatorTraits::const_pointer;
	using reference              = M_AllocatorTraits::reference;
	using const_reference        = M_AllocatorTraits::const_reference;
	using iterator               = ArrayIterator<Array>;
	using const_iterator         = ArrayConstIterator<Array>;
	using reverse_iterator       = std::reverse_iterator<iterator>;
	using reverse_const_iterator = std::reverse_iterator<const_iterator>;

public:
	constexpr Array() noexcept () {}
	constexpr explicit Array(const AllocatorType& allocator) noexcept () {}
	constexpr explicit Array(size_type count, const AllocatorType& allocator) {}
	constexpr Array(size_type count, const reference value, const AllocatorType& allocator) {}
	template <typename InputIterator>
	constexpr Array(InputIterator first, InputIterator last, const AllocatorType& allocator) {}
	Array(std::initializer_list<value_type> list, const AllocatorType& allocator);
	constexpr Array(const Array& other) {}
	constexpr Array(Array&& other) noexcept {}

public:
	[[nodiscard]] constexpr Array& operator=(const Array& other) {}
	[[nodiscard]] constexpr Array& operator=(Array&& other) noexcept {}
	[[nodiscard]] constexpr Array& operator=(std::initializer_list<value_type> list) {}
	[[nodiscard]] constexpr reference operator[](const size_type index) noexcept {}
	[[nodiscard]] constexpr const reference operator[](const size_type index) const noexcept {}
	[[nodiscard]] Array& operator+=(const Array& other) {}
	[[nodiscard]] Array& operator+=(Array&& other) {}
	[[nodiscard]] Array& operator+=(std::initializer_list<value_type> list) {}
public:
	constexpr void Clear() noexcept {}

	value_type* Data() noexcept {}
	const value_type* Data() const noexcept {}

	void Assign(size_type count, const value_type& value) {}
	template <typename InputIterator>
	constexpr void Assign(InputIterator first, InputIterator last) {}
	constexpr void Assign(std::initializer_list<value_type> list) {}

	[[nodiscard]] constexpr reference At(const size_type index) {}
	[[nodiscard]] constexpr reference At(const size_type index) const {}
	[[nodiscard]] constexpr const reference At(const size_type index) {}
	[[nodiscard]] constexpr const reference At(const size_type index) const {}

	[[nodiscard]] constexpr size_type Find(const reference item) const {}
	[[nodiscard]] constexpr size_type Find(const reference item) {}
	template <typename Predicate>
	[[nodiscard]] constexpr size_type FindIf(Predicate pred) const {}
	template <typename Predicate>
	[[nodiscard]] constexpr size_type FindIf(Predicate pred) {}

	template <typename Predicate>
	[[nodiscard]] constexpr Filter(Predicate pred) const {}
	template <typename Predicate>
	[[nodiscard]] constexpr Filter(Predicate pred) {}

	[[nodiscard]] constexpr size_type Count(const reference item) const {}
	[[nodiscard]] constexpr size_type Count(Predicate pred) const {}

	[[nodiscard]] constexpr bool IsContain(const reference item) const {}
	template <typename Predicate>
	[[nodiscard]] constexpr bool IsContain(Predicate pred) const {}

	[[nodiscard]] constexpr reference Front(const size_type index) {}
	[[nodiscard]] constexpr reference Front(const size_type index) const {}
	[[nodiscard]] constexpr const reference Front(const size_type index) {}
	[[nodiscard]] constexpr const reference Front(const size_type index) const {}

	[[nodiscard]] constexpr reference Back(const size_type index) {}
	[[nodiscard]] constexpr reference Back(const size_type index) const {}
	[[nodiscard]] constexpr const reference Back(const size_type index) {}
	[[nodiscard]] constexpr const reference Back(const size_type index) const {}

	constexpr iterator Insert(const_iterator position, value_type& value) {}
	constexpr iterator Insert(const_iterator position, const value_type& value) {}
	constexpr iterator Insert(const_iterator position, size_type count, const value_type& value) {}
	template <typename InputIterator>
	constexpr iterator Insert(const_iterator position, InputIterator first, InputIterator last) {}
	constexpr iterator Insert(const_iterator position, std::initializer_list<value_type> list) {}

	constexpr iterator InsertAt(size_type index, value_type& value) {}
	constexpr iterator InsertAt(size_type index, const value_type& value) {}
	constexpr iterator InsertAt(size_type index, size_type count, const value_type& value) {}

	constexpr iterator InsertUnique(size_type index, value_type& value) {}
	constexpr iterator InsertUnique(size_type index, const value_type& value) {}


	constexpr void Append(Array&& array) {}
	constexpr void Append(pointer ptr, size_type count) {}

	constexpr void InsertUninitializedItem(const_iterator position) {}
	constexpr void InsertUninitializedItem(const_iterator position, const size_type count) {}
	constexpr reference InsertZeroedItem(const_iterator position) {}
	constexpr reference InsertZeroedItem(const_iterator position, const size_type count) {}

	constexpr iterator Erase(const_iterator position){}
	constexpr iterator Erase(const_iterator first, const_iterator last){}
	constexpr iterator EraseAt(const size_type index){}
	template <typename Predicate>
	constexpr iterator EraseIf(Predicate pred){}

	template <typename ... Args>
	constexpr iterator Emplace(const_iterator position, Args&& ... args) {}
	template <typename ... Args>
	constexpr iterator EmplaceAt(size_type index, Args&& ... args) {}

	template <typename ... Args>
	constexpr reference EmplaceBack(Args&& ... args) {}

	constexpr iterator Push(const value_type& value) {}
	constexpr iterator Push(value_type& value) {}
	constexpr iterator Pop() noexcept {}
	constexpr iterator Top() noexcept {} 
	constexpr const_iterator Top() const noexcept {} 

	constexpr void Resize(size_type count) {}
	constexpr void Resize(size_type count, const value_type& value) {}
	constexpr void Reserve(size_type capacity) {}
	constexpr void Swap() noexcept {}
  
	bool IsEmpty() const noexcept {}
	size_type GetSize() const noexcept {}
	size_type GetCapacity() const noexcept {}
	size_type GetSlack() const noexcept {}

	void Shrink() noexcept {}

public:
	[[nodiscard]] constexpr iterator begin() noexcept {}
	[[nodiscard]] constexpr const_iterator begin() const noexcept {}
	[[nodiscard]] constexpr const_iterator cbegin() const noexcept {}

	[[nodiscard]] constexpr iterator end() noexcept {}
	[[nodiscard]] constexpr const_iterator end() const noexcept {}
	[[nodiscard]] constexpr const_iterator cend() const noexcept {}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept {}
	[[nodiscard]] constexpr reverse_const_iterator rbegin() const noexcept {}
	[[nodiscard]] constexpr reverse_const_iterator crbegin() const noexcept {}

	[[nodiscard]] constexpr reverse_iterator rend() noexcept {}
	[[nodiscard]] constexpr reverse_const_iterator rend() const noexcept {}
	[[nodiscard]] constexpr reverse_const_iterator crend() const noexcept {}

private:
	template <typename ... ConstructParams>
	constexpr void M_Construct(const size_type count, ConstructParams&& ... args){

	}

	constexpr size_type M_CalculateGrowth(const size_type newsize) const {

	}
	[[nodiscard]] constexpr size_type M_Capacity() const noexcept {
		auto& M_Data            = this->m_Data.data;
		pointer& start          = M_Data.M_start;
		pointer& finish         = M_Data.M_finish;
		pointer& end_of_storage = M_Data.M_End_Of_Storage;

		return start ? static_cast<size_type>(end_of_storage - start) : 0u;
	}

	[[nodiscard]] constexpr size_type M_Size() const noexcept {
		auto& M_Data            = this->m_Data.data;
		pointer& start          = M_Data.M_start;
		pointer& finish         = M_Data.M_finish;
		pointer& end_of_storage = M_Data.M_End_Of_Storage;

		return start ? static_cast<size_type>(finish - start) : 0u;
	}

	[[nodiscard]] constexpr bool M_IsEmpty() const noexcept {
		return M_Size() == 0;
	}

	/**
	* @brief 分配未初始化内存, 不会调用构造函数, 也不会进行任何安全检测
	* @param capacity 分配的容量
	*/ 
	constexpr void M_AllocateUninitializedMemory(size_type capacity){
		auto& M_Data            = this->m_Data.data;
		pointer& start          = M_Data.M_start;
		pointer& finish         = M_Data.M_finish;
		pointer& end_of_storage = M_Data.M_End_Of_Storage;

		const pointer mem = M_GetAllocator().allocate(capacity);
		start           = mem;
		finish          = mem;
		end_of_storage  = mem + capacity;
	}

	/**
	* @brief 分配未初始化内存, 不会调用构造函数, 但但是会进行安全检测
	* @param capacity 分配的容量
	*/
	constexpr void M_AllocateNonZeroUninitializedMemory(const size_type capacity) {
		auto& M_Data            = this->m_Data.data;
		pointer& start          = M_Data.M_start;
		pointer& finish         = M_Data.M_finish;
		pointer& end_of_storage = M_Data.M_End_Of_Storage;

		assert(capacity != 0 && "M_AllocateNonZeroUninitializedMemory Runtieme Error: capacity can not be zero.");
		assert(start != nullptr && finish != nullptr && end_of_storage != nullptr && "M_AllocateNonZeroUninitializedMemory Runtieme Error: memory has been allocated.");

		M_AllocateUninitializedMemory(capacity);
	}

	/**
	* @brief 重新定位数组内存, 并更新内部指针
	* @param newmem 新的内存地址
	* @param newsize 新的已构造元素数量
	* @param capacity 新的容量
	*/
	constexpr void M_RelocateArray(const pointer newmem, const size_type newsize, const size_type capacity) noexcept {
		auto& allocator = M_GetAllocator();
		auto& M_Data            = this->m_Data.data;
		pointer& start          = M_Data.M_start;
		pointer& finish         = M_Data.M_finish;
		pointer& end_of_storage = M_Data.M_End_Of_Storage;

		const size_type old_size = start ? static_cast<size_type>(finish - start) : 0u;
		const size_type old_capacity = start ? static_cast<size_type>(end_of_storage - start) : 0u;

		/* 检查新内存是否与就旧地址重合 */ 
		if (newmem == start) {
			finish = start + newsize;
			end_of_storage = start + capacity;
			return;
		}

		/* 检查 capacity 是否为0, 为0则释放内存 */
		if (capacity == 0) {
			if (start){
				TypeTools::MemoryTools::DestroyRange<M_AllocatorType>(start, finish, allocator);
				M_AllocatorType::deallocate(allocator, start, old_capacity);
				start = nullptr;
				finish = nullptr;
				end_of_storage = nullptr;
			}
			return;
		}

		pointer newstart = newmem;
		pointer newfinish = newmem;

		/* 元素迁移 */
		const size_type to_move = (old_size < newsize) ? old_size : newsize;
		size_type constructed = 0u;
		try {
			for (; constructed < to_move; ++constructed){
				pointer src = start + constructed;
				if constexpr (std::is_nothrow_move_constructible_v<value_type> 
						|| !std::is_copy_constructible_v<value_type>){	
					M_AllocatorType::construct(allocator, newstart + constructed, std::move(*src));		
				} else {
					M_AllocatorType::construct(allocator, newstart + constructed, *src);
				}
				++newfinish;
			}
			for (; constructed < newsize; ++constructed, ++newfinish){
				M_AllocatorType::construct(allocator, newstart + constructed);
			}
		} catch(...){
			TypeTools::MemoryTools::DestroyRange<M_AllocatorType>(newstart, newstart + constructed, allocator);
			M_AllocatorType::deallocate(allocator, newstart, capacity);
		}

		/* 销毁旧的内存地址中的元素 */
		if (start) {
			TypeTools::MemoryTools::DestroyRange<M_AllocatorType>(start, finish, allocator);
			M_AllocatorType::deallocate(allocator, start, old_capacity);
		}

		start = newmem;
		finish = newmem + newsize;
		end_of_storage = newmem + capacity;
	}
	/**
	* @brief 清除所有已从start开始, 往后count个元素, 释放调用析构函数
	*/
	constexpr void M_DestroyRange(pointer start, const size_type count) noexcept {
		auto& allocator = M_GetAllocator();
		auto& M_Data            = this->m_Data.data;
		pointer& start          = M_Data.M_start;
		pointer& finish         = M_Data.M_finish;
		pointer& end_of_storage = M_Data.M_End_Of_Storage;
	}

	[[nodiscard]] constexpr const M_AllocatorType& GetAllocator() const noexcept {
		return m_Data.GetFirst();
	}
	[[nodiscard]] constexpr M_AllocatorType& GetAllocator() noexcept {
		return m_Data.GetFirst();
	}

private:
	M_RealValueType m_Data;
};
}

#endif // _cplusplus > 202002L
#endif // ARRAY_HPP