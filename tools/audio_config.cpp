#include "audio_config.h"
#include "audio_enc.h"
#include "std_watcher.h"

#include "../mainwindow.h"
#include "../com/style_sheet.h"

#include "ui_mainwindow.h"
#include "ui_audio_config.h"
#include "ui_audio_enc.h"

#define QAAC_BITRATE_MAX (1280)   /* 0~1280 kbps */
#define QAAC_QUALITY_MAX (127)    /* 0~127 */
#define FDKAAC_BITRATE_MAX (320)  /* 0~320 kbps */
#define FDKAAC_QUALITY_MAX (5)    /* 1~5 */
#define NEROAAC_BITRATE_MAX (640) /* 16~640 kbps */
#define NEROAAC_QUALITY_MAX (100) /* 0.0~1.0 */
#define FLAC_QUALITY_MAX (8)      /* 0~8 */
#define OPUS_BITRATE_MAX (512)    /* 6~512 */
#define VORBIS_QUALITY_MAX (100)  /* -2.0~10.0 */
#define MP3_BITRATE_MAX (320)     /* 32~320, 8~320 */
#define MP3_QUALITY_MAX (9)       /* 0~9 */
#define AC3_BITRATE_MAX (640)     /* 64~640 */
#define NEROAAC_BITRATE_DIFF_MULTIPLIER (1000)

static const QString c_audio_config_bitrate = QObject::tr("Bitrate");
static const QString c_audio_config_quality = QObject::tr("Quality");
static const QString c_template_key_default = QObject::tr("<Default>");

AudioConfig::AudioConfig(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::AudioConfig),
    m_advancedMode(false)
{
    ui->setupUi(this);
    this->setup();
}

AudioConfig::~AudioConfig()
{
    delete ui;
}

void AudioConfig::setup(void)
{
    this->setupUi();
    this->installEventFilter(this);
    this->setStyleSheet(c_qss_slider_white_circle);
    this->loadConfig();

    ui->comboBoxAacAppleMode->installEventFilter(this);
}

void AudioConfig::setupUi(void)
{
    /* Group: AAC(APPLE) */
    emit ui->comboBoxAacAppleProfile->currentIndexChanged(ui->comboBoxAacAppleProfile->currentIndex());
    emit ui->comboBoxAacAppleMode->currentIndexChanged(ui->comboBoxAacAppleMode->currentIndex());

    /* Group: AAC(FDK) */
    emit ui->comboBoxAacFdkMode->currentIndexChanged(ui->comboBoxAacFdkMode->currentIndex());
    emit ui->comboBoxAacFdkProfile->currentIndexChanged(ui->comboBoxAacFdkProfile->currentIndex());

    /* Group: AAC(NERO) */
    emit ui->comboBoxAacNeroMode->currentIndexChanged(ui->comboBoxAacNeroMode->currentIndex());
    emit ui->comboBoxAacNeroProfile->currentIndexChanged(ui->comboBoxAacNeroProfile->currentIndex());

    /* Group: ALAC */
    PASS;

    /* Group: FLAC */
    ui->horizontalSliderFlac->setMinimum((int)eINDEX_0);
    ui->horizontalSliderFlac->setMaximum(FLAC_QUALITY_MAX);
    ui->horizontalSliderFlac->setTickInterval((int)eINDEX_1);
    ui->horizontalSliderFlac->setPageStep((int)eINDEX_1);

    /* Group: OPUS */
    emit ui->comboBoxOpusMode->currentIndexChanged(ui->comboBoxOpusMode->currentIndex());

    /* Group: OGG_VORBIS */
    PASS;

    /* Group: MP3 */
    emit ui->comboBoxMp3Mode->currentIndexChanged(ui->comboBoxMp3Mode->currentIndex());

    /* Group: AC3 */
    PASS;

    /* Group: WAV */
    PASS;

    static const QList<QPair<AudioEnc::EENCODE_TYPE, QVariant>> s_config_default_value = {
        { AudioEnc::eENCODE_TYPE_AAC_APPLE,  DEFAULT_BITRATE },
        { AudioEnc::eENCODE_TYPE_AAC_FDK,    DEFAULT_BITRATE },
        { AudioEnc::eENCODE_TYPE_AAC_NERO,   DEFAULT_BITRATE },
        { AudioEnc::eENCODE_TYPE_ALAC,       QVariant()      },
        { AudioEnc::eENCODE_TYPE_FLAC,       eINDEX_5        },
        { AudioEnc::eENCODE_TYPE_OPUS,       DEFAULT_BITRATE },
        { AudioEnc::eENCODE_TYPE_OGG_VORBIS, eINDEX_100      },
        { AudioEnc::eENCODE_TYPE_MP3,        DEFAULT_BITRATE },
        { AudioEnc::eENCODE_TYPE_AC3,        DEFAULT_BITRATE },
        { AudioEnc::eENCODE_TYPE_WAV,        QVariant()      },
    };
    for(uint i = eINDEX_0; i < AudioEnc::eENCODER_MAX; i++)
    {
        setDefaultConfig(getDefaultConfig(i, s_config_default_value.at(i).second));
    }
}

void AudioConfig::loadConfig(void)
{
    ui->comboBoxTemplate->addItem(c_template_key_default);
}

AudioAdvancedConfig AudioConfig::getDefaultConfig(const uint &a_type, const QVariant &a_value)
{
    AudioAdvancedConfig advanced_config;

    advanced_config.type = a_type;
    advanced_config.mode = (uint)NUL;
    advanced_config.profile = (uint)NUL;
    advanced_config.value = a_value;

    switch(static_cast<AudioEnc::EENCODE_TYPE>(a_type))
    {
    case AudioEnc::eENCODE_TYPE_AAC_APPLE:
    default:
        advanced_config.mode = (uint)eQAAC_MODE_LC_AAC_CBR;
        advanced_config.profile = (uint)eQAAC_PROFILE_LC_AAC;
        advanced_config.value2 = false; /* No delay flag (default: true) */
        break;
    case AudioEnc::eENCODE_TYPE_AAC_FDK:
        advanced_config.mode = (uint)eFDKAAC_MODE_CBR;
        advanced_config.profile = (uint)eFDKAAC_PROFILE_MPEG_4_LC_AAC;
        break;
    case AudioEnc::eENCODE_TYPE_AAC_NERO:
        advanced_config.mode = (uint)eNEROAAC_MODE_ABR;
        advanced_config.profile = (uint)eNEROAAC_PROFILE_LC_AAC;
        break;
    case AudioEnc::eENCODE_TYPE_ALAC:
        break;
    case AudioEnc::eENCODE_TYPE_FLAC:
        advanced_config.value = (uint)eINDEX_5;
        break;
    case AudioEnc::eENCODE_TYPE_OPUS:
        advanced_config.mode = (uint)eOPUS_MODE_VBR;
        break;
    case AudioEnc::eENCODE_TYPE_OGG_VORBIS:
        advanced_config.value = (uint)(eINDEX_3 * eINDEX_100);
        break;
    case AudioEnc::eENCODE_TYPE_MP3:
        advanced_config.mode = (uint)eMP3_MODE_CBR;
        break;
    case AudioEnc::eENCODE_TYPE_AC3:
        break;
    case AudioEnc::eENCODE_TYPE_WAV:
        break;
    }
    return advanced_config;
}

void AudioConfig::setDefaultConfig(const AudioAdvancedConfig &advanced_config)
{
    uint type = advanced_config.type;
    int mode = (int)advanced_config.mode;
    int profile = (int)advanced_config.profile;
    int value = advanced_config.value.toInt();

    switch(static_cast<AudioEnc::EENCODE_TYPE>(type))
    {
    case AudioEnc::eENCODE_TYPE_AAC_APPLE:
    default:
        ui->comboBoxAacAppleMode->setCurrentIndex(mode);
        ui->comboBoxAacAppleProfile->setCurrentIndex(profile);
        ui->horizontalSliderAacApple->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_AAC_FDK:
        ui->comboBoxAacFdkMode->setCurrentIndex(mode);
        ui->comboBoxAacFdkProfile->setCurrentIndex(profile);
        ui->horizontalSliderAacFdk->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_AAC_NERO:
        ui->comboBoxAacNeroMode->setCurrentIndex(mode);
        ui->comboBoxAacNeroProfile->setCurrentIndex(profile);
        ui->horizontalSliderAacNero->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_ALAC:
        break;
    case AudioEnc::eENCODE_TYPE_FLAC:
        ui->horizontalSliderFlac->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_OPUS:
        ui->comboBoxOpusMode->setCurrentIndex(mode);
        ui->horizontalSliderOpus->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_OGG_VORBIS:
        ui->horizontalSliderOggVorbis->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_MP3:
        ui->comboBoxMp3Mode->setCurrentIndex(mode);
        ui->horizontalSliderMp3->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_AC3:
        ui->horizontalSliderAc3->setValue(value);
        break;
    case AudioEnc::eENCODE_TYPE_WAV:
        break;
    }
}

void AudioConfig::setDefaultConfig(const uint &a_type, const QVariant &a_value)
{
    setDefaultConfig(getDefaultConfig(a_type, a_value));
}

void AudioConfig::on_comboBoxAudioEncoder_currentIndexChanged(int a_index)
{
    ui->stackedWidgetMode->setCurrentIndex(a_index);
}

void AudioConfig::on_checkBoxAdvancedOption_stateChanged(int a_state)
{
    switch(Qt::CheckState(a_state))
    {
    case Qt::Unchecked:
    default:
        m_advancedMode = false;
        break;
    case Qt::PartiallyChecked:
    case Qt::Checked:
        m_advancedMode = true;
        break;
    }
    ui->stackedWidgetMode->setEnabled(m_advancedMode);
}

void AudioConfig::setMode(bool a_advancedMode)
{
    Qt::CheckState state = Qt::PartiallyChecked;

    if(a_advancedMode)
    {
        state = Qt::Checked;
    }
    else
    {
        state = Qt::Unchecked;
    }

    ui->checkBoxAdvancedOption->setCheckState(state);
    if(state == ui->checkBoxAdvancedOption->checkState())
    {
        emit ui->checkBoxAdvancedOption->stateChanged((int)state);
    }

    m_advancedMode = a_advancedMode;
}

void AudioConfig::contextMenuEvent(QContextMenuEvent *e)
{
    e->accept();
#if 0
    QMenu *menu = new QMenu();
    QAction *action = menu->addAction("Edit value");

    connect(action, SIGNAL(triggered()), this, SLOT(slotEditValue()));
    menu->exec(QCursor::pos());
    delete menu;
#endif
}

bool AudioConfig::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::Show)
    {
        if(o == ui->comboBoxAacAppleMode)
        {
            resizeEvent();
        }
        else if(o == this)
        {
            resizeEventMinimum();
        }
    }
    return false;
}

void AudioConfig::resizeEvent(QResizeEvent *e)
{
    /* Make Combo Box width the same.*/
    ui->comboBoxAudioEncoder->resize(ui->comboBoxTemplate->size().width(), ui->comboBoxAudioEncoder->size().height());
    ui->comboBoxAacAppleMode->resize(ui->comboBoxAacAppleProfile->size().width(), ui->comboBoxAacAppleMode->size().height());
    e->accept();
}

void AudioConfig::resizeEvent(void)
{
    resize(width() + eINDEX_1, height());
    resize(width() - eINDEX_1, height());
}

void AudioConfig::resizeEventMinimum(void)
{
    resize(minimumWidth() + eINDEX_1, minimumHeight());
    resize(minimumWidth(), minimumHeight());
}

///->QAAC_START

void AudioConfig::on_comboBoxAacAppleProfile_currentIndexChanged(int a_index)
{
    QStringList items;

    switch((EAUDIO_CONFIG_PROFILE)a_index)
    {
    case eQAAC_PROFILE_LC_AAC:
    default:
        ui->checkBoxAacAppleNoDelay->setEnabled(true);
        items << tr("True VBR") << tr("Constrained VBR") << tr("ABR") << tr("CBR");
        break;
    case eQAAC_PROFILE_HE_AAC:
        ui->checkBoxAacAppleNoDelay->setEnabled(false);
        items << tr("Constrained VBR") << tr("ABR") << tr("CBR");
        break;
    }
    ui->comboBoxAacAppleMode->clear();
    ui->comboBoxAacAppleMode->addItems(items);
}

void AudioConfig::on_comboBoxAacAppleMode_currentIndexChanged(int a_index)
{
    if( (ui->comboBoxAacAppleProfile->currentIndex() == (int)eQAAC_PROFILE_LC_AAC) && (a_index == (int)eQAAC_MODE_LC_AAC_TRUE_VBR) )
    {
        fitValue(ui->horizontalSliderAacApple, QAAC_QUALITY_MAX);
    }
    else
    {
        fitValue(ui->horizontalSliderAacApple, QAAC_BITRATE_MAX);
    }
    ui->horizontalSliderAacApple->setTickInterval(ui->horizontalSliderAacApple->maximum() / (int)eINDEX_10);
}

void AudioConfig::on_horizontalSliderAacApple_valueChanged(int a_value)
{
    QString mode = c_audio_config_bitrate;

    if( (ui->comboBoxAacAppleProfile->currentIndex() == (int)eQAAC_PROFILE_LC_AAC) && (ui->comboBoxAacAppleMode->currentIndex() == (int)eQAAC_MODE_LC_AAC_TRUE_VBR) )
    {
        mode = c_audio_config_quality;
    }
    ui->groupBoxAacApple->setTitle(QString("QAAC - (%1=%2)").arg(mode).arg(a_value));
}

///->QAAC_END

///->FDKAAC_START

void AudioConfig::on_comboBoxAacFdkMode_currentIndexChanged(int a_index)
{
    switch((EAUDIO_CONFIG_MODE)a_index)
    {
    case eFDKAAC_MODE_CBR:
    default:
        fitValue(ui->horizontalSliderAacFdk, FDKAAC_BITRATE_MAX);
        ui->horizontalSliderAacFdk->setTickInterval(ui->horizontalSliderAacFdk->maximum() / (int)eINDEX_10);
        ui->horizontalSliderAacFdk->setPageStep((int)eINDEX_10);
        break;
    case eFDKAAC_MODE_VBR:
        fitValue(ui->horizontalSliderAacFdk, FDKAAC_QUALITY_MAX);
        ui->horizontalSliderAacFdk->setTickInterval((int)eINDEX_1);
        ui->horizontalSliderAacFdk->setPageStep((int)eINDEX_1);
        break;
    }
}

void AudioConfig::on_horizontalSliderAacFdk_valueChanged(int a_value)
{
    QString mode = c_audio_config_bitrate;

    if(ui->comboBoxAacFdkMode->currentIndex() == (int)eFDKAAC_MODE_VBR)
    {
        mode = c_audio_config_quality;
    }
    ui->groupBoxAacFdk->setTitle(QString("FDKAAC - (%1=%2)").arg(mode).arg(a_value));
}

///->FDKAAC_END

///->NEROAAC_START

void AudioConfig::on_comboBoxAacNeroMode_currentIndexChanged(int a_index)
{
    switch((EAUDIO_CONFIG_MODE)a_index)
    {
    case eNEROAAC_MODE_ABR:
    case eNEROAAC_MODE_CBR:
    default:
        fitValue(ui->horizontalSliderAacNero, NEROAAC_BITRATE_MAX, (int)eINDEX_16);
        ui->horizontalSliderAacNero->setTickInterval(ui->horizontalSliderAacNero->maximum() / (int)eINDEX_10);
        ui->horizontalSliderAacNero->setPageStep((int)eINDEX_10);
        ui->labelAacNeroProfile->setEnabled(true);
        ui->comboBoxAacNeroProfile->setEnabled(true);
        break;
    case eNEROAAC_MODE_VBR:
        fitValue(ui->horizontalSliderAacNero, NEROAAC_QUALITY_MAX, (int)eINDEX_0);
        ui->horizontalSliderAacNero->setTickInterval((int)eINDEX_10);
        ui->horizontalSliderAacNero->setPageStep((int)eINDEX_1);
        ui->labelAacNeroProfile->setDisabled(true);
        ui->comboBoxAacNeroProfile->setDisabled(true);
        break;
    }
}

void AudioConfig::on_horizontalSliderAacNero_valueChanged(int a_value)
{
    QString mode = c_audio_config_bitrate;
    double value = a_value;

    if(ui->comboBoxAacNeroMode->currentIndex() == (int)eNEROAAC_MODE_VBR)
    {
        mode = c_audio_config_quality;
        value /= (int)eINDEX_100;
    }
    ui->groupBoxAacNero->setTitle(QString("NeroAAC - (%1=%2)").arg(mode).arg(value));
}

///->NEROAAC_END

///->FLAC_START

void AudioConfig::on_horizontalSliderFlac_valueChanged(int a_value)
{
    QString mode = c_audio_config_quality;

    ui->groupBoxFlac->setTitle(QString("FLAC - (%1=%2)").arg(mode).arg(a_value));
}

///->FLAC_END

///->OPUS_START

void AudioConfig::on_horizontalSliderOpus_valueChanged(int a_value)
{
    QString mode = c_audio_config_bitrate;

    ui->groupBoxOpus->setTitle(QString("OPUS - (%1=%2)").arg(mode).arg(a_value));
}

///->OPUS_END

///->OGG_VORBIS_START

void AudioConfig::on_horizontalSliderOggVorbis_valueChanged(int a_value)
{
    QString mode = c_audio_config_quality;
    double value = a_value;

    value /= (int)eINDEX_100;
    ui->groupBoxOggVorbis->setTitle(QString("Ogg Vorbis - (%1=%2)").arg(mode).arg(value));
}

///->OGG_VORBIS_END

///->MP3_START

void AudioConfig::on_comboBoxMp3Mode_currentIndexChanged(int a_index)
{
    switch((EAUDIO_CONFIG_MODE)a_index)
    {
    case eMP3_MODE_CBR:
    default:
        fitValue(ui->horizontalSliderMp3, MP3_BITRATE_MAX, (int)eINDEX_32);
        ui->horizontalSliderMp3->setTickInterval((int)eINDEX_8);
        ui->horizontalSliderMp3->setPageStep((int)eINDEX_8);
        break;
    case eMP3_MODE_ABR:
        fitValue(ui->horizontalSliderMp3, MP3_BITRATE_MAX, (int)eINDEX_8);
        ui->horizontalSliderMp3->setTickInterval((int)eINDEX_8);
        ui->horizontalSliderMp3->setPageStep((int)eINDEX_8);
        break;
    case eMP3_MODE_VBR:
        fitValue(ui->horizontalSliderMp3, MP3_QUALITY_MAX, (int)eINDEX_0);
        ui->horizontalSliderMp3->setTickInterval((int)eINDEX_1);
        ui->horizontalSliderMp3->setPageStep((int)eINDEX_1);
        break;
    }
}

void AudioConfig::on_horizontalSliderMp3_valueChanged(int a_value)
{
    QString mode = c_audio_config_bitrate;

    if(ui->comboBoxMp3Mode->currentIndex() == (int)eMP3_MODE_VBR)
    {
        mode = c_audio_config_quality;
    }
    ui->groupBoxMp3->setTitle(QString("Lame MP3 - (%1=%2)").arg(mode).arg(a_value));
}

///->MP3_END

///->AC3_START

void AudioConfig::on_horizontalSliderAc3_valueChanged(int a_value)
{
    QString mode = c_audio_config_bitrate;

    ui->groupBoxAc3->setTitle(QString("AC3 - (%1=%2)").arg(mode).arg(a_value));
}

///->AC3_END

void AudioConfig::on_buttonCancel_clicked()
{
    this->reject();
}

void AudioConfig::on_buttonAccept_clicked()
{
    AudioAdvancedConfig config = getConfig();

    mainUi->m_pAudioEnc->ui->comboBoxAudioEncoder->setCurrentIndex(config.type);
    emit mainUi->m_pAudioEnc->ui->comboBoxAudioEncoder->currentIndexChanged(config.type);

    mainUi->m_pAudioEnc->setConfig(config);
    this->accept();
}

AudioAdvancedConfig AudioConfig::getConfig(void)
{
    AudioAdvancedConfig config;

    config.setEnable(m_advancedMode);
    config.type = ui->comboBoxAudioEncoder->currentIndex();

    switch(static_cast<AudioEnc::EENCODE_TYPE>(config.type))
    {
    case AudioEnc::eENCODE_TYPE_AAC_APPLE:
    default:
        config.name = ui->groupBoxAacApple->title();
        config.cmd = processAccApple();
        config.mode = ui->comboBoxAacAppleMode->currentIndex();
        config.profile = ui->comboBoxAacAppleProfile->currentIndex();
        config.value = ui->horizontalSliderAacApple->value();
        config.value2 = ui->checkBoxAacAppleNoDelay->isChecked();
        break;
    case AudioEnc::eENCODE_TYPE_AAC_FDK:
        config.name = ui->groupBoxAacFdk->title();
        config.cmd = processAccFdk();
        config.mode = ui->comboBoxAacFdkMode->currentIndex();
        config.profile = ui->comboBoxAacFdkProfile->currentIndex();
        config.value = ui->horizontalSliderAacFdk->value();
        break;
    case AudioEnc::eENCODE_TYPE_AAC_NERO:
        config.name = ui->groupBoxAacNero->title();
        config.cmd = processAccNero();
        config.mode = ui->comboBoxAacNeroMode->currentIndex();
        config.profile = ui->comboBoxAacNeroProfile->currentIndex();
        config.value = ui->horizontalSliderAacNero->value();
        break;
    case AudioEnc::eENCODE_TYPE_ALAC:
        config.name = ui->groupBoxAlac->title();
        config.cmd = processAlac();
        break;
    case AudioEnc::eENCODE_TYPE_FLAC:
        config.name = ui->groupBoxFlac->title();
        config.cmd = processFlac();
        config.value = ui->horizontalSliderFlac->value();
        break;
    case AudioEnc::eENCODE_TYPE_OPUS:
        config.name = ui->groupBoxOpus->title();
        config.cmd = processOpus();
        config.mode = ui->comboBoxOpusMode->currentIndex();
        config.value = ui->horizontalSliderOpus->value();
        break;
    case AudioEnc::eENCODE_TYPE_OGG_VORBIS:
        config.name = ui->groupBoxOggVorbis->title();
        config.cmd = processOggVorbis();
        config.value = ui->horizontalSliderOggVorbis->value();
        break;
    case AudioEnc::eENCODE_TYPE_MP3:
        config.name = ui->groupBoxMp3->title();
        config.cmd = processMp3();
        config.mode = ui->comboBoxMp3Mode->currentIndex();
        config.value = ui->horizontalSliderMp3->value();
        break;
    case AudioEnc::eENCODE_TYPE_AC3:
        config.name = ui->groupBoxAc3->title();
        config.cmd = processAc3();
        config.value = ui->horizontalSliderAc3->value();
        break;
    case AudioEnc::eENCODE_TYPE_WAV:
        config.name = ui->groupBoxWav->title();
        config.cmd = processWav();
        break;
    }
    return config;
}

void AudioConfig::setConfig(AudioAdvancedConfig *a_pAdvancedConfig)
{
    setMode(a_pAdvancedConfig->isEnable());

    if(!m_advancedMode)
    {
        a_pAdvancedConfig = &getDefaultConfig();
    }

    switch(static_cast<AudioEnc::EENCODE_TYPE>(a_pAdvancedConfig->type))
    {
    case AudioEnc::eENCODE_TYPE_AAC_APPLE:
    default:
        ui->comboBoxAacAppleMode->setCurrentIndex(a_pAdvancedConfig->mode);
        ui->comboBoxAacAppleProfile->setCurrentIndex(a_pAdvancedConfig->profile);
        ui->horizontalSliderAacApple->setValue(a_pAdvancedConfig->value.toInt());
        ui->checkBoxAacAppleNoDelay->setChecked(a_pAdvancedConfig->value2.toBool());
        break;
    case AudioEnc::eENCODE_TYPE_AAC_FDK:
        ui->comboBoxAacFdkMode->setCurrentIndex(a_pAdvancedConfig->mode);
        ui->comboBoxAacFdkProfile->setCurrentIndex(a_pAdvancedConfig->profile);
        ui->horizontalSliderAacFdk->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_AAC_NERO:
        ui->comboBoxAacNeroMode->setCurrentIndex(a_pAdvancedConfig->mode);
        ui->comboBoxAacNeroProfile->setCurrentIndex(a_pAdvancedConfig->profile);
        ui->horizontalSliderAacNero->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_ALAC:
        break;
    case AudioEnc::eENCODE_TYPE_FLAC:
        ui->horizontalSliderFlac->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_OPUS:
        ui->comboBoxOpusMode->setCurrentIndex(a_pAdvancedConfig->mode);
        ui->horizontalSliderOpus->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_OGG_VORBIS:
        ui->horizontalSliderOggVorbis->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_MP3:
        ui->comboBoxMp3Mode->setCurrentIndex(a_pAdvancedConfig->mode);
        ui->horizontalSliderMp3->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_AC3:
        ui->horizontalSliderAc3->setValue(a_pAdvancedConfig->value.toInt());
        break;
    case AudioEnc::eENCODE_TYPE_WAV:
        break;
    }

    ui->comboBoxAudioEncoder->setCurrentIndex(a_pAdvancedConfig->type);
    emit ui->comboBoxAudioEncoder->currentIndexChanged(a_pAdvancedConfig->type);
}

void AudioConfig::setConfig(AudioAdvancedConfig a_pAdvancedConfig)
{
    AudioAdvancedConfig *at_pAdvancedConfig = new AudioAdvancedConfig();

    at_pAdvancedConfig = &a_pAdvancedConfig;
    setConfig(at_pAdvancedConfig);
    delete at_pAdvancedConfig;
}

QString AudioConfig::processAccApple(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_AAC_APPLE).toUtf8();
    QByteArray delay = ui->checkBoxAacAppleNoDelay->isChecked() ? "--no-delay" : QT_EMPTY;
    EAUDIO_CONFIG_PROFILE profile = static_cast<EAUDIO_CONFIG_PROFILE>(ui->comboBoxAacAppleProfile->currentIndex());
    EAUDIO_CONFIG_MODE mode = static_cast<EAUDIO_CONFIG_MODE>(ui->comboBoxAacAppleMode->currentIndex());

    switch(profile)
    {
    case eQAAC_PROFILE_LC_AAC:
    default:
        if(mode == eQAAC_MODE_LC_AAC_TRUE_VBR) /* not variant value */
        {
            cmd.sprintf("%s --ignorelength --threading -V %d %s - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value(), delay.data());
        }
        else if(mode == eQAAC_MODE_LC_AAC_CONSTRAINED_VBR)
        {
            cmd.sprintf("%s --ignorelength --threading -v %d %s - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value(), delay.data());
        }
        else if(mode == eQAAC_MODE_LC_AAC_ABR)
        {
            cmd.sprintf("%s --ignorelength --threading -a %d %s - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value(), delay.data());
        }
        else if(mode == eQAAC_MODE_LC_AAC_CBR)
        {
            cmd.sprintf("%s --ignorelength --threading -c %d %s - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value(), delay.data());
        }
        break;
    case eQAAC_PROFILE_HE_AAC:
        if(mode == eQAAC_MODE_HE_AAC_CONSTRAINED_VBR)
        {
            cmd.sprintf("%s --ignorelength --threading -he -v %d - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value());
        }
        else if(mode == eQAAC_MODE_HE_AAC_ABR)
        {
            cmd.sprintf("%s --ignorelength --threading -he -a %d - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value());
        }
        else if(mode == eQAAC_MODE_HE_AAC_CBR)
        {
            cmd.sprintf("%s --ignorelength --threading -he -c %d - -o \"%1\"", exec.data(), ui->horizontalSliderAacApple->value());
        }
        break;
    }
    return cmd;
}

QString AudioConfig::processAccFdk(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_AAC_FDK).toUtf8();
    EAUDIO_CONFIG_PROFILE profile = static_cast<EAUDIO_CONFIG_PROFILE>(ui->comboBoxAacFdkProfile->currentIndex());
    EAUDIO_CONFIG_MODE mode = static_cast<EAUDIO_CONFIG_MODE>(ui->comboBoxAacFdkMode->currentIndex());

    switch(profile)
    {
    case eFDKAAC_PROFILE_MPEG_4_LC_AAC:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 2 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 2 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_4_HE_AAC:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 5 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 5 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_4_HE_AAC_V2:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 29 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 29 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_4_AAC_LD:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 23 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 23 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_4_AAC_ELD:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 39 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 39 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_2_LC_AAC:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 129 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 129 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_2_HE_AAC:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 132 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 132 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    case eFDKAAC_PROFILE_MPEG_2_HE_AAC_V2:
        if(mode == eFDKAAC_MODE_CBR)
        {
            cmd.sprintf("%s --ignorelength -m 0 -b %d -p 156 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        else if(mode == eFDKAAC_MODE_VBR)
        {
            cmd.sprintf("%s --ignorelength -m %d -p 156 - -o \"%1\"", exec.data(), ui->horizontalSliderAacFdk->value());
        }
        break;
    }
    return cmd;
}

QString AudioConfig::processAccNero(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_AAC_NERO).toUtf8();
    EAUDIO_CONFIG_PROFILE profile = static_cast<EAUDIO_CONFIG_PROFILE>(ui->comboBoxAacNeroProfile->currentIndex());
    EAUDIO_CONFIG_MODE mode = static_cast<EAUDIO_CONFIG_MODE>(ui->comboBoxAacNeroMode->currentIndex());
    int value = ui->horizontalSliderAacNero->value();

    switch(mode)
    {
    case eNEROAAC_MODE_ABR:
    default:
        value *= NEROAAC_BITRATE_DIFF_MULTIPLIER;
        if(profile == eNEROAAC_PROFILE_AUTO)
        {
            cmd.sprintf("%s -ignorelength -br %d -if - -of \"%1\"", exec.data(), value);
        }
        else if(profile == eNEROAAC_PROFILE_HE_AAC_PS)
        {
            cmd.sprintf("%s -ignorelength -hev2 -br %d -if - -of \"%1\"", exec.data(), value);
        }
        else if(profile == eNEROAAC_PROFILE_HE_AAC)
        {
            cmd.sprintf("%s -ignorelength -he -br %d -if - -of \"%1\"", exec.data(), value);
        }
        else if(profile == eNEROAAC_PROFILE_LC_AAC)
        {
            cmd.sprintf("%s -ignorelength -lc -br %d -if - -of \"%1\"", exec.data(), value);
        }
        break;
    case eNEROAAC_MODE_CBR:
        value *= NEROAAC_BITRATE_DIFF_MULTIPLIER;
        if(profile == eNEROAAC_PROFILE_AUTO)
        {
            cmd.sprintf("%s -ignorelength -cbr %d -if - -of \"%1\"", exec.data(), value);
        }
        else if(profile == eNEROAAC_PROFILE_HE_AAC_PS)
        {
            cmd.sprintf("%s -ignorelength -hev2 -cbr %d -if - -of \"%1\"", exec.data(), value);
        }
        else if(profile == eNEROAAC_PROFILE_HE_AAC)
        {
            cmd.sprintf("%s -ignorelength -he -cbr %d -if - -of \"%1\"", exec.data(), value);
        }
        else if(profile == eNEROAAC_PROFILE_LC_AAC)
        {
            cmd.sprintf("%s -ignorelength -lc -cbr %d -if - -of \"%1\"", exec.data(), value);
        }
        break;
    case eNEROAAC_MODE_VBR:
        cmd.sprintf("%s -ignorelength -q %.2f -if - -of \"%1\"", exec.data(), (double)value / (int)eINDEX_100);
        break;
    }
    return cmd;
}

QString AudioConfig::processAlac(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_ALAC).toUtf8();

    cmd.sprintf("%s --ignorelength - -o \"%1\"", exec.data());
    return cmd;
}

QString AudioConfig::processFlac(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_FLAC).toUtf8();

    cmd.sprintf("%s -%d - -o \"%1\"", exec.data(), ui->horizontalSliderFlac->value());
    return cmd;
}

QString AudioConfig::processOpus(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_OPUS).toUtf8();
    EAUDIO_CONFIG_MODE mode = static_cast<EAUDIO_CONFIG_MODE>(ui->comboBoxOpusMode->currentIndex());

    switch(mode)
    {
    case eOPUS_MODE_VBR:
    default:
        cmd.sprintf("%s --ignorelength --vbr --bitrate %d - \"%1\"", exec.data(), ui->horizontalSliderOpus->value());
        break;
    case eOPUS_MODE_CONSTRAINED_VBR:
        cmd.sprintf("%s --ignorelength --cvbr --bitrate %d - \"%1\"", exec.data(), ui->horizontalSliderOpus->value());
        break;
    case eOPUS_MODE_HARD_CBR:
        cmd.sprintf("%s --ignorelength --hard-cbr --bitrate %d - \"%1\"", exec.data(), ui->horizontalSliderOpus->value());
        break;
    }
    return cmd;
}

QString AudioConfig::processOggVorbis(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_OGG_VORBIS).toUtf8();

    /* not support for bitrate mode in advanced config */
    cmd.sprintf("%s - --ignorelength --quality %.2fk -o \"%1\"", exec.data(), ui->horizontalSliderOggVorbis->value() / (double)eINDEX_100);
    return cmd;
}

QString AudioConfig::processMp3(void)
{
    QString cmd;
    QByteArray exec = mainUi->m_com->findFirstFilePath(AUDIO_CONFIG_EXEC_MP3).toUtf8();
    EAUDIO_CONFIG_MODE mode = static_cast<EAUDIO_CONFIG_MODE>(ui->comboBoxMp3Mode->currentIndex());
    int value = ui->horizontalSliderMp3->value();

    switch(mode)
    {
    case eMP3_MODE_CBR:
    default:
        cmd.sprintf("%s -b %d --cbr -h - \"%1\"", exec.data(), value);
        break;
    case eMP3_MODE_ABR:
        cmd.sprintf("%s --abr %d -h - \"%1\"", exec.data(), value);
        break;
    case eMP3_MODE_VBR:
        cmd.sprintf("%s -V%d - \"%1\"", exec.data(), value);
        break;
    }
    return cmd;
}

QString AudioConfig::processAc3(void)
{
    QString cmd;

    cmd.sprintf("%1 -i \"%2\" -c:a ac3 -b:a %dk \"%3\" -y", ui->horizontalSliderAc3->value());
    return cmd;
}

QString AudioConfig::processWav(void)
{
    QString cmd;

    /* no config */
    return cmd;
}

void AudioConfig::fitValue(QSlider *a_slider, const int &a_maxValue, const int &a_minValue)
{
    a_slider->setMinimum(a_minValue);
    a_slider->setMaximum(a_maxValue);
    if(a_slider->value() > a_maxValue)
    {
        a_slider->setValue(a_maxValue);
    }
    if(a_slider->value() < a_minValue)
    {
        a_slider->setValue(a_minValue);
    }
}

