/*==============================================================================
Play Sound Example
Copyright (c), Firelight Technologies Pty, Ltd 2004-2023.

This example shows how to simply load and play multiple sounds, the simplest 
usage of FMOD. By default FMOD will decode the entire file into memory when it
loads. If the sounds are big and possibly take up a lot of RAM it would be
better to use the FMOD_CREATESTREAM flag, this will stream the file in realtime
as it plays.

For information on using FMOD example code in your own programs, visit
https://www.fmod.com/legal
==============================================================================*/
#include "fmod.hpp"
#include "common.h"
#include <android/log.h>
#include <fmod_errors.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstddef>
int FMOD_Main()
{
    FMOD::System     *system;
    FMOD::Sound      *sound1, *sound2, *sound3;
    FMOD::Channel    *channel = 0;
    FMOD_RESULT       result;
    void             *extradriverdata = 0;
    
    Common_Init(&extradriverdata);

    /*
        Create a System object and initialize
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("drumloop.wav"), FMOD_DEFAULT, 0, &sound1);
    ERRCHECK(result);

    result = sound1->setMode(FMOD_LOOP_OFF);    /* drumloop.wav has embedded loop points which automatically makes looping turn on, */
    ERRCHECK(result);                           /* so turn it off here.  We could have also just put FMOD_LOOP_OFF in the above CreateSound call. */

    result = system->createSound(Common_MediaPath("jaguar.wav"), FMOD_DEFAULT, 0, &sound2);
    ERRCHECK(result);

    result = system->createSound(Common_MediaPath("swish.wav"), FMOD_DEFAULT, 0, &sound3);
    ERRCHECK(result);

    /*
        Main loop
    */
    do
    {
        Common_Update();

        if (Common_BtnPress(BTN_ACTION1))
        {
            /*result = system->playSound(sound1, 0, false, &channel);
            ERRCHECK(result);*/
            FMOD_SOUND_TYPE type;
            FMOD_SOUND_FORMAT format;
            int channels;
            unsigned int soundLength;
            int bits;
            result=sound1->getFormat(&type, &format, &channels, &bits);
            __android_log_print(ANDROID_LOG_INFO, "YourTag", "format: %s", FMOD_ErrorString(result));

            result=sound1->getLength(&soundLength, FMOD_TIMEUNIT_PCMBYTES);
            __android_log_print(ANDROID_LOG_INFO, "YourTag", "sound length: %s", FMOD_ErrorString(result));
            std::vector<char> rawAudioData;
            void* ptr1;
            unsigned int length1;

             result = sound1->lock(0, soundLength, &ptr1, nullptr, &length1, nullptr);
            __android_log_print(ANDROID_LOG_INFO, "YourTag", "sound lock: %s", FMOD_ErrorString(result));
            if (result == FMOD_OK)
            {
                rawAudioData.resize(length1);
                std::memcpy(rawAudioData.data(), ptr1, length1);
                const char *files = "sample";
                const char *str1 = "/storage/emulated/0/Android/data/org.fmod.example/files/";
                const char *str2 = ".raw";

                std::string rawcreate = std::string(str1) + std::string(files) + std::string(str2);
                std::ofstream MyFile;

                MyFile.open(rawcreate.c_str(), std::ios::binary);
                MyFile.write(rawAudioData.data(), rawAudioData.size());
                MyFile.close();
                result=sound1->unlock(ptr1, nullptr, length1, 0);
                __android_log_print(ANDROID_LOG_INFO, "YourTag", "sound unlock: %s", FMOD_ErrorString(result));
            }
        }

        if (Common_BtnPress(BTN_ACTION2))
        {
            result = system->playSound(sound2, 0, false, &channel);
            ERRCHECK(result);
        }

        if (Common_BtnPress(BTN_ACTION3))
        {
            result = system->playSound(sound3, 0, false, &channel);
            ERRCHECK(result);
        }

        result = system->update();
        ERRCHECK(result);

        {
            unsigned int ms = 0;
            unsigned int lenms = 0;
            bool         playing = 0;
            bool         paused = 0;
            int          channelsplaying = 0;

            if (channel)
            {
                FMOD::Sound *currentsound = 0;

                result = channel->isPlaying(&playing);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPaused(&paused);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
               
                channel->getCurrentSound(&currentsound);
                if (currentsound)
                {
                    result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                }
            }

            system->getChannelsPlaying(&channelsplaying, NULL);

            Common_Draw("==================================================");
            Common_Draw("Play Sound Example.");
            Common_Draw("Copyright (c) Firelight Technologies 2004-2023.");
            Common_Draw("==================================================");
            Common_Draw("");
            Common_Draw("Press %s to play a mono sound (drumloop)", Common_BtnStr(BTN_ACTION1));
            Common_Draw("Press %s to play a mono sound (jaguar)", Common_BtnStr(BTN_ACTION2));
            Common_Draw("Press %s to play a stereo sound (swish)", Common_BtnStr(BTN_ACTION3));
            Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
            Common_Draw("");
            Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
            Common_Draw("Channels Playing %d", channelsplaying);
        }

        Common_Sleep(50);
    } while (!Common_BtnPress(BTN_QUIT));

    /*
        Shut down
    */
    result = sound1->release();
    ERRCHECK(result);
    result = sound2->release();
    ERRCHECK(result);
    result = sound3->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    Common_Close();

    return 0;
}
