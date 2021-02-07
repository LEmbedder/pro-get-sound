/**
 * @file getalsadata.cpp
 * @author your name (you@domain.com)
 * @brief 声音的获取和播放设置
 * @version 0.1
 * @date 2020-11-24
 *
 * @copyright Copyright (c) 2020
 *
 */

#include "getalsadata.h"
#define SOUND_RATE 22050
#define SOUND_CHANNLE_NUMBER 1
/**
 * @brief Get the Alsa Data:: Get Alsa Data object
 *
 * @param parent
 */
GetAlsaData::GetAlsaData(QObject *parent) : QObject(parent)
{
    memset(buf, 512, 0);
    capture_handle = NULL;
    playback_handle = NULL;
}
/**
 * @brief Get the Alsa Data::~ Get Alsa Data object
 *
 */
GetAlsaData::~GetAlsaData()
{
    sound_device_close();
}
/**
 * @brief 打开alsa设备
 *
 * @param type SND_PCM_STREAM_PLAYBACK
 *             SND_PCM_STREAM_CAPTURE
 * @return snd_pcm_t*
 */
snd_pcm_t *GetAlsaData::open_sound_dev(snd_pcm_stream_t type)
{
    int err;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hw_params;
    unsigned int rate = SOUND_RATE;

    if ((err = snd_pcm_open (&handle, "default", type, 0)) < 0) {
        return NULL;
    }

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, SOUND_CHANNLE_NUMBER)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
        return NULL;
    }
    snd_pcm_hw_params_free (hw_params);

    return handle;
}
/**
 * @brief
 *
 * @return snd_pcm_t*
 */
snd_pcm_t *GetAlsaData::open_playback(void)
{
    return open_sound_dev(SND_PCM_STREAM_PLAYBACK);
}
/**
 * @brief
 *
 * @return snd_pcm_t*
 */
snd_pcm_t *GetAlsaData::open_capture(void)
{
    return open_sound_dev(SND_PCM_STREAM_CAPTURE);
}
/**
 * @brief 设备初始化
 *
 * @return true
 * @return false
 */
bool GetAlsaData::sound_device_init(void)
{
    int err;

    playback_handle = open_playback();
    if (!playback_handle)
    {
        fprintf (stderr, "cannot open for playback\n");
        goto init_err;
    }

    capture_handle = open_capture();
    if (!capture_handle)
    {
        fprintf (stderr, "cannot open for capture\n");
        goto init_err;
    }

    if ((err = snd_pcm_prepare (playback_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
        goto init_err;
    }

    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
        goto init_err;
    }
    return true;
init_err:
    qDebug("init error");
    return false;
}
/**
 * @brief
 *
 * @return true
 * @return false
 */
bool GetAlsaData::sound_device_close()
{
    snd_pcm_close (playback_handle);
    snd_pcm_close (capture_handle);
    return true;
}

#define DATA_LENGTH 0X114d
/**
 * @brief 录取数据
 *
 * @return unsigned int
 */
unsigned int GetAlsaData::slot_get_capture_data()
{
    int err;
    snd_pcm_prepare (capture_handle);
    while (sdc.enable_recorde )
    {
        buffSend[0] = 0xFF;
        buffSend[1] = DATA_LENGTH & 0XFF;
        buffSend[2] = (DATA_LENGTH>>8) & 0XFF;
        memcpy(&buffSend[3],sysData.guidClienthex,16);
        buffSend[19] = REMOTE_RADIO_DATA_TYPE;

        int length = 0;
        while(length <= 4410)
        {
            if ((err = snd_pcm_readi (capture_handle, buf, 128)) != 128) {
                qDebug ("read from audio interface failed (%s)\n", snd_strerror (err));
                return -1;
            }
            memcpy(&buffSend[20+length], buf,256);
            length += 256;
        }
//        printf("sounddata:%d",length);
        buffSend[4430] = 0xFF;
        emit emitReadData2PC(buffSend, DATA_LENGTH+2);//head+end
    }

    return 0;
}

/**
 * @brief 播放音频数据
 *
 * @return unsigned int
 */
unsigned int GetAlsaData::slot_player_data()
{
    int err;

    if (sdc.state == ALSA_IS_RECORED && sdc.enable_play)
    {
        err = snd_pcm_writei(playback_handle, buf, 128);

        if (err == -EPIPE) {
              /* EPIPE means underrun */
              fprintf(stderr, "underrun occurred\n");
              snd_pcm_prepare(playback_handle);
        }
        else if (err < 0) {
            fprintf(stderr,"error from writei: %s\n", snd_strerror(err));
            err = snd_pcm_recover(playback_handle, err, 0);
        }
        else if (err != 128) {
            fprintf(stderr, "short write, write %d frames\n", err);
        }
    }

    return err;
}

