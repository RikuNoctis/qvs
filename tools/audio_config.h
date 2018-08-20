#ifndef AUDIO_CONFIG_H
#define AUDIO_CONFIG_H

#include <QDialog>
#include <QDebug>

class MainWindow;
class AudioEnc;

namespace Ui {
class AudioConfig;
}

class AudioConfig : public QDialog
{
    Q_OBJECT

public:
    explicit AudioConfig(QDialog *parent = 0);
    ~AudioConfig();
    friend class MainWindow;
    friend class AudioEnc;
    class MainWindow *mainUi;

private slots:
    void resizeEvent(QResizeEvent *e);

    void on_buttonAccept_clicked();
    void on_buttonCancel_clicked();
    void on_comboBoxAudioEncoder_currentIndexChanged(int a_index);

private:
    Ui::AudioConfig *ui;
};

#endif // AUDIO_CONFIG_H