/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "menugridwidget.h"

#include "gridicon.h"
#include "itemmenu.h"
#include "logging.h"
#include "math.h"
#include "hangingprotocol.h"
#include "hangingprotocoldisplayset.h"
#include <QFrame>
#include <QPalette>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpacerItem>
#include <QMovie>
#include <QLabel>

namespace udg {

MenuGridWidget::MenuGridWidget( QWidget *parent )
 : QWidget( parent ), m_searchingWidget(0)
{
    setWindowFlags(Qt::Popup);
    m_maxColumns = 5;

    m_gridLayout = new QGridLayout( this );
    m_gridLayout->setSpacing( 6 );
    m_gridLayout->setMargin( 6 );

    m_nextHangingProtocolRow = 0;
    m_nextHangingProtocolColumn = 0;
    m_gridLayoutHanging = 0;

    // Creem el widget amb l'animació de "searching"
    createSearchingWidget();
}

MenuGridWidget::~MenuGridWidget()
{
}

void MenuGridWidget::createHangingProtocolsWidget()
{
    ItemMenu * icon;
    int positionRow = 0;
    int positionColumn = 0;
    int numberOfHangingProtocols = m_hangingItems.size();

    if( numberOfHangingProtocols > 0 )
    {
        int hangingProtocolNumber;
        HangingProtocol * hangingProtocol;
        positionRow = 0;
        positionColumn = 0;

        m_gridLayoutHanging = new QGridLayout();
        m_gridLayoutHanging->setSpacing( 6 );
        m_gridLayoutHanging->setMargin( 6 );
        QSpacerItem * spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum); 
        m_gridLayoutHanging->addItem(spacerItem2, 0, m_maxColumns, 1, 1);

        QFrame * line_hanging = new QFrame(this);
        line_hanging->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
        line_hanging->setFrameShape(QFrame::HLine);
        line_hanging->setFrameShadow(QFrame::Sunken);
        QLabel * label_hanging = new QLabel(this);
        label_hanging->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
        label_hanging->setText("Hanging protocols");
        QHBoxLayout * hboxLayout_hanging = new QHBoxLayout();
        hboxLayout_hanging->setMargin( 0 );
        hboxLayout_hanging->setSpacing( 6 );
        hboxLayout_hanging->addWidget(line_hanging);
        hboxLayout_hanging->addWidget(label_hanging);

        m_gridLayout->addLayout( hboxLayout_hanging, 2, 0, 1, 1 );
        m_gridLayout->addLayout( m_gridLayoutHanging, 3, 0, 1, 1);

        for( hangingProtocolNumber = 0; hangingProtocolNumber < numberOfHangingProtocols; hangingProtocolNumber++)
        {
            hangingProtocol = m_hangingItems.value( hangingProtocolNumber );
            icon = createIcon( hangingProtocol );

            m_gridLayoutHanging->addWidget( icon, positionRow, positionColumn );
            m_itemList.push_back( icon );
            positionColumn ++;

            if( positionColumn == m_maxColumns )
            {
                positionColumn = 0;
                positionRow++;
            }
        }
    }
    m_nextHangingProtocolRow = positionRow;
    m_nextHangingProtocolColumn = positionColumn;

    if( m_putLoadingItem )
        addSearchingItem();
}

ItemMenu * MenuGridWidget::createIcon( const HangingProtocol * hangingProtocol )
{
    HangingProtocolDisplaySet * displaySet;
    int displaySetNumber;
    ItemMenu * icon = new ItemMenu( this );
    icon->setData( QString( tr( "%1" ).arg( hangingProtocol->getIdentifier() ) ) );
    QStringList listOfPositions;
    double x1;
    double x2;
    double y1;
    double y2;
    GridIcon* newIcon;
    QString iconType;

    icon->setGeometry( 0, 0, 64, 80 ); 
    icon->setMaximumWidth( 64 );
    icon->setMinimumWidth( 64 );
    icon->setMinimumHeight( 80 );
    icon->setMaximumHeight( 80 );
    icon->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Fixed ) );

    QLabel * sizeText = new QLabel( icon );
    sizeText->setText( hangingProtocol->getName() );
    sizeText->setAlignment( Qt::AlignHCenter );
    sizeText->setGeometry( 0, 64, 64, 80 );

    for( displaySetNumber = 1; displaySetNumber <= hangingProtocol->getNumberOfDisplaySets(); displaySetNumber++ )
    {
        displaySet = hangingProtocol->getDisplaySet( displaySetNumber );
        iconType = displaySet->getIconType();

        if( iconType.isEmpty() )
            iconType = hangingProtocol->getIconType();

        newIcon = new GridIcon( icon, iconType );

        listOfPositions = displaySet->getPosition().split("\\");
        x1 = listOfPositions.value( 0 ).toDouble();
        y1 = listOfPositions.value( 1 ).toDouble();
        x2 = listOfPositions.value( 2 ).toDouble();
        y2 = listOfPositions.value( 3 ).toDouble();

        newIcon->setGeometry( x1*64, (1-y1)*64, ((x2-x1)*64), (y1-y2)*64 );
        newIcon->show();
    }

    icon->show();
    connect( icon , SIGNAL( isSelected( ItemMenu * ) ) , this , SLOT( emitSelected( ItemMenu * ) ) );
    return icon;
}

void MenuGridWidget::emitSelected( ItemMenu * selected )
{
    hide();
    emit selectedGrid( selected->getData().toInt() );
}

void MenuGridWidget::dropContent()
{
    int i;
    ItemMenu * item;

    for( i = 0; i < m_itemList.size(); i++ )
    {
        item = m_itemList.value( i );
        m_gridLayout->removeWidget( item );
        delete item;
    }
    m_itemList.clear();
}

void MenuGridWidget::setHangingItems( QList<HangingProtocol *> listOfCandidates )
{
    m_hangingItems.clear();
    m_hangingItems = listOfCandidates;
}

void MenuGridWidget::addHangingItems( QList<HangingProtocol *> items )
{
    m_hangingItems.append( items );
}

void MenuGridWidget::setSearchingItem( bool state )
{
    m_putLoadingItem = state;

    if( state == false )
    {
        if( m_gridLayoutHanging != 0 )
        {
            m_searchingWidget->setVisible( false );
            m_gridLayoutHanging->removeWidget( m_searchingWidget );
        }
    }
}

void MenuGridWidget::addSearchingItem()
{
    // S'assumeix que el widget ha d'estar creat
    Q_ASSERT( m_searchingWidget );
    
    if( m_searchingWidget->isVisible() || (m_gridLayoutHanging == 0) )
        return;
        
    // Afegim el widget dins del layout del menú i el fem visible
    m_gridLayoutHanging->addWidget( m_searchingWidget, m_nextHangingProtocolColumn, m_nextHangingProtocolRow );
    m_searchingWidget->setVisible(true);
	
    m_loadingColumn = m_nextHangingProtocolColumn;
    m_loadingRow = m_nextHangingProtocolRow;
}

void MenuGridWidget::createSearchingWidget()
{
    if( !m_searchingWidget )
    {
        m_searchingWidget = new QWidget( this );
        m_searchingWidget->setVisible( false );
        m_searchingWidget->setGeometry ( 0, 0, 64, 64 );
        m_searchingWidget->setMaximumWidth( 64 );
        m_searchingWidget->setMinimumWidth( 64 );
        m_searchingWidget->setMinimumHeight( 64 );
        m_searchingWidget->setMaximumHeight( 64 );
        m_searchingWidget->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,QSizePolicy::Fixed ) );
        QVBoxLayout * verticalLayout = new QVBoxLayout( m_searchingWidget );

        // Construcció del label per l'animació
        QMovie * searchingMovie = new QMovie( m_searchingWidget );
        searchingMovie->setFileName(QString::fromUtf8(":/images/loader.gif"));
        QLabel * searchingLabelMovie = new QLabel( m_searchingWidget );
        searchingLabelMovie->setMovie( searchingMovie );
        searchingLabelMovie->setAlignment( Qt::AlignCenter );

        // Construcció del label pel text
        QLabel * searchingLabelText = new QLabel( m_searchingWidget );
        searchingLabelText->setText( "Searching..." );

        // Es col·loca dins al widget i a la graella per mostrar-ho
        verticalLayout->addWidget(searchingLabelMovie);
        verticalLayout->addWidget(searchingLabelText);

        searchingMovie->start();
    }
}

}
