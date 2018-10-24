#include "KafkaServer.h"

int main(int argc, char * argv[])
{
    int nServerType = 1;
    if (argc == 2)
    {
        nServerType = atoi(argv[1]);
    }
    else
    {
        printf("****Error: Param Lost...\n");
        return 0;
    }

    if (nServerType < 0 || nServerType > 4)
    {
        printf("****Error: Server Type Not Found!\n");
        return 0;
    }

    CKafkaServer KafkaServer;

    if (!KafkaServer.StartKafkaServer(nServerType))
    {
        printf("****Error: AnalyseService Start Failed!\n");
        printf("Press any key to stop...\n");
        getchar();
    }


    return 0;
}

