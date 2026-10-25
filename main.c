#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>



struct file_buffer{

char* buffer;
int fd;

};



int fileio(int buf_size , struct file_buffer *file_buffer_instance){



char* buffer = calloc(buf_size,sizeof(char));

if(buffer==NULL){
 write(1,"Error in Memory Allocation",22);  
 abort(); 
}



int fd = open("hello.txt",O_RDONLY);



if(fd == -1){

write(1,"Error in opening file",22);
abort();
}

int bytes_read;


if((bytes_read = read(fd , buffer ,buf_size))== -1){
 write(1,"Error in reading file", 22);
    abort();
}




file_buffer_instance->buffer = buffer;

file_buffer_instance->fd = fd; 




return bytes_read;

}


int boyeprinter(char* buffer, int buf_size , int fd ){




int arr[26]; 


char* key = "world";


for(size_t i = 0;i<strlen(key);i++)

if((key[i]-'a') < 0 || (key[i]-'a') > 25){

  write(1,"Key must be in lowercase",25 ); 
  abort();

}

if(strlen(key)<1 || strlen(key)>(INT_MAX-1)){
   
    write(1,"Key too large",14); 
    abort();

}


size_t count =  0;


int ptr2 = strlen(key) - 1; // Potential Overflow



for(int i = 0 ; i < 26 ; i++){


arr[i] = strlen(key);

}



int i = 0;







for(size_t i = 0 ; i< (strlen(key)-1) ; i++){
        
      arr[key[i]-'a'] = strlen(key) -i - 1; }




while(i < buf_size){
 
if(INT_MAX-i==  ptr2){
  write(1,"File Too Large", 15);
  abort(); 
}



if(i+ptr2 >= buf_size){
  write(1,"Word Not Found", 15);

    break;
    
}








while(ptr2+i<buf_size && buffer[ptr2+i]==key[ptr2] ){

    ptr2--;
    count++;




if(count==strlen(key)){
    printf("Found String at %d", i);
    return 0;
}

}


if((buffer[i+ptr2]-'a') < 0 || (buffer[i+ptr2]-'a') > 25){
i++;
count = 0;
ptr2 = strlen(key) - 1;


continue;
}



count = 0;
ptr2 = strlen(key) - 1;

i = i + arr[buffer[i + ptr2]-'a'];
}


return fd;
}








int main(){
static struct file_buffer file_buffer_instance; 


int buf_size = 1024*4;



int bytes_read = fileio(buf_size,&file_buffer_instance);



boyeprinter(file_buffer_instance.buffer,bytes_read,file_buffer_instance.fd);

close(file_buffer_instance.fd);
free(file_buffer_instance.buffer);

return 0;
}
