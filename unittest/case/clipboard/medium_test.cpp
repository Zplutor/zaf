#include <gtest/gtest.h>
#include <zaf/base/as.h>
#include <zaf/base/global_mem.h>
#include <zaf/clipboard/internal/medium_wrapper.h>
#include <zaf/clipboard/medium.h>

using namespace zaf;
using namespace zaf::clipboard;

namespace {

class Releaser : public IUnknown {
public:
    ~Releaser() {
        *is_deleted_ = true;
    }

    const std::shared_ptr<bool>& IsDeleted() const {
        return is_deleted_;
    }

    LONG ReferenceCount() const {
        return reference_count_;
    }

    HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj) override {

        if (!ppvObj) {
            return E_INVALIDARG;
        }

        if (riid == IID_IUnknown) {
            *ppvObj = this;
            AddRef();
            return S_OK;
        }

        *ppvObj = nullptr;
        return E_NOINTERFACE;
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

private:
    LONG reference_count_{ 1 };
    std::shared_ptr<bool> is_deleted_{ std::make_shared<bool>() };
};


STGMEDIUM CreateGlobalMemSTGMEDIUM() {

    STGMEDIUM result{};
    result.tymed = TYMED_HGLOBAL;
    auto global_mem = GlobalMem::Alloc(10, GlobalMemFlags::Movable);
    result.hGlobal = global_mem.Detach();
    return result;
}

}

//Make sure that the memory of lpszFileName field of STGMEDIUM is managed by 
//CoTaskMemAlloc()/CoTaskMemFree()
TEST(MediumTest, STGMEDIUM_ReleaseFile) {

    constexpr const wchar_t* TestString = L"ReleaseFile";

    STGMEDIUM medium{};
    medium.tymed = TYMED_FILE;

    auto string_length = std::wcslen(TestString) + 1;
    medium.lpszFileName =  
        reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(string_length * sizeof(wchar_t)));
    wcscpy_s(medium.lpszFileName, string_length, TestString);

    //ReleaseStgMedium() uses CoTaskMemFree() to free the memory of lpszFileName.
    //If we use a mismatched function to allocate memory for lpszFileName, an error would occur.
    ReleaseStgMedium(&medium);
}


TEST(MediumTest, DefaultConstruct) {

    Medium medium;
    ASSERT_EQ(medium.Type(), MediumType::Null);
}


TEST(MediumTest, ConstructWithSTDMEDIUM) {
    
    //Global memory without releaser.
    {
        auto inner = CreateGlobalMemSTGMEDIUM();
        Medium medium{ inner };
        ASSERT_EQ(medium.Type(), MediumType::GlobalMem);
        ASSERT_NE(medium.Inner().pUnkForRelease, nullptr);
        auto wrapper = As<zaf::clipboard::internal::MediumWrapper>(medium.Inner().pUnkForRelease);
        ASSERT_NE(wrapper, nullptr);
    }

    //Global memory with releaser.
    {
        auto inner = CreateGlobalMemSTGMEDIUM();
        inner.pUnkForRelease = new Releaser();

        Medium medium{ inner };
        ASSERT_EQ(medium.Type(), MediumType::GlobalMem);
        ASSERT_NE(medium.Inner().pUnkForRelease, nullptr);

        auto releaser = As<Releaser>(medium.Inner().pUnkForRelease);
        ASSERT_NE(releaser, nullptr);
        ASSERT_EQ(releaser->ReferenceCount(), 1);
    }
}


TEST(MediumTest, Destruct) {

    //Destruct without releaser
    {
        auto inner = CreateGlobalMemSTGMEDIUM();
        auto global_handle = inner.hGlobal;
        {
            Medium medium{ inner };
        }
        auto free_result = GlobalFree(global_handle);
        //Failure is expected. The result is equal to the passed in handle if failed.
        ASSERT_EQ(free_result, global_handle);
    }

    //Destruct with releaser.
    {
        auto releaser = new Releaser();
        auto is_deleted = releaser->IsDeleted();

        auto inner = CreateGlobalMemSTGMEDIUM();
        inner.pUnkForRelease = releaser;
        {
            Medium medium{ inner };
        }
        ASSERT_EQ(*is_deleted, true);
    }
}