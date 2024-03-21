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
#include "zringbuffer.h"

//DEFINE ------------------------------------------------------------
#define ZRINGBUFFER_INITFLAG   0x5

#define ZRB_ASSERT_CHECK(_ERRRES_,_CONDITION_) if(_CONDITION_){res = _ERRRES_;goto finally;}
#define ZRB_READ(_QUEUE_,_LEN_,_PART1_,_PART2_)  \
if(_QUEUE_->Rpos < _QUEUE_->Wpos){ /* S--R--W--E */  \
    if(_QUEUE_->Wpos - _QUEUE_->Rpos > _LEN_){_PART1_ = _LEN_;} \
    else{_PART1_ = _QUEUE_->Wpos - _QUEUE_->Rpos;} \
    _PART2_ = 0; \
} \
else{ /* S--W--R--E */\
    if((_QUEUE_->Length - _QUEUE_->Rpos) > _LEN_){_PART1_ = _LEN_;_PART2_ = 0;} \
    else{ \
        _PART1_ = _QUEUE_->Length - _QUEUE_->Rpos; \
        if(_LEN_ - _PART1_ < _QUEUE_->Wpos){_PART2_ = _LEN_ - _PART1_;}\
        else{_PART2_ = _QUEUE_->Wpos;}\
    } \
}

#define ZRB_READ_OFFSET(_QUEUE_,_LEN_,_PART1_,_PART2_)  \
if(_QUEUE_->Offset < _QUEUE_->Wpos){ /* S--R--W--E */  \
    if(_QUEUE_->Wpos - _QUEUE_->Offset > _LEN_){_PART1_ = _LEN_;} \
    else{_PART1_ = _QUEUE_->Wpos - _QUEUE_->Offset;} \
    _PART2_ = 0; \
} \
else{ /* S--W--R--E */\
    if((_QUEUE_->Length - _QUEUE_->Offset) > _LEN_){_PART1_ = _LEN_;_PART2_ = 0;} \
    else{ \
        _PART1_ = _QUEUE_->Length - _QUEUE_->Offset; \
        if(_LEN_ - _PART1_ < _QUEUE_->Wpos){_PART2_ = _LEN_ - _PART1_;}\
        else{_PART2_ = _QUEUE_->Wpos;}\
    } \
}

/**-------------------------------------------------------------------
 * @fn     : zRB_init
 * @brief  : 初始化RingBuffer
 * @param  : _queue   环形缓冲区结构体
 *           _cache   缓冲区地址
 *           _size    缓冲区大小
 * @return : res
 */
uint8_t zRB_init(zrb_queue_t *_queue,uint8_t* _cache,uint16_t _size)
{
    uint8_t res = ZRB_ERR_OK;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0 || _cache == 0 || _size == 0);

    memset(_queue,0,sizeof(zrb_queue_t));
    _queue->Rpos = 0;
    _queue->Wpos = 0;
    _queue->Offset = 0;
    _queue->Length = _size;
    _queue->RemainSize = _size;
    _queue->Buffer = _cache;
    _queue->Stat.InitFlag = ZRINGBUFFER_INITFLAG;


finally:
    return res;
}

/**-------------------------------------------------------------------
 * @fn     : zRB_put
 * @brief  : 将数据放入环形缓冲区中
 * @param  : _queue  初始化的结构体
 *           _data   待写入的数据
 *           _len    待写入的长度
 * @return : res
 */
uint8_t zRB_put(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len)
{
    uint8_t res = ZRB_ERR_OK;
    uint16_t wpart1=0,wpart2=0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0 || _data == 0 || _len == 0 || _queue->Buffer == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT,_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);
    ZRB_ASSERT_CHECK(ZRB_ERR_FULL,_queue->RemainSize == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_NS,_queue->RemainSize < _len);

    // model : S---R--w---E
    if(_queue->Wpos >= _queue->Rpos){
        //  WE区间足够写入数据
        if(_queue->Length > _queue->Wpos + _len){
            wpart1 = _len;
            wpart2 = 0;
        }
        //  WE区间不够写入数据
        else{
            wpart1 = _queue->Length - _queue->Wpos;
            if(_queue->Rpos > _len - (_queue->Length + _queue->Wpos)){
                wpart2 = _len - wpart1;
            }
            else{
                wpart2 = _queue->Rpos;
            }
            
        }
    }
    // model : S---W--R---E
    else{
        wpart1 = _len;
        wpart2 = 0;
    }
    
    if(wpart1 != 0)
    {
        memcpy(&_queue->Buffer[_queue->Wpos],_data,wpart1);
        _queue->Wpos += wpart1;
    }
    
    if(wpart2 != 0){
        memcpy(_queue->Buffer,&_data[wpart1],wpart2);
        _queue->Wpos = wpart2;
    }
    if(_queue->Wpos > _queue->Length - 1){
        _queue->Wpos = 0;
    }
    if(_queue->RemainSize > _len){
        _queue->RemainSize -= _len;
    }
    else{
        _queue->RemainSize = 0;
    }
finally:
    return res;
}


/**-------------------------------------------------------------------
 * @fn     : zRB_insert
 * @brief  : 将数据插入入环形缓冲区中
 * @param  : _queue  初始化的结构体
 *           _data   待写入的数据
 *           _len    待写入的长度
 * @return : res
 */
uint8_t zRB_insert(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len)
{
    uint8_t res = ZRB_ERR_OK;
    uint16_t wpart1=0,wpart2=0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0 || _data == 0 || _len == 0 || _queue->Buffer == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT,_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);
    ZRB_ASSERT_CHECK(ZRB_ERR_FULL,_queue->RemainSize == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_NS,_queue->RemainSize < _len);

    // model : S---R--w---E
    if(_queue->Wpos >= _queue->Rpos){
        //  SR区间足够写入数据
        if(_len < _queue->Rpos){
            wpart1 = _len;
            wpart2 = 0;
        }
        //  WE区间不够写入数据
        else{
            wpart1 = _queue->Rpos;
            wpart2 = _len - wpart1;
        }
    }
    // model : S---W--R---E
    else{
        wpart1 = _len;
        wpart2 = 0;
    }
    
    if(wpart1 != 0)
    {
        memcpy(&_queue->Buffer[_queue->Rpos-wpart1],_data,wpart1);
        _queue->Rpos -= wpart1;
    }
    if(wpart2 != 0){
        memcpy(_queue->Buffer,&_data[_queue->Length-wpart2],wpart2);
        _queue->Rpos = wpart2;
    }
    _queue->Offset = _queue->Rpos;
    _queue->RemainSize -= _len;
finally:
    return res;
}


/**-------------------------------------------------------------------
 * @fn     : zRB_get
 * @brief  : 从环形缓冲区中提取数据
 * @param  : _queue  初始化的结构体
 *           _data   待读取数据的空间
 *           _len    待读取数据的空间长度
 *           _rlen   冲缓冲区中读取数据的长度
 * @return : res
 */
uint8_t zRB_get(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len,uint16_t *_rlen)
{
    uint8_t res = ZRB_ERR_OK;
    uint16_t rpart1=0,rpart2=0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0 || _data == 0 || _len == 0 || _queue->Buffer == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT,(_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG));
    ZRB_ASSERT_CHECK(ZRB_ERR_NDATA,_queue->RemainSize == _queue->Length);

    ZRB_READ(_queue,_len,rpart1,rpart2);

    if(rpart1 != 0)
    {
        memcpy(_data,&_queue->Buffer[_queue->Rpos],rpart1);
        _queue->Rpos += rpart1;
    }
    if(rpart2 != 0)
    {  
       memcpy(&_data[rpart1],_queue->Buffer,rpart2);
       _queue->Rpos = rpart2;
    }
    if(_rlen != 0){
        *_rlen = rpart1 + rpart2;
    }
    if((*_rlen) + _queue->RemainSize > _queue->Length){
        _queue->RemainSize = _queue->Length;
    }
    else{
        _queue->RemainSize += (*_rlen);
    }
    _queue->Offset = _queue->Rpos;
finally:
    return res;
}

/**-------------------------------------------------------------------
 * @fn     : zRB_remove
 * @brief  : 删除缓冲区前段数据
 * @param  : _queue  初始化的结构体
 *           _len    待删除数据的长度
 * @return : 
 */
uint8_t zRB_remove(zrb_queue_t *_queue,uint16_t _len)
{
    uint8_t res = 0;
    uint16_t rpart1=0,rpart2=0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0 || _len == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT,_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);
    ZRB_ASSERT_CHECK(ZRB_ERR_NDATA,_queue->RemainSize == _queue->Length);

    ZRB_READ(_queue,_len,rpart1,rpart2);

    if(rpart1 != 0)
    {
        _queue->Rpos += rpart1;
    }
    if(rpart2 != 0)
    {  
       _queue->Rpos = rpart2;
    }
    if((rpart1 + rpart2) + _queue->RemainSize > _queue->Length){
        _queue->RemainSize = _queue->Length;
    }
    else{
        _queue->RemainSize += rpart1 + rpart2;
    }
    _queue->Offset = _queue->Rpos;

finally:
    return res;
}

/**-------------------------------------------------------------------
 * @fn     : zRB_lseek
 * @brief  : 移动读取的位置，仅适用于 zRB_read 函数
 * @param  : _queue    初始化的结构体
 *           _offset   偏移位置，不超过Rpos与Wpos之间的范围
 * @return :
 */
uint8_t zRB_lseek(zrb_queue_t* _queue, int _offset)
{
    uint8_t res = 0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM, _queue == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT, _queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);

    if (_queue->Offset < _queue->Wpos) {
        if (_offset > _queue->Wpos - _queue->Offset) {
            _queue->Offset = _queue->Wpos;
        }
        else if (_queue->Offset + _queue->Length - _queue->Rpos < -_offset) {
            _queue->Offset = _queue->Rpos;
        }
        else if (_queue->Offset > _queue->Rpos &&_queue->Offset - _queue->Rpos < -_offset) {
            _queue->Offset = _queue->Rpos;
        }
        else if (_queue->Offset < -_offset) {
            _queue->Offset += _queue->Length + _offset;
        }
        else {
            _queue->Offset += _offset;
        }
    }
    else if (_queue->Offset >= _queue->Rpos) {
        if (_queue->Offset - _queue->Rpos < -_offset) {
            _queue->Offset = _queue->Rpos;
        }
        else if (_offset > _queue->Wpos + _queue->Length - _queue->Offset) {
            _queue->Offset = _queue->Wpos;
        }
        else if (_queue->Offset + _offset > _queue->Length - 1) {
            _queue->Offset += _offset - _queue->Length;
        }
        else {
            _queue->Offset += _offset;
        }
    }

finally:
    return res;
}


/**-------------------------------------------------------------------
 * @fn     : zRB_resetOffset
 * @brief  : 重置offset为RPos的位置
 * @param  : _queue    初始化的结构体
 * @return :
 */
uint8_t zRB_resetOffset(zrb_queue_t* _queue)
{
    uint8_t res = 0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM, _queue == 0);
    _queue->Offset = _queue->Rpos;
finally:
    return res;

}

/**-------------------------------------------------------------------
 * @fn     : zRB_read
 * @brief  : 从环形缓冲区中读取数据，且数据任保留在缓冲区中
 * @param  : _queue  初始化的结构体
 *           _data   待读取数据的空间
 *           _len    待读取数据的空间长度
 *           _rlen   冲缓冲区中读取数据的长度
 * @return : 
 */
uint8_t zRB_read(zrb_queue_t *_queue,uint8_t* _data,uint16_t _len,uint16_t *_rlen)
{
    uint8_t res = ZRB_ERR_OK;
    uint16_t rpart1=0,rpart2=0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0 || _data == 0 || _len == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT,_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);
    ZRB_ASSERT_CHECK(ZRB_ERR_NDATA,_queue->RemainSize == _queue->Length);

    ZRB_READ_OFFSET(_queue,_len,rpart1,rpart2);

    if(rpart1 != 0)
    {
        memcpy(_data,&_queue->Buffer[_queue->Offset],rpart1);
    }
    if(rpart2 != 0)
    {  
       memcpy(&_data[rpart1],_queue->Buffer,rpart2);
    }
    if(_rlen != 0)
        *_rlen = rpart1 + rpart2;
finally:
    return res;
}

/**-------------------------------------------------------------------
 * @fn     : zRB_remianSize
 * @brief  : 获得剩余容量大小
 * @param  : _queue  初始化的结构体
 * @return : 
 */
uint16_t zRB_remianSize(zrb_queue_t *_queue)
{
    uint16_t res = 0;
    ZRB_ASSERT_CHECK(0,_queue == 0);
    ZRB_ASSERT_CHECK(0,_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);

    res = _queue->RemainSize;

finally:
    return res;
}

/**-------------------------------------------------------------------
 * @fn     : zRB_clean
 * @brief  : 清空缓冲区
 * @param  : _queue  初始化的结构体
 * @return : 
 */
uint8_t zRB_clean(zrb_queue_t *_queue)
{
    uint16_t res = 0;
    ZRB_ASSERT_CHECK(ZRB_ERR_PARAM,_queue == 0);
    ZRB_ASSERT_CHECK(ZRB_ERR_INIT,_queue->Stat.InitFlag != ZRINGBUFFER_INITFLAG);
    _queue->Rpos = 0;
    _queue->Wpos = 0;
    _queue->Offset = 0;
    _queue->RemainSize = _queue->Length;

finally:
    return res;
}

/**-------------------------------------------------------------------
 * @fn     : 
 * @brief  : 
 * @param  : 
 * @return : 
 */


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/
