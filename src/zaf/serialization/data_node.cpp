#include <zaf/serialization/data_node.h>

namespace zaf {

class DataString : public DataNode {
public:
    DataString(const std::wstring& string) : DataNode(Type::String), string_(string) {

    }

    std::wstring GetString() const override {
        return string_;
    }

    void SetString(const std::wstring& string) {
        string_ = string;
    }

private:
    std::wstring string_;
};


class DataBoolean : public DataNode {
public:
    DataBoolean(bool boolean) : DataNode(Type::Boolean), boolean_(boolean) {

    }

    bool GetBoolean() const override {
        return boolean_;
    }

    void SetBoolean(bool boolean) override {
        boolean_ = boolean;
    }

private:
    bool boolean_;
};


class DataNumber : public DataNode {
public:
    DataNumber(const Number& number) : DataNode(Type::Number), number_(number) {
    
    }

    Number GetNumber() const override {
        return number_;
    }

    void SetNumber(const Number& number) override {
        number_ = number;
    }

private:
    Number number_;
};


class DataObject : public DataNode {
public:
    DataObject() : DataNode(Type::Object) {
    
    }

    void AddChild(const std::wstring& key, const std::shared_ptr<DataNode>& data_node) override {
        fields_.emplace_back(key, data_node);
    }

    std::size_t GetChildCount() const override {
        return fields_.size();
    }

    std::shared_ptr<DataNode> GetChild(std::size_t index) const override {
        if (index >= fields_.size()) {
            return nullptr;
        }
        return fields_[index].second;
    }

    std::shared_ptr<DataNode> GetChild(const std::wstring& key) const override {
        for (const auto& each_pair : fields_) {
            if (each_pair.first == key) {
                return each_pair.second;
            }
        }
        return nullptr;
    }

    void EnumerateChildren(const ChildrenEnumerator& enumerator) const override {
        for (const auto& each_pair : fields_) {
            enumerator(*each_pair.second);
        }
    }

    void EnumerateKeyedChildren(const KeyedChildrenEnumerator& enumerator) const override {
        for (const auto& each_pair : fields_) {
            enumerator(each_pair.first, *each_pair.second);
        }
    }

private:
    std::vector<std::pair<std::wstring, std::shared_ptr<DataNode>>> fields_;
};


class DataArray : public DataNode {
public:
    DataArray() : DataNode(Type::Array) {
    
    }

    void AddChild(const std::shared_ptr<DataNode>& data_node) override {
        elements_.emplace_back(data_node);
    }

    std::size_t GetChildCount() const override {
        return elements_.size();
    }

    std::shared_ptr<DataNode> GetChild(std::size_t index) const override {
        if (index >= elements_.size()) {
            return nullptr;
        }
        return elements_[index];
    }

    void EnumerateChildren(const ChildrenEnumerator& enumerator) const override {
        for (const auto& each_element : elements_) {
            enumerator(*each_element);
        }
    }

private:
    std::vector<std::shared_ptr<DataNode>> elements_;
};


std::shared_ptr<DataNode> DataNode::CreateString(const std::wstring& value) {
    return std::make_shared<DataString>(value);
}

std::shared_ptr<DataNode> DataNode::CreateBoolean(bool value) {
    return std::make_shared<DataBoolean>(value);
}

std::shared_ptr<DataNode> DataNode::CreateNumber(const Number& number) {
    return std::make_shared<DataNumber>(number);
}

std::shared_ptr<DataNode> DataNode::CreateObject() {
    return std::make_shared<DataObject>();
}

std::shared_ptr<DataNode> DataNode::CreateArray() {
    return std::make_shared<DataArray>();
}

}