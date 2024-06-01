#pragma once

#include <xmllite.h>
#include <zaf/base/com_ptr.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/stream.h>

namespace zaf {

class XMLWriter : NonCopyable {
public:
    explicit XMLWriter(Stream stream);

    void WriteDocumentStart();
    void WriteDocumentEnd();

    void WriteElementStart(const std::wstring& name);
    void WriteElementEnd();

    void WriteString(const std::wstring& string);
    void WriteCDATA(const std::wstring& string);

    void WriteAttribute(const std::wstring& name, const std::wstring& value);

private:
    COMPtr<IXmlWriter> inner_;
};

}