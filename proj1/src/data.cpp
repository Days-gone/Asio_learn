#include <data.hpp>
#include <string>

void DataProcessor::init() { doc.SetObject(); }

void DataProcessor::addinfo(std::string const& key, std::string const& value)
{
    rapidjson::Value jsonKey(
        key.data(), static_cast<rapidjson::SizeType>(key.length()), allocator);
    rapidjson::Value jsonValue(value.data(),
                               static_cast<rapidjson::SizeType>(value.length()),
                               allocator);

    doc.AddMember(jsonKey, jsonValue, allocator);
}

std::string DataProcessor::Getstring()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

std::string DataParser::extractValue(const std::string& key)
{
    if (jsonText.empty())
    {
        throw std::invalid_argument("JSON string is empty\n");
    }
    rapidjson::Document doc;
    doc.Parse(jsonText.c_str());

    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember(key.c_str()))
    {
        throw std::invalid_argument("JSON hasn't been read\n");
    }

    const rapidjson::Value& value = doc[key.c_str()];

    if (value.IsString())
    {
        return value.GetString();
    }
    else
    {
        throw std::invalid_argument("Value is not a string\n");
    }
}