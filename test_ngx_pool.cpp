
#include"ngx_mem_pool.h"
#include<string>

void func1(void* p1)     //void (*)(void*)
{
    char* p = (char*) p1;
    printf("free ptr mem!\n");
    free(p);
}

void func2(void* pf2)    //void (*)(void*)
{
    FILE* pf = (FILE*) pf2;
    printf("close file!\n");
    fclose(pf);
}

struct stData
{
    char* ptr;
    FILE* pfile;
};

int main() {
    // 512 - sizeof(ngx_pool_t) - 4095   =>   max
    ngx_mem_pool mempool(512);


    void* p1 = mempool.ngx_palloc(128); // 从小块内存池分配的
    if (p1 == nullptr)
    {
        printf("ngx_palloc 128 bytes fail...");
        return -1;
    }

    stData* p2 = (stData*)mempool.ngx_palloc(512); // 从大块内存池分配的
    if (p2 == nullptr)
    {
        printf("ngx_palloc 512 bytes fail...");
        return -1;
    }
    p2->ptr = (char*)malloc(12);    //堆中创建堆区内存
    strcpy_s(p2->ptr, 12 ,"hello world");  //堆中ptr指向堆区字符串  
    fopen_s(&p2->pfile, "1.txt", "w");  //堆中pfile指向磁盘文件

    //调用回调函数进行内存释放
    ngx_pool_cleanup_s* c1 = mempool.ngx_pool_cleanup_add(sizeof(char*));
    c1->handler = func1;
    c1->data = p2->ptr;

    ngx_pool_cleanup_s* c2 = mempool.ngx_pool_cleanup_add(sizeof(FILE*));
    c2->handler = func2;
    c2->data = p2->pfile;


    return 0 ;
}