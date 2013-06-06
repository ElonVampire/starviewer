#ifndef UDGMEASUREMENTTOOL_H
#define UDGMEASUREMENTTOOL_H

#include "tool.h"

namespace udg {

class QViewer;
class Q2DViewer;
class Image;

/**
    Tool superclass for measurement tools
 */
class MeasurementTool : public Tool {
Q_OBJECT
public:
    MeasurementTool(QViewer *viewer, QObject *parent = 0);
    ~MeasurementTool();

protected:
    /// Returns the image that should be used to compute the measurements
    Image* getImageForMeasurement() const;

protected:
    /// Q2DViewer where the tool operates
    Q2DViewer *m_2DViewer;
};

} // End namespace udg

#endif
