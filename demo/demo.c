#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "../src/zringbuffer.h"

uint8_t Cache[1024];
zrb_queue_t RB_Que;

void print_data(uint8_t *_data,uint16_t _size);

void main(void)
{
    uint8_t buf[128],data[1024];
    uint16_t len = 0;
    zRB_init(&RB_Que,Cache,sizeof(Cache));

    for(int i = 0;i<128;i++){
        buf[i] = i;
    }

    for(int i = 0;i<10;i++){
        printf("Round : %d\n",i);
        zRB_put(&RB_Que,buf,128);
        zRB_get(&RB_Que,data,50);
        print_data(data,50);
        len = zRB_remianSize(&RB_Que);
        printf("Remain Size : %d\n",len);
        memset(data,0,sizeof(data));
        zRB_read(&RB_Que,data,len);
        print_data(data,len);
    }
}

void print_data(uint8_t *_data,uint16_t _size)
{
    if(_data == 0 || _size == 0){
        goto end;
    }

    printf("Data : \n  ");
    for(int i = 0;i<_size;i++){
        printf("0x%x ",_data[i]);
        if(i % 16 == 15){
            printf("\n  ");
        }
    }
    printf("\n")

end:
    return;
}