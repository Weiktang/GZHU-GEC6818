#include "header/music.h"


struct music* print_dir(char *path, int depth)
{
	struct dirent **name_list;
	int n = scandir(path,&name_list,0,alphasort);
    struct music * music_list = NULL;
    struct music * music_last_ptr = NULL;
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
                struct music* current_music = (struct music*)malloc(sizeof(struct music));
                current_music->next = NULL;
                current_music->last = NULL;
                strcpy(current_music->name,name_list[index]->d_name);
                // printf("%s\n",current_music->name);
                // 列表初始化
                if(index == 2)
                {
                    music_list = current_music; // 
                    music_last_ptr = current_music; 
                }else{
                    current_music->last = music_last_ptr;
                    music_last_ptr->next = current_music;
                    music_last_ptr = current_music;
                }
            }
            free(name_list[index++]);
        }
        free(name_list);
    }
    n = n - 2;
    // printf("个数为：%d\n",n);

    return music_list;
}

void show_music(struct music* header)
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
    system("killall -KILL madplay");
    printf("**********************************\n");     
}

void music(int statue, struct music* header, struct music** current,int* STOP)
{
    char mc[50] = {"madplay /Project/music/"};

    if(statue == 0)// last
    {
        if(*current == header)
        {
            printf("this is the first one\n"); // 最上面一首
        }else
        {
            *current = (*current)->last;
            printf("%s\n",(*current)->name);
            strcat(mc,(*current)->name);
            strcat(mc," &");
            system("killall -KILL madplay &");// kill
            sleep(1);
            system(mc); // restart
        }
    }
    else if(statue == 1)// stop
    {
        if(*STOP == 0)
        {
            system("killall -STOP madplay &");
            printf("stop\n");
            *STOP = 1;
        }  
    }
    else if(statue == 2)// start
    {
        if(*STOP == 0)
        {
            printf("%s\n",(*current)->name);
            strcat(mc,(*current)->name);
            strcat(mc," &");
            system(mc);
        }
        else
        {
            system("killall -CONT madplay &");
            printf("restar\n");
            *STOP = 0;
        }
    }
    else if(statue == 3)// next
    {
        if((*current)->next != NULL)
        {
            *current = (*current)->next;
            printf("%s\n",(*current)->name);
            strcat(mc,(*current)->name);
            strcat(mc," &");
            system("killall -KILL madplay &");
            sleep(1);
            system(mc); 
        }
        else
        {
            printf("this is the final one\n");
        }
    }
    else if(statue == 4)
    {
        system("killall -KILL madplay &");
        printf("Kill the music\n");
    }
    else
    {
        printf("exit music\n");
        // system("killall -KILL madplay &");
    }
}

int voice(audio_volume_action action, long* outvol)
{
    int ret = 0;
    int fd, devs;

    if ((fd = open("/dev/dsp", O_WRONLY)) > 0)
    {
        if(action == AUDIO_VOLUME_SET) {
            if(*outvol < 0 || *outvol > 100)
                return -2;
            *outvol = (*outvol << 8) | *outvol;
            ioctl(fd, SOUND_MIXER_WRITE_VOLUME, outvol);
        }
        else if(action == AUDIO_VOLUME_GET) {
            ioctl(fd, SOUND_MIXER_READ_VOLUME, outvol);
            *outvol = *outvol & 0xff;
        }
        close(fd);
        return 0;
    }
    return -1;
}
