#include "pch.hpp"
#include "XmlParser.hpp"
#include "Utility/FileUtils.hpp"
#include "Core/ServerEngineGlobal.hpp"

namespace Engine
{
	_locale_t kr = _create_locale(LC_NUMERIC, "kor");

	bool XmlNode::GetBoolAttr(const WCHAR* key, bool defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return ::_wcsicmp(attr->value(), L"true") == 0;

		return defaultValue;
	}

	int8 XmlNode::GetInt8Attr(const WCHAR* key, int8 defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return static_cast<int8>(::_wtoi(attr->value()));

		return defaultValue;
	}

	int16 XmlNode::GetInt16Attr(const WCHAR* key, int16 defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return static_cast<int16>(::_wtoi(attr->value()));

		return defaultValue;
	}

	int32 XmlNode::GetInt32Attr(const WCHAR* key, int32 defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return ::_wtoi(attr->value());

		return defaultValue;
	}

	int64 XmlNode::GetInt64Attr(const WCHAR* key, int64 defaultValue)
	{
		xml_attribute<WCHAR>* attr = m_Node->first_attribute(key);
		if (attr)
			return ::_wtoi64(attr->value());

		return defaultValue;
	}

	float XmlNode::GetFloatAttr(const WCHAR* key, float defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return static_cast<float>(::_wtof(attr->value()));

		return defaultValue;
	}

	double XmlNode::GetDoubleAttr(const WCHAR* key, double defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return ::_wtof_l(attr->value(), kr);

		return defaultValue;
	}

	const WCHAR* XmlNode::GetStringAttr(const WCHAR* key, const WCHAR* defaultValue)
	{
		XmlAttributeType* attr = m_Node->first_attribute(key);
		if (attr)
			return attr->value();

		return defaultValue;
	}

	bool XmlNode::GetBoolValue(bool defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return ::_wcsicmp(val, L"true") == 0;

		return defaultValue;
	}

	int8 XmlNode::GetInt8Value(int8 defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return static_cast<int8>(::_wtoi(val));

		return defaultValue;
	}

	int16 XmlNode::GetInt16Value(int16 defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return static_cast<int16>(::_wtoi(val));
		return defaultValue;
	}

	int32 XmlNode::GetInt32Value(int32 defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return static_cast<int32>(::_wtoi(val));

		return defaultValue;
	}

	int64 XmlNode::GetInt64Value(int64 defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return static_cast<int64>(::_wtoi64(val));

		return defaultValue;
	}

	float XmlNode::GetFloatValue(float defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return static_cast<float>(::_wtof(val));

		return defaultValue;
	}

	double XmlNode::GetDoubleValue(double defaultValue)
	{
		WCHAR* val = m_Node->value();
		if (val)
			return ::_wtof_l(val, kr);

		return defaultValue;
	}

	const WCHAR* XmlNode::GetStringValue(const WCHAR* defaultValue)
	{
		WCHAR* val = m_Node->first_node()->value();
		if (val)
			return val;

		return defaultValue;
	}

	XmlNode XmlNode::FindChild(const WCHAR* key)
	{
		return XmlNode(m_Node->first_node(key));
	}

	Vector<XmlNode> XmlNode::FindChildren(const WCHAR* key)
	{
		Vector<XmlNode> nodes;

		xml_node<WCHAR>* node = m_Node->first_node(key);
		while (node)
		{
			nodes.push_back(XmlNode(node));
			node = node->next_sibling(key);
		}

		return nodes;
	}

	bool XmlParser::ParseFromFile(const WCHAR* path, OUT XmlNode& root)
	{
		Vector<BYTE> bytes = FileUtils::ReadFile(path);
		m_Data = FileUtils::Convert(std::string(bytes.begin(), bytes.end()));

		if (m_Data.empty())
			return false;

		m_Document = MakeShared<XmlDocumentType>();
		m_Document->parse<0>(reinterpret_cast<WCHAR*>(&m_Data[0]));
		root = XmlNode(m_Document->first_node());
		return true;
	}
}