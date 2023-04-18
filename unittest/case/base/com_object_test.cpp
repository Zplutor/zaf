#include <gtest/gtest.h>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/non_copyable.h>

namespace {

class TestCOMObject : public IUnknown, zaf::NonCopyable {
public:
    TestCOMObject() : is_deleted_(std::make_shared<bool>(false)) {

    }

    ~TestCOMObject() {
        *is_deleted_ = true;
    }

    LONG ReferenceCount() const {
        return reference_count_;
    }

    const std::shared_ptr<bool>& IsDeleted() const {
        return is_deleted_;
    }

    ULONG AddRef() override {
        return InterlockedIncrement(&reference_count_);
    }

    ULONG Release() override {
        ULONG result = InterlockedDecrement(&reference_count_);
        if (result == 0) {
            delete this;
        }
        return result;
    }

    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override {
        return E_NOINTERFACE;
    }

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<bool> is_deleted_;
};

}


TEST(COMObjectTest, TypeAssert) {

    zaf::COMObject<IUnknown> unknown;
    zaf::COMObject<TestCOMObject> ole_object;

    //Compile will fail if uncomment below line.
    //zaf::COMObject<int> i;
}


TEST(COMObjectTest, ConstructionWithValue) {

    zaf::COMObject<TestCOMObject> object(new TestCOMObject());
    ASSERT_EQ(object.Inner()->ReferenceCount(), 1);
}


TEST(COMObjectTest, CopyConstruction) {

    //Copy construct from a valid object.
    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        zaf::COMObject<TestCOMObject> copy_object(object);

        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object.Inner(), copy_object.Inner());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 2);
    }

    //Copy construct from an invalid object.
    {
        zaf::COMObject<TestCOMObject> object;
        zaf::COMObject<TestCOMObject> copy_object(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(copy_object.IsValid());
    }
}


TEST(COMObjectTest, CopyAssignment) {

    //Copy assign from a valid object.
    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        zaf::COMObject<TestCOMObject> copy_object;
        copy_object = object;

        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object.Inner(), copy_object.Inner());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 2);
    }

    //Copy assign from an invalid object.
    {
        zaf::COMObject<TestCOMObject> object;
        zaf::COMObject<TestCOMObject> copy_object;
        copy_object = object;

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(copy_object.IsValid());
    }
}


TEST(COMObjectTest, MoveConstruction) {

    //Move construct from a valid object.
    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        zaf::COMObject<TestCOMObject> other_object(std::move(object));

        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(other_object.IsValid());
        ASSERT_EQ(other_object.Inner()->ReferenceCount(), 1);
    }

    //Move construct from an invalid object.
    {
        zaf::COMObject<TestCOMObject> object;
        zaf::COMObject<TestCOMObject> other_object(std::move(object));

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }
}


TEST(COMObjectTest, MoveAssignment) {

    //Move assign from a valid object.
    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        zaf::COMObject<TestCOMObject> other_object;
        other_object = std::move(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(other_object.IsValid());
        ASSERT_EQ(other_object.Inner()->ReferenceCount(), 1);
    }

    //Move assign from an invalid object.
    {
        zaf::COMObject<TestCOMObject> object;
        zaf::COMObject<TestCOMObject> other_object;
        other_object = std::move(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }
}


TEST(COMObjectTest, OperatorBool) {

    {
        zaf::COMObject<TestCOMObject> object;
        bool has_value{};
        if (object) {
            has_value = true;
        }
        ASSERT_FALSE(has_value);
    }

    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        bool has_value{};
        if (object) {
            has_value = true;
        }
        ASSERT_TRUE(has_value);
    }
}


TEST(COMObjectTest, CompareWithNullptr) {

    {
        zaf::COMObject<TestCOMObject> object;
        ASSERT_TRUE(object == nullptr);
        ASSERT_FALSE(object != nullptr);
    }

    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        ASSERT_FALSE(object == nullptr);
        ASSERT_TRUE(object != nullptr);
    }
}


TEST(COMObjectTest, IsValid) {

    {
        zaf::COMObject<TestCOMObject> object;
        ASSERT_FALSE(object.IsValid());
    }

    {
        zaf::COMObject<TestCOMObject> object(new TestCOMObject());
        ASSERT_TRUE(object.IsValid());
    }
}


TEST(COMObjectTest, Reset) {

    zaf::COMObject<TestCOMObject> object;
    object.Reset();
    ASSERT_FALSE(object.IsValid());

    object.Reset(new TestCOMObject());
    ASSERT_TRUE(object.IsValid());
    ASSERT_EQ(object->ReferenceCount(), 1);

    auto is_deleted = object->IsDeleted();
    object.Reset();
    ASSERT_FALSE(object.IsValid());
    ASSERT_TRUE(*is_deleted);
}


TEST(COMObjecTest, OperatorArrow) {

    zaf::COMObject<TestCOMObject> object(new TestCOMObject());
    ASSERT_EQ(object->ReferenceCount(), 1);
}