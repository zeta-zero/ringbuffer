/*--------------------------------------------------------------------
 @file           : zringbuffer.c
 @brief          : The program is a ring buffer.
                   The main function is to temporarily store and read data.
---------------------------------------------------------------------
 Release Version : 1.0.0
 Release Data    : 2024/03/21
---------------------------------------------------------------------
@attention :
    Copyright zhangpeiwei02@gmail.com and TEXEG

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
     http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
--------------------------------------------------------------------*/

#ifndef __ZRINGBUFFER_H__
#define __ZRINGBUFFER_H__
//INCLUDE------------------------------------------------------------
#include "stdint.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif
//DEFINE ------------------------------------------------------------
#define ZRB_ERR_OK      0x00   //一切正常
#define ZRB_ERR_PARAM   0x01   //输入的参数错误
#define ZRB_ERR_INIT    0x02   //queue结构体数据未初始化
#define ZRB_ERR_FULL    0x03   //缓冲区数据已满
#define ZRB_ERR_NS      0x04   //no sufficent,缓冲区空间不足
#define ZRB_ERR_NDATA   0x05   //no data，缓冲区中没有数据


//TYPEDEF------------------------------------------------------------
typedef struct{
    uint8_t *Buffer;
    uint16_t Length;
    uint16_t RemainSize;
    uint16_t Wpos;
    uint16_t Rpos;
    uint16_t Offset;
    struct {
        uint16_t InitFlag:4;
        uint16_t Remian:12;
    }Stat;
}zrb_queue_t;

uint8_t zRB_init(zrb_queue_t *_queue,uint8_t* _cache,uint16_t _size);
uint8_t zRB_put(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len);
uint8_t zRB_insert(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len);
uint8_t zRB_get(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len,uint16_t *_rlen);
uint8_t zRB_read(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len,uint16_t *_rlen);
uint16_t zRB_remianSize(zrb_queue_t *_queue);
uint8_t zRB_clean(zrb_queue_t *_queue);
uint8_t zRB_remove(zrb_queue_t* _queue, uint16_t _len);
uint8_t zRB_lseek(zrb_queue_t* _queue, int _offset);
uint8_t zRB_resetOffset(zrb_queue_t* _queue);

#ifdef __cplusplus
}
#endif
#endif //__ZRINGBUFFER_H__


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/
