#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <zaf/base/number.h>

namespace zaf {

class DataNode {
public:
    enum class Type {
        String,
        Boolean,
        Number,
        Object,
        Array,
    };

    typedef std::function<void(const DataNode& data_node)> ChildrenEnumerator;
    typedef std::function<void(const std::wstring& key, const DataNode& data_node)> KeyedChildrenEnumerator;

public:
    static std::shared_ptr<DataNode> CreateString(const std::wstring& value);
    static std::shared_ptr<DataNode> CreateBoolean(bool value);
    static std::shared_ptr<DataNode> CreateNumber(const Number& number);
    static std::shared_ptr<DataNode> CreateObject();
    static std::shared_ptr<DataNode> CreateArray();

public:
    virtual ~DataNode() { }

    Type GetType() const {
        return type_;
    }

    bool IsString() const {
        return type_ == Type::String;
    }

    bool IsBoolean() const {
        return type_ == Type::Boolean;
    }

    bool IsNumber() const {
        return type_ == Type::Number;
    }

    bool IsObject() const {
        return type_ == Type::Object;
    }

    bool IsArray() const {
        return type_ == Type::Array;
    }

    virtual std::wstring GetString() const { return std::wstring(); }
    virtual void SetString(const std::wstring&) { }

    virtual bool GetBoolean() const { return false; }
    virtual void SetBoolean(bool) { }

    virtual Number GetNumber() const { return Number(); }
    virtual void SetNumber(const Number& ) { }

    virtual void AddChild(const std::shared_ptr<DataNode>& data_node) { }
    virtual void AddChild(const std::wstring& key, const std::shared_ptr<DataNode>& data_node) { }

    virtual std::size_t GetChildCount() const { return 0; }
    virtual std::shared_ptr<DataNode> GetChild(std::size_t index) const { return nullptr; }
    virtual std::shared_ptr<DataNode> GetChild(const std::wstring& key) const { return nullptr; }

    virtual void EnumerateChildren(const ChildrenEnumerator& enumerator) const { }
    virtual void EnumerateKeyedChildren(const KeyedChildrenEnumerator& enumerator) const { }

    float GetFloat() const { 
        return GetNumber().GetValue<float>();
    }

    void SetFloat(float value) {
        SetNumber(value);
    }

    double GetDouble() const {
        return GetNumber().GetValue<double>();
    }

    void SetDouble(double value) {
        SetNumber(value);
    }

    std::int64_t GetInt64() const { 
        return GetNumber().GetValue<std::int64_t>();
    }

    void SetInt64(std::int64_t value) {
        SetNumber(value);
    }

    std::uint64_t GetUint64() const { 
        return GetNumber().GetValue<std::uint64_t>();
    }

    void SetUint64(std::uint64_t value) {
        SetNumber(value);
    }

    std::int32_t GetInt32() const { 
        return GetNumber().GetValue<std::int32_t>();
    }

    void SetInt32(std::int32_t value) {
        SetNumber(value);
    }

    std::uint32_t GetUint32() const { 
        return GetNumber().GetValue<std::uint32_t>();
    }

    void SetUint32(std::uint32_t value) {
        SetNumber(value);
    }

    DataNode(const DataNode&) = delete;
    DataNode& operator=(const DataNode&) = delete;

protected:
    DataNode(Type type) : type_(type) { }

private:
    Type type_;
};

}