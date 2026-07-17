#include "../Precompiled.h"

struct SchemaData_t
{
	FNV1A_t uHashedFieldName = 0x0ULL;
	std::uint32_t uOffset = 0x0U;
};

static std::vector<SchemaData_t> vecSchemaData;

bool Schema::Setup()
{
	CSchemaSystemTypeScope* pClientTypeScope = Interfaces::m_pSchemaSystem->FindTypeScopeForModule(X("client.dll"));
	if (!pClientTypeScope)
		return false;

	for (uint16_t c = 0; c < pClientTypeScope->declared_classes_num; ++c)
	{
		declared_class_entry& entry = pClientTypeScope->declared_classes[c];
		declared_class* pDeclaredClassInfo = entry.declared_classes;

		if (!pDeclaredClassInfo || !pDeclaredClassInfo->schema_class)
			continue;

		const char* class_name = pDeclaredClassInfo->schema_class->name;
		if (!class_name)
			continue;

		schema_field* fields = pDeclaredClassInfo->schema_class->fields;
		uint16_t fields_count = pDeclaredClassInfo->schema_class->fields_num;

		if (!fields)
			continue;

		for (uint16_t f = 0; f < fields_count; ++f)
		{
			const char* field_name = fields[f].name;
			if (!field_name)
				continue;

			CRT::String_t<MAX_PATH> szFieldClassBuffer(X("%s->%s"), class_name, field_name);
			vecSchemaData.emplace_back(FNV1A::Hash(szFieldClassBuffer.Data()), fields[f].offset);
		}
	}

	return true;
}

std::uint32_t Schema::GetOffset(const FNV1A_t uHashedFieldName)
{
	if (const auto it = std::ranges::find_if(vecSchemaData, [uHashedFieldName](const SchemaData_t& data) { return data.uHashedFieldName == uHashedFieldName; }); it != vecSchemaData.end())
		return it->uOffset;

	Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
	Logging::Print(X("failed to find offset for field with hash: {}"), uHashedFieldName);
	Logging::PopConsoleColor();

	CS_ASSERT(false); // schema field not found
	return 0U;
}