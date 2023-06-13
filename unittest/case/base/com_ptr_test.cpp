#include <gtest/gtest.h>
#include <memory>
#include <zaf/base/com_ptr.h>
#include <zaf/base/non_copyable.h>

namespace {

class BaseObject : public IUnknown, zaf::NonCopyableNonMovable {
public:
    BaseObject() : is_deleted_(std::make_shared<bool>(false)) {

    }

    ~BaseObject() {
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


class DerivedObject : public BaseObject {
public:

};

}


TEST(COMPtrTest, TypeAssert) {

    zaf::COMPtr<IUnknown> unknown;
    zaf::COMPtr<BaseObject> ole_object;

    //Compile will fail if uncomment below line.
    //zaf::COMPtr<int> i;
}


TEST(COMPtrTest, Construction) {

    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 1);
    }

    {
        zaf::COMPtr<BaseObject> object(new DerivedObject());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 1);
        ASSERT_NE(dynamic_cast<DerivedObject*>(object.Inner()), nullptr);
    }

    //Implicit convertible from nullptr.
    {
        zaf::COMPtr<BaseObject> object = nullptr;
    }
}


TEST(COMPtrTest, Destruction) {

    std::shared_ptr<bool> is_deleted;
    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        is_deleted = object->IsDeleted();
    }
    ASSERT_TRUE(is_deleted);
}


TEST(COMPtrTest, CopyConstruction) {

    //Copy construct from a valid object.
    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        zaf::COMPtr<BaseObject> copy_object(object);

        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object.Inner(), copy_object.Inner());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 2);
    }

    //Copy construct from an invalid object.
    {
        zaf::COMPtr<BaseObject> object;
        zaf::COMPtr<BaseObject> copy_object(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(copy_object.IsValid());
    }

    //Copy construct from valid derived COMPtr.
    {
        zaf::COMPtr<DerivedObject> derived_object(new DerivedObject());
        zaf::COMPtr<BaseObject> base_object(derived_object);
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object.Inner(), derived_object.Inner());
        ASSERT_EQ(base_object->ReferenceCount(), 2);
    }

    //Copy construct from invalid derived COMPtr.
    {
        zaf::COMPtr<DerivedObject> derived_object;
        zaf::COMPtr<BaseObject> base_object(derived_object);
        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_FALSE(base_object.IsValid());
    }

    //Implicit convert from derived COMPtr.
    {
        auto function1 = [](const zaf::COMPtr<BaseObject>& object) {
            return !!dynamic_cast<DerivedObject*>(object.Inner());
        };

        auto function2 = [](zaf::COMPtr<BaseObject> object) {
            return !!dynamic_cast<DerivedObject*>(object.Inner());
        };

        zaf::COMPtr<DerivedObject> derived_object(new DerivedObject());
        ASSERT_TRUE(function1(derived_object));
        ASSERT_TRUE(function2(derived_object));
    }
}


TEST(COMPtrTest, CopyAssignment) {

    //Copy assign from a valid object.
    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        zaf::COMPtr<BaseObject> other_object(new BaseObject());
        auto is_deleted = other_object->IsDeleted();
        other_object = object;

        ASSERT_TRUE(*is_deleted);
        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object.Inner(), other_object.Inner());
        ASSERT_EQ(object.Inner()->ReferenceCount(), 2);
    }

    //Copy assign from an invalid object.
    {
        zaf::COMPtr<BaseObject> object;
        zaf::COMPtr<BaseObject> other_object;
        other_object = object;

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }

    //Copy assign from valid derived COMPtr.
    {
        zaf::COMPtr<DerivedObject> derived_object(new DerivedObject());
        zaf::COMPtr<BaseObject> base_object(new BaseObject());
        auto is_deleted = base_object->IsDeleted();
        base_object = derived_object;

        ASSERT_TRUE(*is_deleted);
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object.Inner(), derived_object.Inner());
        ASSERT_EQ(base_object->ReferenceCount(), 2);
    }

    //Copy assign from invalid derived COMPtr.
    {
        zaf::COMPtr<DerivedObject> derived_object;
        zaf::COMPtr<BaseObject> base_object;
        base_object = derived_object;

        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_FALSE(base_object.IsValid());
    }

    //Self copy assignment.
    {
        zaf::COMPtr<BaseObject> object{ new BaseObject() };
        object = object;
        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object->ReferenceCount(), 1);
    }

    //Indirect self copy assignment.
    {
        zaf::COMPtr<DerivedObject> derived_object{ new DerivedObject() };
        zaf::COMPtr<BaseObject> base_object{ derived_object };

        base_object = derived_object;
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object->ReferenceCount(), 2);

        ASSERT_TRUE(derived_object.IsValid());
        ASSERT_EQ(derived_object->ReferenceCount(), 2);
    }
}


TEST(COMPtrTest, MoveConstruction) {

    //Move construct from a valid object.
    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        auto is_deleted = object->IsDeleted();
        zaf::COMPtr<BaseObject> other_object(std::move(object));

        ASSERT_FALSE(*is_deleted);
        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(other_object.IsValid());
        ASSERT_EQ(other_object.Inner()->ReferenceCount(), 1);
    }

    //Move construct from an invalid object.
    {
        zaf::COMPtr<BaseObject> object;
        zaf::COMPtr<BaseObject> other_object(std::move(object));

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }

    //Move construct from valid derived object.
    {
        zaf::COMPtr<DerivedObject> derived_object(new DerivedObject());
        auto is_deleted = derived_object->IsDeleted();
        zaf::COMPtr<BaseObject> base_object(std::move(derived_object));

        ASSERT_FALSE(*is_deleted);
        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object.Inner()->ReferenceCount(), 1);
    }

    //Move construct from invalid derived object.
    {
        zaf::COMPtr<DerivedObject> derived_object;
        zaf::COMPtr<BaseObject> base_object(std::move(derived_object));

        ASSERT_FALSE(derived_object.IsValid());
        ASSERT_FALSE(base_object.IsValid());
    }
}


TEST(COMPtrTest, MoveAssignment) {

    //Move assign from a valid object.
    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        zaf::COMPtr<BaseObject> other_object(new BaseObject());
        auto is_deleted = other_object->IsDeleted();
        other_object = std::move(object);

        ASSERT_TRUE(*is_deleted);
        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(other_object.IsValid());
        ASSERT_EQ(other_object.Inner()->ReferenceCount(), 1);
    }

    //Move assign from an invalid object.
    {
        zaf::COMPtr<BaseObject> object;
        zaf::COMPtr<BaseObject> other_object;
        other_object = std::move(object);

        ASSERT_FALSE(object.IsValid());
        ASSERT_FALSE(other_object.IsValid());
    }

    //Move assign from valid derived object.
    {
        zaf::COMPtr<DerivedObject> derived(new DerivedObject());
        zaf::COMPtr<BaseObject> base(new BaseObject());;
        auto is_deleted = base->IsDeleted();
        base = std::move(derived);

        ASSERT_TRUE(*is_deleted);
        ASSERT_FALSE(derived.IsValid());
        ASSERT_TRUE(base.IsValid());
        ASSERT_EQ(base->ReferenceCount(), 1);
    }

    //Move assign from invalid derived object.
    {
        zaf::COMPtr<DerivedObject> derived;
        zaf::COMPtr<BaseObject> base;
        base = std::move(derived);

        ASSERT_FALSE(derived.IsValid());
        ASSERT_FALSE(base.IsValid());
    }

    //Self move assignment.
    {
        zaf::COMPtr<BaseObject> object{ new BaseObject() };
        object = std::move(object);
        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object->ReferenceCount(), 1);
    }

    //Indirect self move assignment.
    {
        zaf::COMPtr<DerivedObject> derived_object{ new DerivedObject() };
        zaf::COMPtr<BaseObject> base_object{ derived_object };

        base_object = std::move(derived_object);
        ASSERT_TRUE(base_object.IsValid());
        ASSERT_EQ(base_object->ReferenceCount(), 1);

        ASSERT_FALSE(derived_object.IsValid());
    }
}


TEST(COMPtrTest, OperatorBool) {

    {
        zaf::COMPtr<BaseObject> object;
        bool has_value{};
        if (object) {
            has_value = true;
        }
        ASSERT_FALSE(has_value);
    }

    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        bool has_value{};
        if (object) {
            has_value = true;
        }
        ASSERT_TRUE(has_value);
    }
}


TEST(COMPtrTest, CompareWithNullptr) {

    {
        zaf::COMPtr<BaseObject> object;
        ASSERT_TRUE(object == nullptr);
        ASSERT_FALSE(object != nullptr);
    }

    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        ASSERT_FALSE(object == nullptr);
        ASSERT_TRUE(object != nullptr);
    }
}


TEST(COMPtrTest, IsValid) {

    {
        zaf::COMPtr<BaseObject> object;
        ASSERT_FALSE(object.IsValid());
    }

    {
        zaf::COMPtr<BaseObject> object(new BaseObject());
        ASSERT_TRUE(object.IsValid());
    }
}


TEST(COMPtrTest, Reset) {

    {
        zaf::COMPtr<BaseObject> object;
        object.Reset();
        ASSERT_FALSE(object.IsValid());
    }

    {
        zaf::COMPtr<BaseObject> object;
        object.Reset(new BaseObject());
        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object->ReferenceCount(), 1);
    }

    {
        zaf::COMPtr<BaseObject> object{ new BaseObject() };
        auto is_deleted = object->IsDeleted();
        object.Reset();
        ASSERT_FALSE(object.IsValid());
        ASSERT_TRUE(*is_deleted);
    }

    //Reset to itself.
    {
        zaf::COMPtr<BaseObject> object{ new BaseObject() };
        object.Reset(object.Inner());

        ASSERT_TRUE(object.IsValid());
        ASSERT_EQ(object->ReferenceCount(), 1);
    }
}


TEST(COMPtrTest, ResetAsVoid) {

    zaf::COMPtr<BaseObject> object(new BaseObject());

    zaf::COMPtr<IUnknown> unknown;
    HRESULT hresult = object->QueryInterface(IID_IUnknown, unknown.ResetAsVoid());

    ASSERT_EQ(hresult, S_OK);
    ASSERT_TRUE(unknown.IsValid());
    ASSERT_EQ(object.Inner(), unknown.Inner());
    ASSERT_EQ(object->ReferenceCount(), 2);
}


TEST(COMObjecTest, OperatorArrow) {

    zaf::COMPtr<BaseObject> object(new BaseObject());
    ASSERT_EQ(object->ReferenceCount(), 1);
}


TEST(COMPtrTest, Query) {

    zaf::COMPtr<BaseObject> object(new BaseObject());

    auto unknown = object.Query<IUnknown>();
    ASSERT_TRUE(unknown.IsValid());
    ASSERT_EQ(object.Inner(), unknown.Inner());

    auto ole_object = object.Query<IOleObject>();
    ASSERT_FALSE(ole_object.IsValid());
}


TEST(COMPtrTest, As) {

    zaf::COMPtr<BaseObject> base(new DerivedObject());

    auto derived = base.As<DerivedObject>();
    ASSERT_TRUE(derived.IsValid());
    ASSERT_EQ(derived.Inner(), base.Inner());
    ASSERT_EQ(derived->ReferenceCount(), 2);
}