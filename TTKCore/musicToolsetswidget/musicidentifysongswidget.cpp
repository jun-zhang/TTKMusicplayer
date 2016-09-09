#include "musicidentifysongswidget.h"
#include "musictoolsetsuiobject.h"
#include "musicuiobject.h"
#include "musicnumberdefine.h"
#include "musicidentifysongsthread.h"
#include "musicaudiorecordercore.h"

#include <QMovie>
#include <QTimer>
#include <QShortcut>
#include <QBoxLayout>
#include <QStackedWidget>

MusicIdentifySongsWidget::MusicIdentifySongsWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    m_mainWindow = new QStackedWidget(this);
    m_mainWindow->setObjectName("MainWindow");
    m_mainWindow->setStyleSheet(QString("#MainWindow{%1}").arg(MusicUIObject::MBackgroundStyle17));
    layout->addWidget(m_mainWindow);
    setLayout(layout);

    m_timer = new QTimer(this);
    m_timer->setInterval(10*MT_S2MS);
    connect(m_timer, SIGNAL(timeout()), SLOT(detectedTimeOut()));

    m_recordCore = new MusicAudioRecorderCore(this);
    m_detectedThread = new MusicIdentifySongsThread(this);

    QShortcut *cut = new QShortcut(Qt::SHIFT + Qt::CTRL + Qt::Key_T, this);
    connect(cut, SIGNAL(activated()), SLOT(detectedButtonClicked()));

    createDetectedWidget();

    m_detectedButton->setEnabled(false);
    getKey();
}

MusicIdentifySongsWidget::~MusicIdentifySongsWidget()
{
    delete m_timer;
    delete m_recordCore;
    delete m_detectedThread;
    delete m_detectedButton;
    delete m_detectedLabel;
    delete m_detectedMovie;
    delete m_mainWindow;
}

QString MusicIdentifySongsWidget::getClassName()
{
    return staticMetaObject.className();
}

void MusicIdentifySongsWidget::getKey()
{
    if(m_detectedThread->getKey())
    {
        m_detectedButton->setEnabled(true);
    }
}

void MusicIdentifySongsWidget::detectedButtonClicked()
{
    if(m_detectedButton->styleSheet().contains(MusicUIObject::MKGSongsDetectStartBtn))
    {
        m_recordCore->onRecordStart();
        m_detectedMovie->start();
        m_timer->start();
        m_detectedButton->setStyleSheet(MusicUIObject::MKGSongsDetectStopBtn);
        m_detectedLabel->setText(tr("Recogniting the music being played"));
    }
    else
    {
        m_recordCore->onRecordStop();
        m_detectedMovie->stop();
        m_timer->stop();
        m_detectedButton->setStyleSheet(MusicUIObject::MKGSongsDetectStartBtn);
        m_detectedLabel->setText(tr("Intelligent Recognition Of The Music Being Played"));
    }
}

void MusicIdentifySongsWidget::reDetectButtonClicked()
{
    m_mainWindow->setCurrentIndex(0);
}

void MusicIdentifySongsWidget::detectedTimeOut()
{
    m_recordCore->addWavHeader(RECORD_IN_FILE);

    QEventLoop loop;
    m_detectedThread->query("21.mp3");
    connect(m_detectedThread, SIGNAL(downLoadDataChanged(QString)), &loop, SLOT(quit()));
    loop.exec();

    detectedButtonClicked();
    if(m_detectedThread->getIdentifySongs().isEmpty())
    {
        createDetectedFailedWidget();
    }
    else
    {
        createDetectedSuccessedWidget();
    }
}

void MusicIdentifySongsWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
}

void MusicIdentifySongsWidget::createDetectedWidget()
{
    QWidget *widget = new QWidget(m_mainWindow);
    widget->setStyleSheet(MusicUIObject::MColorStyle03 + MusicUIObject::MCustomStyle04);
    QVBoxLayout *widgetLayout = new QVBoxLayout(widget);

    m_detectedMovie = new QMovie(":/toolSets/lb_radar", QByteArray(), widget);
    QLabel *iconLabel = new QLabel(widget);
    iconLabel->setFixedSize(200, 200);
    QVBoxLayout *iconLabelLayout = new QVBoxLayout(iconLabel);
    iconLabelLayout->setSpacing(0);
    iconLabelLayout->setContentsMargins(0, 0, 0, 0);
    m_detectedButton = new QPushButton(widget);
    m_detectedButton->setStyleSheet(MusicUIObject::MKGSongsDetectStartBtn);
    m_detectedButton->setCursor(QCursor(Qt::PointingHandCursor));
    m_detectedButton->setFixedSize(162, 162);
    iconLabelLayout->addWidget(m_detectedButton, 0, Qt::AlignCenter);
    iconLabel->setMovie(m_detectedMovie);
    iconLabel->setLayout(iconLabelLayout);
    connect(m_detectedButton, SIGNAL(clicked()), SLOT(detectedButtonClicked()));

    m_detectedLabel = new QLabel(widget);
    m_detectedLabel->setText(tr("Intelligent Recognition Of The Music Being Played"));

    QLabel *text = new QLabel(tr("ShotCut:") + " Shift+Ctrl+T", widget);
    text->setStyleSheet(MusicUIObject::MCustomStyle02);

    widgetLayout->addStretch(2);
    widgetLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    widgetLayout->addStretch(1);
    widgetLayout->addWidget(m_detectedLabel, 0, Qt::AlignCenter);
    widgetLayout->addStretch(1);
    widgetLayout->addWidget(text, 0, Qt::AlignCenter);
    widget->setLayout(widgetLayout);

    m_mainWindow->addWidget(widget);
    m_mainWindow->setCurrentWidget(widget);
}

void MusicIdentifySongsWidget::createDetectedSuccessedWidget()
{

}

void MusicIdentifySongsWidget::createDetectedFailedWidget()
{
    QWidget *widget = new QWidget(m_mainWindow);
    widget->setStyleSheet(MusicUIObject::MColorStyle03 + MusicUIObject::MCustomStyle04);
    QVBoxLayout *widgetLayout = new QVBoxLayout(widget);

    QLabel *iconLabel = new QLabel(widget);
    iconLabel->setPixmap(QPixmap(":/toolSets/lb_detect_error"));
    QLabel *text1Label = new QLabel(tr("No Songs Identified"), widget);
    QLabel *text2Label = new QLabel(tr("Only The Music Being Played Can Be Recognized"), widget);
    QLabel *text3Label = new QLabel(tr("Redetect"), widget);
    text3Label->setStyleSheet(MusicUIObject::MCustomStyle02);

    QPushButton *reDetect = new QPushButton(widget);
    reDetect->setFixedSize(56, 56);
    reDetect->setStyleSheet(MusicUIObject::MKGSongsRedetectBtn);
    reDetect->setCursor(QCursor(Qt::PointingHandCursor));
    connect(reDetect, SIGNAL(clicked()), SLOT(reDetectButtonClicked()));

    widgetLayout->addStretch(2);
    widgetLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    widgetLayout->addStretch(1);
    widgetLayout->addWidget(text1Label, 0, Qt::AlignCenter);
    widgetLayout->addStretch(1);
    widgetLayout->addWidget(text2Label, 0, Qt::AlignCenter);
    widgetLayout->addStretch(2);
    widgetLayout->addWidget(reDetect, 0, Qt::AlignCenter);
    widgetLayout->addWidget(text3Label, 0, Qt::AlignCenter);
    widgetLayout->addStretch(1);
    widget->setLayout(widgetLayout);

    m_mainWindow->addWidget(widget);
    m_mainWindow->setCurrentWidget(widget);
}
