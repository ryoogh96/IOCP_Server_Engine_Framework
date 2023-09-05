#pragma once
#include "Core/ServerEngineTypes.hpp"
#include "Memory/Container.hpp"
#include "rapidxml.hpp"

using namespace rapidxml;

namespace Engine
{
	using XmlNodeType = xml_node<WCHAR>;
	using XmlDocumentType = xml_document<WCHAR>;
	using XmlAttributeType = xml_attribute<WCHAR>;

	class XmlNode
	{
	public:
		XmlNode(XmlNodeType* node = nullptr) : m_Node(node) { }

		bool				IsValid() { return m_Node != nullptr; }

		bool				GetBoolAttr(const WCHAR* key, bool defaultValue = false);
		int8				GetInt8Attr(const WCHAR* key, int8 defaultValue = 0);
		int16				GetInt16Attr(const WCHAR* key, int16 defaultValue = 0);
		int32				GetInt32Attr(const WCHAR* key, int32 defaultValue = 0);
		int64				GetInt64Attr(const WCHAR* key, int64 defaultValue = 0);
		float				GetFloatAttr(const WCHAR* key, float defaultValue = 0.0f);
		double				GetDoubleAttr(const WCHAR* key, double defaultValue = 0.0);
		const WCHAR* GetStringAttr(const WCHAR* key, const WCHAR* defaultValue = L"");

		bool				GetBoolValue(bool defaultValue = false);
		int8				GetInt8Value(int8 defaultValue = 0);
		int16				GetInt16Value(int16 defaultValue = 0);
		int32				GetInt32Value(int32 defaultValue = 0);
		int64				GetInt64Value(int64 defaultValue = 0);
		float				GetFloatValue(float defaultValue = 0.0f);
		double				GetDoubleValue(double defaultValue = 0.0);
		const WCHAR* GetStringValue(const WCHAR* defaultValue = L"");

		XmlNode				FindChild(const WCHAR* key);
		Vector<XmlNode>		FindChildren(const WCHAR* key);

	private:
		XmlNodeType* m_Node = nullptr;
	};

	class XmlParser
	{
	public:
		bool ParseFromFile(const WCHAR* path, OUT XmlNode& root);

	private:
		std::shared_ptr<XmlDocumentType>		m_Document = nullptr;
		String									m_Data;
	};
}