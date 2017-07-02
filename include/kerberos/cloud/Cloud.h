//
//  Class: Cloud
//  Description: Cloud storage services
//  Created:     05/02/2015
//  Author:      Cédric Verstraeten
//  Mail:        cedric@verstraeten.io
//  Website:     www.verstraeten.io
//
//  The copyright to the computer program(s) herein
//  is the property of Verstraeten.io, Belgium.
//  The program(s) may be used and/or copied under
//  the CC-NC-ND license model.
//
//  https://doc.kerberos.io/license
//
/////////////////////////////////////////////////////

#include "Factory.h"
#include <sys/stat.h>
#include "Guard.h"

#ifndef __Cloud_H_INCLUDED__   // if Cloud.h hasn't been included yet...
#define __Cloud_H_INCLUDED__   // #define this so the compiler knows it has been included

#include "restclient-cpp/restclient.h"

namespace kerberos
{
    class Cloud
    {
        protected:
            const char * name;
            int m_min;
            int m_max;
            int m_interval;
            FW::Guard * guard;
            std::string m_captureDirectory;

        public:
            pthread_t m_pollThread;
            pthread_t m_uploadThread;
            pthread_t m_healthThread;
            std::string m_productKey;
            std::string m_configuration_path;
            std::string m_hash;

            Cloud(){};
            virtual ~Cloud(){};
            virtual void setup(kerberos::StringMap & settings) = 0;
            virtual bool upload(std::string pathToImage) = 0;
            void scan();
            void setProductKey(std::string key)
            {
                m_productKey = key;
            };
            void setConfigurationPath(std::string path)
            {
                m_configuration_path = path;
            };
            void generateHash(kerberos::StringMap & settings);
            std::string getDiskPercentage()
            {
                std::string size = "-1";

                if(RUNNING_ON_A_RASPBERRYPI)
                {
                    size = helper::GetStdoutFromCommand("echo $(df -h | grep /dev/mmcblk0p3 | head -1 | awk -F' ' '{ print $5/1 }' | tr ['%'] [\"0\"])");
                }

                return size;
            };
            std::string getTemperature()
            {
                std::string temperature = "-1";

                if(RUNNING_ON_A_RASPBERRYPI)
                {
                    temperature = helper::GetStdoutFromCommand("vcgencmd measure_temp");
                }

                return temperature;
            };

            void startUploadThread();
            void stopUploadThread();
            void startPollThread();
            void stopPollThread();
            void startHealthThread();
            void stopHealthThread();
    };

    template<const char * Alias, typename Class>
    class CloudCreator: public Cloud
    {
        protected:
            CloudCreator(){name = ID;}

        public:
            static Cloud* create(){return new Class();};
            static const char * ID;
    };
}
#endif
