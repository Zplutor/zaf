#pragma once

#include <vector>
#include <zaf/base/com_object.h>
#include <zaf/base/direct2d.h>
#include <zaf/base/error/com_error.h>
#include <zaf/base/flag_enum.h>
#include <zaf/graphic/geometry/arc_segment.h>
#include <zaf/graphic/point.h>

namespace zaf {
    
/**
 Describes a geometric path that can contain lines, arcs, cubic Bezier curves,
 and quadratic Bezier curves.
 */
class GeometrySink : public COMObject<ID2D1GeometrySink> {
public:
    /**
     Specifies how the intersecting areas of geometries or figures are combined
     to form the area of the composite geometry.
     */
    enum class FillMode {

        /**
         Determines whether a point is in the fill region by drawing a ray from
         that point to infinity in any direction, and then counting the number of
         path segments within the given shape that the ray crosses. If this number 
         is odd, the point is in the fill region; if even, the point is outside
         the fill region.
         */
        Alternate = D2D1_FILL_MODE_ALTERNATE,

        /**
         Determines whether a point is in the fill region of the path by drawing
         a ray from that point to infinity in any direction, and then examining the
         places where a segment of the shape crosses the ray. Starting with a count
         of zero, add one each time a segment crosses the ray from left to right and
         subtract one each time a path segment crosses the ray from right to left,
         as long as left and right are seen from the perspective of the ray. After 
         counting the crossings, if the result is zero, then the point is outside the 
         path. Otherwise, it is inside the path.
         */
        Winding = D2D1_FILL_MODE_WINDING,
    };

    /**
     Indicates whether a segment should be stroked and whether the join between this 
     segment and the previous one should be smooth.

     This enumeration allows a bitwise combination of its member values.
     */
    enum class SegmentFlag {

        /**
         The segment is joined as specified by the Stroke class, and it is stroked.
         */
        None = D2D1_PATH_SEGMENT_NONE,

        /**
         The segment is not stroked.
         */
        ForceUnstroked = D2D1_PATH_SEGMENT_FORCE_UNSTROKED,

        /**
         The segment is always joined with the one preceding it using a round line join,
         regardless of which Stroke::LineJoinStyle enumeration is specified by the Stroke class.
         If this segment is the first segment and the figure is closed, a round line join
         is used to connect the closing segment with the first segment. If the figure is not
         closed, this setting has no effect on the first segment of the figure. If GeometrySink::SetSegmentFlags
         is called just before GeometrySink::EndFigure, the join between the closing segment
         and the last explicitly specified segment is affected.
         */
        ForceRoundLineJoin = D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN,
    };

    /**
     Indicates whether a specific figure is filled or hollow.
     */
    enum class BeginFigureOption {

        /**
         Indicates the figure will be filled.
         */
        Fill = D2D1_FIGURE_BEGIN_FILLED,

        /**
         Indicates the figure will not be filled and will only consist of an outline.
         Moreover, the bounds of a hollow figure are zero. This value should be used 
         for stroking, or for other geometry operations.
         */
        Hollow = D2D1_FIGURE_BEGIN_HOLLOW,
    };

    /**
     Indicates whether a specific figure is open or closed.
     */
    enum class EndFigureOption {

        /**
         The figure is open.
         */
        Open = D2D1_FIGURE_END_OPEN,

        /**
         The figure is closed.
         */
        Close = D2D1_FIGURE_END_CLOSED,
    };

public:
    GeometrySink() = default;

    /**
     Construct the instance with specified handle, as well as an origin of coordinate that used to
     align geometry.

     The instance takes over the lifetime of handle. It would release the handle when destroyed.
     */
    GeometrySink(
        COMPtr<ID2D1GeometrySink> inner,
        const Point& coordinate_origin,
        const Point& aligned_coordinate_origin) 
        :
        COMObject(std::move(inner)),
        coordinate_origin_(coordinate_origin),
        aligned_coordinate_origin_(aligned_coordinate_origin) { }

    /**
     Specifies the method used to determine which points are inside the geometry
     described by this geometry sink and which points are outside.

     The fill mode defaults to FillMode::Alternate. To set the fill mode, call SetFillMode
     before the first call to BeginFigure. Not doing will put the geometry sink in an error 
     state.
     */
    void SetFillMode(FillMode fill_mode) {
        Inner()->SetFillMode(static_cast<D2D1_FILL_MODE>(fill_mode));
    }

    /**
     Specifies stroke and join options to be applied to new segments added to the geometry sink.

     After this method is called, the specified segment flags are applied to each segment subsequently
     added to the sink. The segment flags are applied to every additional segment until this method
     is called again and a different set of segment flags is specified.
     */
    void SetSegmentFlag(SegmentFlag flag) {
        Inner()->SetSegmentFlags(static_cast<D2D1_PATH_SEGMENT>(flag));
    }

    /**
     Starts a new figure at the specified point.

     @param start_position
         The point at which to begin the new figure.

     @param option
         Whether the new figure should be hollow or filled.
     */
    void BeginFigure(const Point& start_position, BeginFigureOption option);

    /**
     Ends the current figure; optionally, closes it.

     @param
         A value that indicates whether the current figure is closed. If the figure is closed,
         a line is drawn between the current point and the start point specified by BeginFigure.

     Calling this method without a matching call to BeginFigure places the geometry sink in an
     error state; subsequent calls are ignored, and the overall failure will be returned when 
     the Close method is called.
     */
    void EndFigure(EndFigureOption option) {
        Inner()->EndFigure(static_cast<D2D1_FIGURE_END>(option));
    }

    /**
     Creates a line segment between the current point and the specified end point and adds it 
     to the geometry sink.

     @param end_point
         The end point of the line to draw.   
     */
    void AddLine(const Point& end_point);

    /**
     Creates a sequence of lines using the specified points and adds them to the geometry sink.

     @param
        An array of one or more points that describe the lines to draw. A line is drawn from the
        geometry sink's current point (the end point of the last segment drawn or the location 
        specified by BeginFigure) to the first point in the array. if the array contains additional
        points, a line is drawn from the first point to the second point in the array, from the 
        second point to the third point, and so on.
     */
    void AddLines(const std::vector<Point>& points);

    void AddArc(const ArcSegment& arc_segment);

    /**
     Closes the geometry sink, indicates whether it is in an error state, and resets the sink's 
     error state.

     @param error_code
         An output parameter indicating the error, if any.

     Do not close the geometry sink while a figure is still in progress; doing so puts the geometry
     sink in an error state. For the close operation to be successful, there must be one EndFigure
     call for each call to BeginFigure.
     */
    void Close() {
        HRESULT result = Inner()->Close();
        ZAF_THROW_IF_COM_ERROR(result);
    }

private:
    Point coordinate_origin_;
    Point aligned_coordinate_origin_;
};

ZAF_ENABLE_FLAG_ENUM(GeometrySink::SegmentFlag);

}