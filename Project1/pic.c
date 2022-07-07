#include "header/pic.h"


struct pic* print_pic_dir(char *path, int depth)
{
	struct dirent **name_list;
	int n = scandir(path,&name_list,0,alphasort);
    struct pic * pic_list = NULL;
    struct pic * pic_last_ptr = NULL;
    if(n < 0)
    { 
        printf( "scandir return %d \n",n);
    }
    else
    {
        int index=0;
        while(index < n)
        {
            if(index>1)
            {
                struct pic* current_pic = (struct pic*)malloc(sizeof(struct pic));
                current_pic->next = NULL;
                current_pic->last = NULL;
                strcpy(current_pic->name,name_list[index]->d_name);
                // printf("%s\n",current_pic->name);
                // 列表初始化
                if(index == 2)
                {
                    pic_list = current_pic; // 
                    pic_last_ptr = current_pic; 
                }else{
                    current_pic->last = pic_last_ptr;
                    pic_last_ptr->next = current_pic;
                    pic_last_ptr = current_pic;
                }
            }
            free(name_list[index++]);
        }
        free(name_list);
    }
    n = n - 2;
    // printf("个数为：%d\n",n);

    return pic_list;
}

void show_pic(struct pic* header)
{
    if(header->next !=NULL)
    {
        while(header->next != NULL)
        {
            printf("%s\n",header->name);
            header = header->next;
        }
        printf("%s\n",header->name);
    }
    printf("**********************************\n");  
    printf("**********************************\n");     
}

void pic(struct pic* header, struct pic** current)
{
    char mc[50] = {"/Project/pic2/"};
    strcat(mc,(*current)->name);
    sleep(1);
    int fd_pic = open(mc,O_RDWR);
    bmp_process(fd_pic);
    if((*current)->next != NULL)
    {
        *current = (*current)->next;
    }
    else
    {
        *current = header;
    }
    
    close(fd_pic);
}