#pragma once

/**
@file
    Defines the XML serialization functions.
*/

#include <zaf/base/code_page.h>
#include <zaf/io/stream/stream.h>
#include <zaf/xml/xml_input_options.h>
#include <zaf/xml/xml_output_options.h>
#include <zaf/xml/xml_serializable.h>

namespace zaf {

/**
Serializes the specified object to an XML document and writes the result to the specified stream.

@param serializable
    The object to be serialized. The WriteToXML() method of the object will be called to perform 
    the serialization.

@param stream
    The stream to which the XML document will be written.

@param options
    The options that indicate how the XML document will be written, such as the encoding.

@pre
    stream is not null.

@throw zaf::PreconditionError
    Thrown if the precondition is violated.

@throw zaf::COMError
    Thrown if serialization fails.

@throw ...
    Any exceptions thrown by the WriteToXML() mehtod of the object, or any exceptions thrown by the 
    stream when writing the result.
*/
void XMLSerialize(
    const XMLSerializable& serializable,
    Stream stream,
    const XMLOutputOptions& options = {});


/**
Serializes the specified object to an XML document and writes the result to a memory stream.

@param serializable
    The object to be serialized. The WriteToXML() method of the object will be called to perform
    the serialization.

@param options
    The options that indicate how the XML document will be written, such as the encoding.

@return
    The memory stream that contains the result.

@post
    The returned stream is not null.

@throw zaf::COMError
    Thrown if serialization fails.

@throw ...
    Any exceptions thrown by the WriteToXML() mehtod of the object, or any exceptions thrown by the
    stream when writing the result.
*/
Stream XMLSerializeToMemoryStream(
    const XMLSerializable& serializable,
    const XMLOutputOptions& options = {});


/**
Serializes the specified object to an XML document and writes the result as UTF-16 encoded text.

@param serializable
    The object to be serialized. The WriteToXML() method of the object will be called to perform
    the serialization.

@return
    The UTF-16 encoded text that contains the result.

@throw zaf::COMError
    Thrown if serialization fails.

@throw ...
    Any exceptions thrown by the WriteToXML() mehtod of the object, or any exceptions thrown when
    writing the result.
*/
std::wstring XMLSerializeToText(const XMLSerializable& serializable);


/**
Deserializes the specified object from the XML document contained in the specified stream.

@param serializable
    The object to be deserialized. The ReadFromXML() method of the object will be called to perform
    the deserialization.

@param stream
    The stream from which the XML document will be read.

@param options
    The options that indicate how the XML document will be read, such as the encoding.

@pre
    stream is not null.

@throw zaf::PreconditionError
    Thrown if the precondition is violated.

@throw zaf::COMError
    Thrown if the XML document in the stream is malformed.

@throw zaf::XMLError
    Thrown if the object cannot be deserialized from the XML document, such as if the structure of 
    the elements is incorrect, or the element names are incorrect.

@throw ...
    Any exceptions thrown by the ReadFromXML() method of the object, or any exceptions thrown when
    reading the XML document from the stream.
*/
void XMLDeserialize(
    XMLSerializable& serializable,
    const Stream& stream,
    const XMLInputOptions& options = {});


/**
Deserializes the specified object from the XML document contained in the specified UTF-16 encoded 
text.

@param serializable
    The object to be deserialized. The ReadFromXML() method of the object will be called to perform
    the deserialization.

@param text
    The UTF-16 encoded text that contains the XML document.

@throw zaf::COMError
    Thrown if the XML document in the text is malformed.

@throw zaf::XMLError
    Thrown if the object cannot be deserialized from the XML document, such as if the structure of
    the elements is incorrect, or the element names are incorrect.

@throw ...
    Any exceptions thrown by the ReadFromXML() method of the object, or any exceptions thrown when
    reading the XML document from the text.
*/
void XMLDeserialize(XMLSerializable& serializable, std::wstring_view text);

}