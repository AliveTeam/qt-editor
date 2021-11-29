#include "aboutdialog.hpp"
#include "ui_aboutdialog.h"
#include "firewidget.hpp"
#include <QLayout>
#include <QTimer>
#include "relive_api.hpp"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::AboutDialog),
    mThread( nullptr )
{
    ui->setupUi(this);


    FireWidget* f = new FireWidget( this );
    this->layout()->addWidget( f );

    this->startMusic();

    this->setMaximumSize( this->size() );
    this->setMinimumSize( this->size() );

    setWindowTitle(windowTitle() + QString::fromStdString(" (Relive API v" + std::to_string(ReliveAPI::GetApiVersion()) + ")"));
}

AboutDialog::~AboutDialog()
{
    stopMusic();
    delete ui;
}

void AboutDialog::stopMusic()
{
    if ( mThread )
    {
        mThread->StopPlay();
        mThread->exit( 0 );
        mThread->wait(  );
        delete mThread;
        mThread = NULL;
    }
}

void AboutDialog::startMusic()
{
    if ( !mThread )
    {
        mThread = new ModThread( ":/about/rsc/about/tune.xm" );
        mThread->start();
    }
}
