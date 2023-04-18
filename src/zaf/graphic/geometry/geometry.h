#pragma once

#include <memory>
#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/geometry/geometry_sink.h>

namespace zaf {

/**
 Represents a geometry resource.

 This is a base class of all concrete geometry.
 */
class Geometry : public COMObject<ID2D1Geometry>{
public:
    /**
     Specifies the different methods by which two geometries can be combined.
     */
    enum class CombineMode {

        /**
         The two regions are combined by taking the union of both.
         */
        Union = D2D1_COMBINE_MODE_UNION,

        /**
         The two regions are combined by taking their intersection.
         */
        Intersect = D2D1_COMBINE_MODE_INTERSECT,

        /**
         The two regions are combined by taking the area that exists in 
         the first region but not the second and the area that exists in 
         the second region but not the first.
         */
        Xor = D2D1_COMBINE_MODE_XOR,

        /**
         The second region is excluded from the first.
         */
        Exclude = D2D1_COMBINE_MODE_EXCLUDE,
    };

public:
    /**
     Combine two specified geometries.

     @param geometry1
        The first geometry.

     @param geometry2
        The second geometry.

     @param combine_mode
        A value indicating how to combine the two geometires.

     @param sink
        A geometry sink that stores the combine result.
     */
    static void Combine(
        const Geometry& geometry1,
        const Geometry& geometry2,
        CombineMode combine_mode,
        const GeometrySink& sink) {

        HRESULT result = geometry1.GetHandle()->CombineWithGeometry(
            geometry2.GetHandle(),
            static_cast<D2D1_COMBINE_MODE>(combine_mode),
            nullptr,
            sink.GetHandle());

        ZAF_THROW_IF_COM_ERROR(result);
    }

public:
    Geometry() { }

    /**
     Construct the instance with specified handle.

     The geometry instance takes over the lifetime of handle. It would
     release the handle when destroyed.
     */
    explicit Geometry(ID2D1Geometry* handle) : COMObject(handle) { }
};

}