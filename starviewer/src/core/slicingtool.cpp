/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "slicingtool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
//qt
#include <QTime>
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

SlicingTool::SlicingTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_slicingMode(SliceMode), m_mouseMovement(false)
{
    m_state = NONE;
    m_toolName = "SlicingTool";
    m_startPosition[0] = 0;
    m_startPosition[1] = 0;
    m_currentPosition[0] = 0;
    m_currentPosition[1] = 0;
    m_time = new QTime();
    m_latestTime = 0;
    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );
    // cada cop que canvïi l'input cal fer algunes inicialitzacions
    connect( m_2DViewer, SIGNAL(volumeChanged(Volume *) ), SLOT( inputChanged(Volume *) ) );
}

SlicingTool::~SlicingTool()
{
}

void SlicingTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::LeftButtonPressEvent:
        m_mouseMovement = false;
        this->startSlicing();
    break;

    case vtkCommand::MouseMoveEvent:
        m_mouseMovement = true;
        this->doSlicing();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        m_mouseMovement = false;
        this->endSlicing();
    break;

    case vtkCommand::MouseWheelForwardEvent:
        m_mouseMovement = false;
        m_viewer->setCursor( QCursor( QPixmap(":/images/slicing.png") ) );
        this->updateIncrement( 1 );
        m_viewer->setCursor( Qt::ArrowCursor );
    break;

    case vtkCommand::MouseWheelBackwardEvent:
        m_mouseMovement = false;
        m_viewer->setCursor( QCursor( QPixmap(":/images/slicing.png") ) );
        this->updateIncrement( -1 );
        m_viewer->setCursor( Qt::ArrowCursor );
    break;

    case vtkCommand::MiddleButtonPressEvent:
        m_mouseMovement = false;
    break;
    case vtkCommand::MiddleButtonReleaseEvent:
        if( !m_mouseMovement )
            switchSlicingMode();
    break;
    case vtkCommand::KeyPressEvent:
    {
        QString keySymbol = m_2DViewer->getInteractor()->GetKeySym();
        if( keySymbol == "Home" )
        {
            m_2DViewer->setSlice(0);
        }
        else if( keySymbol == "End" )
        {
            m_2DViewer->setSlice( m_2DViewer->getNumberOfSlices() );
        }
        else if( keySymbol == "Up" )
        {
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() + 1 );
        }
        else if( keySymbol == "Down" )
        {
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() - 1 );
        }
        else if( keySymbol == "Left" )
        {
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() - 1 );
        }
        else if( keySymbol == "Right" )
        {
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() + 1 );
        }
    }
    break;

    default:
    break;
    }
}

void SlicingTool::startSlicing()
{
    if( m_2DViewer )
    {
        m_state = SLICING;
        m_startPosition[0] = m_2DViewer->getEventPositionX();
        m_startPosition[1] = m_2DViewer->getEventPositionY();
        m_time->start();
    }
    else
        DEBUG_LOG( "::startSlicing(): El 2DViewer és NUL!" );
}

void SlicingTool::doSlicing()
{
    if( m_2DViewer )
    {
        if( m_state == SLICING )
        {
            m_viewer->setCursor( QCursor(QPixmap(":/images/slicing.png")) );
            m_currentPosition[1] = m_2DViewer->getEventPositionY();
            int dy = m_currentPosition[1] - m_startPosition[1];

            double timeElapsed = ( m_time->elapsed() - m_latestTime )/1000.0; // Es passa a segons
            double acceleracio = (dy*( 1/2300.0 ) )/( timeElapsed*timeElapsed );// 1m = 2300 px aprox.
            m_latestTime = m_time->elapsed();
            m_startPosition[1] = m_currentPosition[1];
            int value = 0;
            if( dy )
            {
                /*value = dy/abs(dy);*/
                /// Canviem un nombre de llesques segons una acceleracio
                value = (int)round(acceleracio);
                if( value == 0 )
                {
                    if( dy >= 0 ) value = 1;
                    else value = -1;
                }
            }
            this->updateIncrement( value );
        }
    }
    else
        DEBUG_LOG( "::doSlicing(): El 2DViewer és NUL!" );
}

void SlicingTool::endSlicing()
{
    if( m_2DViewer )
    {
        m_viewer->setCursor( Qt::ArrowCursor );
        m_state = NONE;
    }
    else
        DEBUG_LOG( "::endSlicing(): El 2DViewer és NUL!" );
}

void SlicingTool::inputChanged( Volume *input )
{
    m_slicingMode = SliceMode;
    m_mouseMovement = false;
}

bool SlicingTool::currentInputHasPhases()
{
    bool hasPhases = false;

    if( m_2DViewer )
    {
        if( m_2DViewer->getInput() )
        {
            if( m_2DViewer->getInput()->getNumberOfPhases() > 1 )
                hasPhases = true;
        }
        else
        {
            DEBUG_LOG("L'input del viewer és NULL!");
        }
    }
    else
    {
        DEBUG_LOG("El viewer és NULL!");
    }

    return hasPhases;
}

void SlicingTool::switchSlicingMode()
{
    if( currentInputHasPhases() )
    {
        if( m_slicingMode == SliceMode )
            m_slicingMode = PhaseMode;
        else
            m_slicingMode = SliceMode;
    }
}

void SlicingTool::updateIncrement(int increment)
{
    switch( m_slicingMode )
    {
        case SliceMode:
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() + increment );
            break;

        case PhaseMode:
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() + increment );
            break;
    }
}

}

