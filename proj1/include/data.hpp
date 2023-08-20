#ifndef DATA_H
#define DATA_H
#define LOG(msg) printf("%s:%d:%s\n", __FILE__, __LINE__, msg);
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <string>
#include <exception>

class DataProcessor
{
public:
    explicit DataProcessor() noexcept = default;
    void init();
    void addinfo(std::string const &key, std::string const &value);
    std::string Getstring();

private:
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType &allocator = doc.GetAllocator();
};

class DataParser
{
public:
    explicit DataParser(std::string const &jsonString) noexcept
        : jsonText(jsonString)
    {
    }
    std::string extractValue(std::string const &key);

private:
    std::string jsonText;
};

#endif