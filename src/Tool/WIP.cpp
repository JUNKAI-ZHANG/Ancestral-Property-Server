#ifndef _WIP_CPP
#define _WIP_CPP

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<string>

class IpInfos
{
private:
    /* 返回0表示调用成功 */
    int _System(const std::string cmd, std::string &output)
    {
        FILE *fp;
        int res = -1;
        if ((fp = popen(cmd.c_str(), "r")) == NULL)
        {
            printf("Popen Error!\n");
            return -2;
        }
        else
        {
            char pRetMsg[10240] = {0};
            // get lastest result
            while (fgets(pRetMsg, 10240, fp) != NULL)
            {
                output += pRetMsg;
            }

            if ((res = pclose(fp)) == -1)
            {
                printf("close popenerror!\n");
                return -3;
            }
            return 0;
        }
    }

public:
    std::string GetWIP()
    {
        std::string cmd = "curl -s icanhazip.com";
        int ret = 0;
        std::string result;
        ret = _System(cmd, result);
        if (result.size()) result.pop_back();
        if (ret == 0)
            return result;
        else
            return "ERROR";
    }

    std::string GetNIP()
    {
        std::string cmd = "hostname -I";
        int ret = 0;
        std::string result;
        ret = _System(cmd, result);
        if (result.size()) result.pop_back();
        if (ret == 0)
            return result;
        else
            return "ERROR";
    }
};

#endif