# ringbuffer
 A simple ring buffer (circular buffer) designed for embedded systems.


## How To Use

### 1. A new ring buffer is created using the  zRB_init(&rb_queue, cache, sizeof(cache)) function:

```C
    uint8_t cache[1024];
    zrb_queue_t rb_queue;
    zRB_init(&rb_queue,cache,sizeof(cache));
```

### 2. Put Data Into RingBuffer using the zRB_put(&rb_queue,data,len) function:

```C
    uint8_t data[5] = {1,2,3,4,5};
    zRB_put(&rb_queue,data,5);
```

### 3. Get Data From RingBuffer Usng the zRB_get(&rb_queue,buf,len) function:

```C
    uint8_t buf[5] = {0};
    zRB_put(&rb_queue,buf,4);
```

The array of buf data : 1,2,3,4,0

Note: This function will removes the read data.

### 4. Read Data From RingBuffer Using the zRB_read(&rb_queue,buf,len) function:

```C
    uint8_t buf[5] = {0};
    zRB_read(&rb_queue,buf,5);
```

Note: This function does not remove read data.

### 5. Check Remain Size of the RingBuffer Using the zRB_remianSize(&rb_queue) function:

```C
    printf("The Ring Buffer Remain : %d\n",zRB_remianSize(&rb_queue));
```

### 6. Clean All Data of the RingBuffer Using the zRB_clean(&rb_queue) function:

```C
    zRB_clean(&rb_queue);
    printf("The Ring Buffer Remain : %d\n",zRB_remianSize(&rb_queue));
```

### 7. Remove Some Data of the RingBuffer Using the zRB_remove(&rb_queue,len) function:

```C
    zRB_remove(&rb_queue,3);
```

If there are 10 bytes of data in the RingBuffer, it removes the first 3 bytes.  
Example:  
    RingBuffer Data - 1,2,3,4,5,6,7,8,9,10  
    Remove 3 after  - 4,5,6,7,8,9,10

### 8. Read data more flexibly.

```C
    uint8_t buf[5] = {0};
    zRB_lseek(&rb_queue,3);
    zRB_read(&rb_queue,buf,5);

    
    zRB_resetOffset(&rb_queue);
    zRB_read(&rb_queue,buf,5);
```
Example :   
    RingBuffer Data - a,b,c,d,e,f,g,h  
    First Operation And Read 5 - d,e,f,g,h  
    Second Operation ANd Read 5 - a,b,c,d,e  

## API
The module provides the following functions for accessing the ring buffer (documentation can be found in zringbuffer.h):

```C
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
```