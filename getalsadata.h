#ifndef GETALSADATA_H
#define GETALSADATA_H

#include <QObject>
#include <QDebug>
#include "sysvar.h"
#ifdef DEBUG_ON_PC
    #ifdef ENBALE_ALSA
    #include <alsa/asoundlib.h>
    #endif
#endif
#include <QTime>

typedef enum{
    ALSA_IS_RECORED = 1,
    ALSE_IS_PLAY,
}ObjectState;
typedef struct {
    ObjectState state;
    bool enable_recorde;//控制采集板是否开始音频数据
    int recorde_times;
    bool enable_recorde_times;
    bool enable_play;

}sound_data_control;


class GetAlsaData : public QObject
{
    Q_OBJECT
public:
    explicit GetAlsaData(QObject *parent = 0);
    ~GetAlsaData();

    bool sound_device_init();

    snd_pcm_t *playback_handle;
    snd_pcm_t *capture_handle;
    char buf[512];
    char buffSend[10*1024];
    dataFormat_t dataToPC;
    sound_data_control sdc;



private:
    snd_pcm_t *open_sound_dev(snd_pcm_stream_t type);
    snd_pcm_t *open_playback();
    snd_pcm_t *open_capture();
    void close_sound_dev(snd_pcm_t *handle);
    int set_hardware_params();
    bool sound_device_close();
signals:
    void emitReadData2PC(char *,int);
public slots:
//    void slotWriteData2Device(char *value, int len);
    unsigned int slot_player_data();
    unsigned int slot_get_capture_data();
};

#endif // GETALSADATA_H
