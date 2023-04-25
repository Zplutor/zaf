#include <gtest/gtest.h>
#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/non_copyable.h>

namespace {

class BaseCOMObject : public IUnknown, zaf::NonCopyable {
public:
    BaseCOMObject() : is_deleted_(std::make_shared<bool>(false)) {

    }

    ~BaseCOMObject() {
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

        if (riid == IID_IUnknown) {
            *ppvObj = this;
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<bool> is_deleted_;
};


class DerivedCOMObject : public BaseCOMObject {
public:

};

}


TEST(COMObjectTest, TypeAssert) {

    zaf::COMObject<IUnknown> unknown;
    zaf::COMObject<BaseCOMObject> ole_object;

    //Compile will fail if uncomment below line.
    //zaf::COMObject<int> i;
}


TEST(COMObjectTest, Construction) {

    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 1);
    }

    {
        zaf::COMObject<BaseCOMObject> object(new DerivedCOMObject());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 1);
        ASSERT_NE(dynamic_cast<DerivedCOMObject*>(object.Inner()), nullptr);
    }

    //Implicit convertible from nullptr.
    {
        zaf::COMObject<BaseCOMObject> object = nullptr;
    }
}


TEST(COMObjectTest, Destruction) {

    std::shared_ptr<bool> is_deleted;
    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        is_deleted = object->IsDeleted();
    }
    ASSERT_TRUE(is_deleted);
}


TEST(COMObjectTest, CopyConstruction) {

    //Copy construct from a valid object.
    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        zaf::COMObject<BaseCOMObject> copy_object(object);

        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object.Inner(), copy_object.Inner());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 2);
    }

    //Copy construct from an invalid object.
    {
        zaf::COMObject<BaseCOMObject> object;
        zaf::COMObject<BaseCOMObject> copy_object(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(copy_object.IsValid());
    }

    //Copy construct from valid derived COMObject.
    {
        zaf::COMObject<DerivedCOMObject> derived_object(new DerivedCOMObject());
        zaf::COMObject<BaseCOMObject> base_object(derived_object);
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object.Inner(), derived_object.Inner());
        ASSERT_EQ(base_object->ReferenceCount(), 2);
    }

    //Copy construct from invalid derived COMObject.
    {
        zaf::COMObject<DerivedCOMObject> derived_object;
        zaf::COMObject<BaseCOMObject> base_object(derived_object);
        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_FALSE(base_object.IsValid());
    }

    //Implicit convert from derived COMObject.
    {
        auto function1 = [](const zaf::COMObject<BaseCOMObject>& object) {
            return !!dynamic_cast<DerivedCOMObject*>(object.Inner());
        };

        auto function2 = [](zaf::COMObject<BaseCOMObject> object) {
            return !!dynamic_cast<DerivedCOMObject*>(object.Inner());
        };

        zaf::COMObject<DerivedCOMObject> derived_object(new DerivedCOMObject());
        ASSERT_TRUE(function1(derived_object));
        ASSERT_TRUE(function2(derived_object));
    }
}


TEST(COMObjectTest, CopyAssignment) {

    //Copy assign from a valid object.
    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        zaf::COMObject<BaseCOMObject> other_object(new BaseCOMObject());
        auto is_deleted = other_object->IsDeleted();
        other_object = object;

        ASSERT_TRUE(*is_deleted);
        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object.Inner(), other_object.Inner());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 2);
    }

    //Copy assign from an invalid object.
    {
        zaf::COMObject<BaseCOMObject> object;
        zaf::COMObject<BaseCOMObject> other_object;
        other_object = object;

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }

    //Copy assign from valid derived COMObject.
    {
        zaf::COMObject<DerivedCOMObject> derived_object(new DerivedCOMObject());
        zaf::COMObject<BaseCOMObject> base_object(new BaseCOMObject());
        auto is_deleted = base_object->IsDeleted();
        base_object = derived_object;

        ASSERT_TRUE(*is_deleted);
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object.Inner(), derived_object.Inner());
        ASSERT_EQ(base_object->ReferenceCount(), 2);
    }

    //Copy assign from invalid derived COMObject.
    {
        zaf::COMObject<DerivedCOMObject> derived_object;
        zaf::COMObject<BaseCOMObject> base_object;
        base_object = derived_object;

        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_FALSE(base_object.IsValid());
    }
}


TEST(COMObjectTest, MoveConstruction) {

    //Move construct from a valid object.
    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        auto is_deleted = object->IsDeleted();
        zaf::COMObject<BaseCOMObject> other_object(std::move(object));

        ASSERT_FALSE(*is_deleted);
        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(other_object.IsValid());
        ASSERT_EQ(other_object.Inner()->ReferenceCount(), 1);
    }

    //Move construct from an invalid object.
    {
        zaf::COMObject<BaseCOMObject> object;
        zaf::COMObject<BaseCOMObject> other_object(std::move(object));

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }

    //Move construct from valid derived object.
    {
        zaf::COMObject<DerivedCOMObject> derived_object(new DerivedCOMObject());
        auto is_deleted = derived_object->IsDeleted();
        zaf::COMObject<BaseCOMObject> base_object(std::move(derived_object));

        ASSERT_FALSE(*is_deleted);
        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object.Inner()->ReferenceCount(), 1);
    }

    //Move construct from invalid derived object.
    {
        zaf::COMObject<DerivedCOMObject> derived_object;
        zaf::COMObject<BaseCOMObject> base_object(std::move(derived_object));

        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_FALSE(base_object.IsValid());
    }
}


TEST(COMObjectTest, MoveAssignment) {

    //Move assign from a valid object.
    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        zaf::COMObject<BaseCOMObject> other_object(new BaseCOMObject());
        auto is_deleted = other_object->IsDeleted();
        other_object = std::move(object);

        ASSERT_TRUE(*is_deleted);
        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(other_object.IsValid());
        ASSERT_EQ(other_object.Inner()->ReferenceCount(), 1);
    }

    //Move assign from an invalid object.
    {
        zaf::COMObject<BaseCOMObject> object;
        zaf::COMObject<BaseCOMObject> other_object;
        other_object = std::move(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }

    //Move assign from valid derived object.
    {
        zaf::COMObject<DerivedCOMObject> derived(new DerivedCOMObject());
        zaf::COMObject<BaseCOMObject> base(new BaseCOMObject());;
        auto is_deleted = base->IsDeleted();
        base = std::move(derived);

        ASSERT_TRUE(*is_deleted);
        ASSERT_FALSE(derived.IsValid());
        ASSERT_TRUE(base.IsValid());
        ASSERT_EQ(base->ReferenceCount(), 1);
    }

    //Move assign from invalid derived object.
    {
        zaf::COMObject<DerivedCOMObject> derived;
        zaf::COMObject<BaseCOMObject> base;
        base = std::move(derived);

        ASSERT_FALSE(derived.IsValid());
        ASSERT_FALSE(base.IsValid());
    }
}


TEST(COMObjectTest, OperatorBool) {

    {
        zaf::COMObject<BaseCOMObject> object;
        bool has_value{};
        if (object) {
            has_value = true;
        }
        ASSERT_FALSE(has_value);
    }

    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        bool has_value{};
        if (object) {
            has_value = true;
        }
        ASSERT_TRUE(has_value);
    }
}


TEST(COMObjectTest, CompareWithNullptr) {

    {
        zaf::COMObject<BaseCOMObject> object;
        ASSERT_TRUE(object == nullptr);
        ASSERT_FALSE(object != nullptr);
    }

    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        ASSERT_FALSE(object == nullptr);
        ASSERT_TRUE(object != nullptr);
    }
}


TEST(COMObjectTest, IsValid) {

    {
        zaf::COMObject<BaseCOMObject> object;
        ASSERT_FALSE(object.IsValid());
    }

    {
        zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
        ASSERT_TRUE(object.IsValid());
    }
}


TEST(COMObjectTest, Reset) {

    zaf::COMObject<BaseCOMObject> object;
    object.Reset();
    ASSERT_FALSE(object.IsValid());

    object.Reset(new BaseCOMObject());
    ASSERT_TRUE(object.IsValid());
    ASSERT_EQ(object->ReferenceCount(), 1);

    auto is_deleted = object->IsDeleted();
    object.Reset();
    ASSERT_FALSE(object.IsValid());
    ASSERT_TRUE(*is_deleted);
}


TEST(COMObjectTest, StoreVoid) {

    zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());

    zaf::COMObject<IUnknown> unknown;
    HRESULT hresult = object->QueryInterface(IID_IUnknown, unknown.StoreVoid());

    ASSERT_EQ(hresult, S_OK);
    ASSERT_TRUE(unknown.IsValid());
    ASSERT_EQ(object.Inner(), unknown.Inner());
    ASSERT_EQ(object->ReferenceCount(), 2);
}


TEST(COMObjecTest, OperatorArrow) {

    zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());
    ASSERT_EQ(object->ReferenceCount(), 1);
}


TEST(COMObjectTest, Query) {

    zaf::COMObject<BaseCOMObject> object(new BaseCOMObject());

    auto unknown = object.Query<IUnknown>();
    ASSERT_TRUE(unknown.IsValid());
    ASSERT_EQ(object.Inner(), unknown.Inner());

    auto ole_object = object.Query<IOleObject>();
    ASSERT_FALSE(ole_object.IsValid());
}


TEST(COMObjectTest, As) {

    zaf::COMObject<BaseCOMObject> base(new DerivedCOMObject());

    auto derived = base.As<DerivedCOMObject>();
    ASSERT_TRUE(derived.IsValid());
    ASSERT_EQ(derived.Inner(), base.Inner());
    ASSERT_EQ(derived->ReferenceCount(), 2);
}