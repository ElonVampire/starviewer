/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qlocaldatabaseconfigurationscreen.h"

#include <QIntValidator>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

#include "localdatabasemanager.h"
#include "starviewerapplication.h"
#include "starviewersettings.h"
#include "logging.h"

namespace udg {

QLocalDatabaseConfigurationScreen::QLocalDatabaseConfigurationScreen( QWidget *parent ) : QWidget(parent)
{
    setupUi( this );

    loadCacheDefaults();
    m_buttonApplyCache->setEnabled(false);

    m_configurationChanged = false;
    m_createDatabase = false;

    createConnections();
    m_buttonApplyCache->setIcon( QIcon( ":images/apply.png" ) );
    configureInputValidator();
}

QLocalDatabaseConfigurationScreen::~QLocalDatabaseConfigurationScreen()
{
}

void QLocalDatabaseConfigurationScreen::createConnections()
{
    //connecta el boto examinar de la cache amb el dialog per escollir el path de la base de dades
    connect( m_buttonExaminateDataBase , SIGNAL( clicked() ), SLOT( examinateDataBaseRoot() ) );

    //connecta el boto examinar de la cache amb el dialog per escollir el path de la base de dades
    connect( m_buttonExaminateCacheImage , SIGNAL( clicked() ), SLOT( examinateCacheImagePath() ) );

    //connecta el boto aplicar de la cache amb l'slot apply
    connect( m_buttonApplyCache , SIGNAL( clicked() ),  SLOT( applyChanges() ) );

    //connecta el boto aplicar de l'informació de l'institució amb l'slot apply
    connect( m_buttonCreateDatabase , SIGNAL( clicked() ),  SLOT( createDatabase() ) );

    //activen el boto apply quant canvia el seu valor
    connect( m_textDatabaseRoot, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textCacheImagePath, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textMinimumSpaceRequiredToRetrieve, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );
    connect( m_textDatabaseRoot, SIGNAL( textChanged(const QString &) ), SLOT ( configurationChangedDatabaseRoot() ) );
    connect( m_textMaximumDaysNotViewed, SIGNAL( textChanged(const QString &) ), SLOT( enableApplyButtons() ) );

    //mateniment base de dades
    connect( m_buttonDeleteStudies , SIGNAL( clicked() ), SLOT( deleteStudies() ) );
    connect( m_buttonCompactDatabase , SIGNAL( clicked() ), SLOT( compactCache() ) );

    //afegeix la / al final del Path de la cache d'imatges
    connect( m_textCacheImagePath , SIGNAL( editingFinished() ), SLOT( cacheImagePathEditingFinish() ) );
}

void QLocalDatabaseConfigurationScreen::configureInputValidator()
{
    m_textMaximumDaysNotViewed->setValidator( new QIntValidator(0, 9999, m_textMaximumDaysNotViewed) );
    m_textMinimumSpaceRequiredToRetrieve->setValidator( new QIntValidator(0, 999, m_textMinimumSpaceRequiredToRetrieve) );
}

void QLocalDatabaseConfigurationScreen::loadCacheDefaults()
{
    StarviewerSettings settings;

    m_textDatabaseRoot->setText(settings.getDatabasePath());
    m_textCacheImagePath->setText(settings.getCacheImagePath());
    m_textMaximumDaysNotViewed->setText(settings.getMaximumDaysNotViewedStudy());
    m_textMinimumSpaceRequiredToRetrieve->setText(QString().setNum(settings.getMinimumSpaceRequiredToRetrieveInGbytes()));
}

bool QLocalDatabaseConfigurationScreen::validateChanges()
{
    QDir dir;

    if ( m_textDatabaseRoot->isModified() )
    {
        if ( !dir.exists(m_textDatabaseRoot->text() ) && m_createDatabase == false ) // si el fitxer indicat no existeix i no s'ha demanat que es crei una nova base de dades, el path és invàlid
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "Invalid database path" ) );
            return false;
        }
    }

    if ( m_textCacheImagePath->isModified() )
    {
        if ( !dir.exists(m_textCacheImagePath->text() ) )
        {
            switch ( QMessageBox::question( this ,
                    tr( "Create directory ?" ) ,
                    tr( "The cache image directory doesn't exists. Do you want to create it ?" ) ,
                    tr( "&Yes" ) , tr( "&No" ) , 0 , 1 ) )
            {
                case 0:
                    if ( !dir.mkpath( m_textCacheImagePath->text() ) )
                    {
                        QMessageBox::critical( this , ApplicationNameString , tr( "Can't create the directory. Please check users permission" ) );
                        return false;
                    }
                    else return true;
                    break;
                case 1:
                    return false;
                    break;
            }
        }
    }

    if (m_textMinimumSpaceRequiredToRetrieve->isModified())
    {
        if (m_textMinimumSpaceRequiredToRetrieve->text().toUInt() < 1)
        {
            QMessageBox::warning(this, ApplicationNameString, tr("At least 1 GByte of free space is necessary."));
            return false;
        }
    }

    return true;
}

bool QLocalDatabaseConfigurationScreen::applyChanges()
{
    if (validateChanges())
    {
        applyChangesCache();

        if ( m_textDatabaseRoot->isModified() && m_createDatabase == false ) // només s'ha de reiniciar en el cas que que s'hagi canviat el path de la base de dades, per una ja existent. En el cas que la base de dades no existeixi, a l'usuari al fer click al botó crear base de dades, ja se li haurà informat que s'havia de reiniciar l'aplicació
        {
            QMessageBox::warning( this , ApplicationNameString , tr( "The application has to be restarted to apply the changes" ) );
        }

        m_configurationChanged = false;

        return true;
    }
    else return false;
}

void QLocalDatabaseConfigurationScreen::enableApplyButtons()
{
    m_buttonApplyCache->setEnabled( true );
    m_configurationChanged = true;
}

void QLocalDatabaseConfigurationScreen::configurationChangedDatabaseRoot()
{
    m_createDatabase= false; //indiquem no s'ha demanat que es creï la base de dades indicada a m_textDatabaseRoot
    enableApplyButtons();
}

void QLocalDatabaseConfigurationScreen::examinateDataBaseRoot()
{
    //a la pàgina de QT indica que en el cas que nomes deixem seleccionar un fitxer, agafar el primer element de la llista i punt, no hi ha cap mètode que te retornin directament el fitxer selccionat
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./" , ApplicationNameString + " Database (*.sdb)" );
    dlg->setFileMode( QFileDialog::ExistingFile );

    if ( dlg->exec() == QDialog::Accepted )
    {
        if ( !dlg->selectedFiles().empty() )
        {
            m_textDatabaseRoot->setText( dlg->selectedFiles().takeFirst() );
            m_textDatabaseRoot->setModified( true );// indiquem que m_textDatabaseRoot ha modificat el seu valor
        }
    }

    delete dlg;
}

void QLocalDatabaseConfigurationScreen::examinateCacheImagePath()
{
    QFileDialog *dlg = new QFileDialog( 0 , QFileDialog::tr( "Open" ) , "./" , tr( "Database Directory" ) );
    QString path;

    dlg->setFileMode( QFileDialog::DirectoryOnly );

    if ( dlg->exec() == QDialog::Accepted )
    {
        if ( !dlg->selectedFiles().empty() ) m_textCacheImagePath->setText( dlg->selectedFiles().takeFirst() );
        cacheImagePathEditingFinish();//afegeix la '/' al final
    }

    delete dlg;
}

void QLocalDatabaseConfigurationScreen::applyChangesCache()
{
    StarviewerSettings settings;

    //Aquest els guardem sempre
    settings.setCacheImagePath( m_textCacheImagePath->text() );
    settings.setDatabasePath( m_textDatabaseRoot->text() );

    if ( m_textMinimumSpaceRequiredToRetrieve->isModified() )
    {
        INFO_LOG( "Es modificarà l'espai mínim necessari per descarregar" + m_textMinimumSpaceRequiredToRetrieve->text() );
        settings.setMinimumSpaceRequiredToRetrieveInGbytes(m_textMinimumSpaceRequiredToRetrieve->text().toUInt());
    }

    if ( m_textCacheImagePath->isModified() )
    {
        INFO_LOG( "Es modificarà el directori de la cache d'imatges " + m_textCacheImagePath->text() );
        settings.setCacheImagePath( m_textCacheImagePath->text() );
    }

    if ( m_textMaximumDaysNotViewed->isModified() )
    {
        INFO_LOG( "Es modificarà el nombre maxim de dies d'un estudi a la cache" + m_textMaximumDaysNotViewed->text() );
        settings.setMaximumDaysNotViewedStudy( m_textMaximumDaysNotViewed->text() );
    }

    m_buttonApplyCache->setEnabled( false );
}

void QLocalDatabaseConfigurationScreen::deleteStudies()
{
    QMessageBox::StandardButton response = QMessageBox::question(this, ApplicationNameString,
                                                                       tr("Are you sure you want to delete all Studies of the cache ?"),
                                                                       QMessageBox::Yes | QMessageBox::No,
                                                                       QMessageBox::No);
    if(response == QMessageBox::Yes)
    {
        INFO_LOG( "Neteja de la cache" );

        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        LocalDatabaseManager localDatabaseManager;
        localDatabaseManager.clear();

        QApplication::restoreOverrideCursor();

        if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok )
        {
            Status state;
            state.setStatus(tr("The cache cannot be deleted, an unknown error has ocurred."
                               "\nTry to close all %1 windows and try again."
                               "\n\nIf the problem persist contact with an administrator.").arg(ApplicationNameString), false, -1);
            showDatabaseErrorMessage( state );
        }

        emit configurationChanged("Pacs/CacheCleared");
    }
}

void QLocalDatabaseConfigurationScreen::compactCache()
{
    INFO_LOG( "Compactació de la base de dades" );

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    LocalDatabaseManager localDatabaseManager;
    localDatabaseManager.compact();

    QApplication::restoreOverrideCursor();

    if (localDatabaseManager.getLastError() != LocalDatabaseManager::Ok )
    {
        Status state;
        state.setStatus(tr("The database cannot be compacted, an unknown error has ocurred."
                "\n Try to close all %1 windows and try again."
                "\n\nIf the problem persist contact with an administrator.").arg(ApplicationNameString), false, -1);
        showDatabaseErrorMessage( state );
    }
}

void QLocalDatabaseConfigurationScreen::cacheImagePathEditingFinish()
{
    if ( !m_textCacheImagePath->text().endsWith( QDir::toNativeSeparators( "/" ) , Qt::CaseInsensitive ) )
    {
        m_textCacheImagePath->setText( m_textCacheImagePath->text() + QDir::toNativeSeparators( "/" ) );
    }
}

void QLocalDatabaseConfigurationScreen::createDatabase()
{
    StarviewerSettings settings;
    QFile databaseFile;
    QString stringDatabasePath;

    if ( m_textDatabaseRoot->text().right(4) != ".sdb" )
    {
        QMessageBox::warning( this , ApplicationNameString , tr( "The extension of the database has to be '.sdb'" ) );
    }
    else
    {
        if ( databaseFile.exists( m_textDatabaseRoot->text() ) )
        {
            QMessageBox::warning( this , ApplicationNameString , tr ( "%1 can't create the database because, a database with the same name exists in the directory" ).arg(ApplicationNameString) );
        }
        else
        {
            settings.setDatabasePath( m_textDatabaseRoot->text() );
            QMessageBox::warning( this , ApplicationNameString , tr( "The application has to be restarted to apply the changes"  ));
            m_createDatabase = true;
        }
    }
}

void QLocalDatabaseConfigurationScreen::showDatabaseErrorMessage( const Status &state )
{
    if( !state.good() )
    {
        QMessageBox::critical( this , ApplicationNameString , state.text() + tr("\nError Number: %1").arg(state.code() ) );
    }
}

};
