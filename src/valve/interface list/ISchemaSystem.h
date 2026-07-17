#pragma once

#define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x190/*0x188*/
#define SCHEMASYSTEMTYPESCOPE_OFF1 0x3F8 // 824
#define SCHEMASYSTEMTYPESCOPE_OFF2 0x8

using SchemaString_t = const char*;
struct SchemaMetadataEntryData_t;

class CSchemaSystemTypeScope;
class CSchemaType;

struct CSchemaClassBinding
{
	CSchemaClassBinding* pParent;
	const char* szBinaryName; // ex: C_World
	const char* szModuleName; // ex: libclient.so
	const char* szClassName; // ex: client
	void* pClassInfoOldSynthesized;
	void* pClassInfo;
	void* pThisModuleBindingPointer;
	CSchemaType* pSchemaType;
};

class CSchemaType
{
public:
	bool GetSizes(int* pOutSize, uint8_t* unkPtr)
	{
		return Memory::CallVFunc<bool, 3U>(this, pOutSize, unkPtr);
	}

	bool GetSize(int* pOutSize)
	{
		uint8_t smh = 0;
		return GetSizes(pOutSize, &smh);
	}
public:
	void* pVtable; // 0x0000
	const char* szName; // 0x0008
	CSchemaSystemTypeScope* pSystemTypeScope; // 0x0010
	uint8_t nTypeCategory; // ETypeCategory 0x0018
	uint8_t nAatomicCategory; // EAtomicCategory 0x0019
};

struct SchemaClassFieldData_t
{
	SchemaString_t szName; // 0x0000
	CSchemaType* pSchemaType; // 0x0008
	std::uint32_t nSingleInheritanceOffset; // 0x0010
	std::int32_t nMetadataSize; // 0x0014
	SchemaMetadataEntryData_t* pMetaData; // 0x0018
};

struct SchemaClassInfoData_t;
struct SchemaStaticFieldData_t;
struct SchemaBaseClassInfoData_t
{
	unsigned int uOffset;
	SchemaClassInfoData_t* pClass;
};

struct SchemaClassInfoData_t
{
	MEM_PAD(0x8); // 0x0000

	const char* szName; // 0x0008
	char* szModuleName; // 0x0010

	int m_nSize; // 0x0018
	std::int16_t nFieldSize; // 0x001C

	std::int16_t nStaticSize; // 0x001E
	std::int16_t nMetadataSize; // 0x0020
	std::uint8_t nAlignOf; // 0x0022
	std::uint8_t nBaseClassesCount; // 0x0023
	MEM_PAD(0x4); // 0x0024
public:
	SchemaClassFieldData_t* pFields; // 0x0028
	SchemaStaticFieldData_t* pStaticFields; // 0x0030
	SchemaBaseClassInfoData_t* pSchemaParent; // 0x0038

	bool InheritsFrom(SchemaClassInfoData_t* pClassInfo)
	{
		if (pClassInfo == this && pClassInfo)
			return true;
		else if (!pSchemaParent || !pClassInfo)
			return false;

		for (int i = 0; i < nBaseClassesCount; i++)
		{
			SchemaBaseClassInfoData_t& baseClass = pSchemaParent[i];
			if (baseClass.pClass->InheritsFrom(pClassInfo))
				return true;
		}

		return false;
	}
};

struct SchemaEnumeratorInfoData_t
{
	SchemaString_t szName;

	union
	{
		unsigned char value_char;
		unsigned short value_short;
		unsigned int value_int;
		unsigned long long value;
	};

	MEM_PAD(0x10); // 0x0010
};

class CSchemaEnumInfo
{
public:
	SchemaEnumeratorInfoData_t enumInfoData;
};

class CSchemaEnumBinding
{
public:
	virtual const char* GetBindingName() = 0;
	virtual CSchemaClassBinding* AsClassBinding() = 0;
	virtual CSchemaEnumBinding* AsEnumBinding() = 0;
	virtual const char* GetBinaryName() = 0;
	virtual const char* GetProjectName() = 0;

public:
	char* szBindingName_; // 0x0008
	char* szDllName_; // 0x0010
	std::int8_t nAlign_; // 0x0018
	MEM_PAD(0x3); // 0x0019
	std::int16_t nSize_; // 0x001C
	std::int16_t nFlags_; // 0x001E
	SchemaEnumeratorInfoData_t* pEnumInfo_;
	MEM_PAD(0x8); // 0x0028
	CSchemaSystemTypeScope* pTypeScope_; // 0x0030
	MEM_PAD(0x8); // 0x0038
	std::int32_t unk1_; // 0x0040
};

template <class K, class V>
class CSchemaPtrMap
{
public:
	CUtlMap<K, V> m_Map;
	CThreadFastMutex m_Mutex;
};

class schema_field
{
public:
	const char* name;
	void* type;
	uint32_t offset;
	uint32_t metadata_size;
	void* metadata;
};

class some_class
{
public:
	void* vtable;
	const char* name;
	const char* module_name;
	uint32_t size;
	uint16_t fields_num;
	MEM_PAD(2);
	uint16_t staticSize;
	uint16_t metadataSize;
	MEM_PAD(4);
	schema_field* fields;
};

class declared_class
{
public:
	void* vtable;
	const char* name;
	const char* module_name;
	const char* unknown_str;
	some_class* schema_class;
};

class declared_class_entry
{
public:
	uint64_t hash[2];
	declared_class* declared_classes;
};

class CSchemaSystemTypeScope
{
public:
	void FindDeclaredClass(SchemaClassInfoData_t** pReturnClass, const char* szClassName)
	{
		return Memory::CallVFunc<void, 2U>(this, pReturnClass, szClassName);
	}

	void* pVtable; // 0x0000
	char szName[256U]; // 0x0008
	MEM_PAD(824);
	uint16_t declared_classes_num;
	MEM_PAD(6);
	declared_class_entry* declared_classes; //0x448
};

class ISchemaSystem
{
public:
	CSchemaSystemTypeScope* GlobalTypeScope()
	{
		return Memory::CallVFunc<CSchemaSystemTypeScope*, 11U>(this);
	}

	CSchemaSystemTypeScope* FindTypeScopeForModule(const char* szModuleName)
	{
		return Memory::CallVFunc<CSchemaSystemTypeScope*, 13U>(this, szModuleName, nullptr);
	}

	MEM_PAD(SCHEMASYSTEM_TYPE_SCOPES_OFFSET); // 0x0000:
	CUtlVector<CSchemaSystemTypeScope*> vecTypeScopes; // SCHEMASYSTEM_TYPE_SCOPES_OFFSET
};