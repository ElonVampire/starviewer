/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "voxelinformationtool.h"

#include "q2dviewer.h"

//vtk
#include <vtkCaptionActor2D.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>

namespace udg {

VoxelInformationTool::VoxelInformationTool( Q2DViewer *viewer, QObject *parent )
 : /*Tool(parent),*/ m_isEnabled(false), m_voxelInformationCaption(0)
{
    m_2DViewer = viewer;
    createCaptionActor();
}

VoxelInformationTool::~VoxelInformationTool()
{
}

void VoxelInformationTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::MouseMoveEvent:
        if( m_isEnabled )
            updateVoxelInformation();
    break;

    case vtkCommand::EnterEvent:
    break;

    case vtkCommand::LeaveEvent:
        m_voxelInformationCaption->VisibilityOff();
        m_2DViewer->refresh();
    break;

    default:
    break;
    }

}

void VoxelInformationTool::enable( bool enable )
{
    m_isEnabled = enable;
    if(!enable)
        m_voxelInformationCaption->VisibilityOff();
}

void VoxelInformationTool::createCaptionActor()
{
    double xyz[3];
    m_2DViewer->getCurrentCursorPosition(xyz);

    m_voxelInformationCaption = vtkCaptionActor2D::New();
    m_voxelInformationCaption->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->SetAttachmentPoint( xyz );
    m_voxelInformationCaption->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    m_voxelInformationCaption->BorderOff();
    m_voxelInformationCaption->LeaderOff();
    m_voxelInformationCaption->ThreeDimensionalLeaderOff();
    m_voxelInformationCaption->GetProperty()->SetColor( 1.0 , 0 , 0 );
    m_voxelInformationCaption->SetPadding( 0 );
    m_voxelInformationCaption->SetPosition( -1.0 , -1.0 );
    m_voxelInformationCaption->SetHeight( 0.05 );
    m_voxelInformationCaption->SetWidth( 0.3 );
    // propietats del texte
    m_voxelInformationCaption->GetCaptionTextProperty()->SetColor( 1. , 0.7 , 0.0 );
    m_voxelInformationCaption->GetCaptionTextProperty()->ShadowOn();
    m_voxelInformationCaption->GetCaptionTextProperty()->ItalicOff();
    m_voxelInformationCaption->GetCaptionTextProperty()->BoldOff();

    // l'afegim al Q2DViewer TODO ara només es té en compte 1 sol renderer!
    m_2DViewer->getRenderer()->AddActor( m_voxelInformationCaption );
}

void VoxelInformationTool::updateVoxelInformation()
{
    double xyz[3];
    if( m_2DViewer->getCurrentCursorPosition(xyz) )
    {
        m_voxelInformationCaption->VisibilityOn();
        m_voxelInformationCaption->SetAttachmentPoint( xyz );
        m_voxelInformationCaption->SetCaption( qPrintable( QString("(%1,%2,%3):%4").arg(xyz[0],0,'f',2).arg(xyz[1],0,'f',2).arg(xyz[2],0,'f',2).arg( m_2DViewer->getCurrentImageValue() ) ) );
    }
    else
    {
        m_voxelInformationCaption->VisibilityOff();
    }
    m_2DViewer->refresh();
}

}
