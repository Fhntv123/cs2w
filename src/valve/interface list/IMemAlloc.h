#pragma once
#pragma warning(push)
#pragma warning(disable : 4191)

class IMemAlloc
{
public:
	void* Alloc(std::size_t nSize)
	{
		return Memory::CallVFunc<void*, 1>(this, nSize);
	}

	void* ReAlloc(const void* pMemory, std::size_t nSize)
	{
		return Memory::CallVFunc<void*, 2>(this, pMemory, nSize);
	}

	void Free(const void* pMemory)
	{
		return Memory::CallVFunc<void, 3>(this, pMemory);
	}

	std::size_t GetSize(const void* pMemory)
	{
		return Memory::CallVFunc<std::size_t, 21>(this, pMemory);
	}

	//IMemAlloc* GetMemAlloc() {
	//	static auto g_pMemAlloc = *reinterpret_cast<IMemAlloc**>(Memory::GetExportAddress(L"tier0.dll", "g_pMemAlloc"));
	//	return g_pMemAlloc;
	//}
};

#pragma warning(pop)