
#include "KafkaServer.h"

int main()
{
    CKafkaServer KafkaServer;

    if (!KafkaServer.StartKafkaServer())
    {
        printf("****Error: AnalyseService Start Failed!\n");
        printf("Press any key to stop...\n");
        getchar();
    }


    return 0;
}

