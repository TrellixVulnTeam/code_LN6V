#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/mman.h>  
#include <sys/stat.h>  

typedef struct{  
  char name[20];  
  short age;  
  float score;  
  char sex;  
}student;  

bool init_file(int count)
{
	int fd=open("user.dat",O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	student st;
	for(int i = 0; i < count; ++i)
	{
		memset(&st, 0x00, sizeof(student));
		snprintf(st.name, 20, "name%d", i);
		st.age = i;
		st.score = 1.0;
		st.sex = '1';
		write(fd, &st, sizeof(student));
	}
	close(fd);
}

int main()  
{  
	init_file(8);

	student *p,*pend;    
	//���ļ���������  
	int fd;  
	/*���ļ�*/  
	fd=open("user.dat",O_RDWR);  
    if(fd==-1){//�ļ�������  
        fd=open("user.dat",O_RDWR|O_CREAT,0666);  
        if(fd==-1){  
            printf("�򿪻򴴽��ļ�ʧ��:%m\n");  
            exit(-1);  
        }  
    } 
 
  	//���ļ�ok�����Խ�����һ������  
  	printf("open ok!\n");    

  	//��ȡ�ļ��Ĵ�С��ӳ��һ����ļ���Сһ�����ڴ�ռ䣬����ļ��Ƚϴ󣬿��Էֶ�Σ�һ�ߴ���һ��ӳ�䣻  
  	struct stat st; //�����ļ���Ϣ�ṹ��  

  	/*ȡ���ļ���С*/  
  	int r=fstat(fd,&st);  
  	if(r==-1){  
      	printf("��ȡ�ļ���Сʧ��:%m\n");  
      	close(fd);  
      	exit(-1);  
  	}
  
  int len=st.st_size;      
  /*���ļ�ӳ��������ڴ��ַ*/  
  p=(student *)mmap(NULL,len,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);      
  if(p==NULL || p==(void*)-1){  
      printf("ӳ��ʧ��:%m\n");  
      close(fd);  
      exit(-1);  
  }  


  /*��λ���ļ���ʼ*/  
  pend=p;   
  /*ͨ���ڴ��ȡ��¼*/  
  int i=0;  
  while(i<(len/sizeof(student)))  
  {  
    printf("index [%d]\n",i);  
    printf("name=%s\n",p[i].name);  
    printf("age=%d\n",p[i].age);  
    printf("score=%f\n",p[i].score);  
    printf("sex=%c\n",p[i].sex);  
    i++;  
  }    
  /*ж��ӳ��*/  
  munmap(p,len);  
  /*�ر��ļ�*/      
  close(fd);      
}  
