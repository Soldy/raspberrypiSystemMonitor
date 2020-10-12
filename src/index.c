#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>


float cpuUsage, lastCpuUsage;
float memUsage, lastMemUsage;
time_t rawtime;
char str[100];
const char d[2] = " ";
char* token;
int i = 0,times,lag;
long int sum = 0, idle, lastSum = 0,lastIdle = 0;
long double idleFraction;

void clearScreen(){
    printf("\033[2J");
    printf("\033[H");
}

void tableMake(){
    clearScreen();
    printf("\033[2;2H ┌────────────────────────────────┐");
    printf("\033[3;2H │  CPU Tempeture :               │");
    printf("\033[4;2H │      CPU Usage :               │");
    printf("\033[5;2H │      CPU Clock :               │");
    printf("\033[6;2H │         Memory :               │");
    printf("\033[7;2H │          Count :               │");
    printf("\033[8;2H └────────────────────────────────┘");
    printf("\033[3;30HC");
    printf("\033[4;30H%%");
    printf("\033[5;30HMhz");

}

int tempDetect(){
    FILE *fpf = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    int tempNow;
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &tempNow);
        fclose(fpf);
        printf("\033[3;22H %.2f", ((float) tempNow / (float) 1000));
        return tempNow;
    }
}


int freqDetect(){
    FILE *fpf = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", "r");
    int freqNow;
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &freqNow);
        fclose(fpf);
        printf("\033[5;22H %.d", ((int) freqNow / (int) 1000));
        return   freqNow ;
    }
    return 0;

}

int cpuUsageDetect(){
    FILE *fpf = fopen("/proc/stat","r");
    fgets(str,100,fpf);
    fclose(fpf);
    token = strtok(str,d);
    sum=0;
    i=0;
    while(token!=NULL){
        token = strtok(NULL,d);
        if((token!=NULL)){
            sum += atoi(token);
            if(i==3)
                idle = atoi(token);
        }
        i++;
    }
    cpuUsage = ((1.0 - (float)((float)(idle-lastIdle)/(float)(sum-lastSum)))*100);
    lastIdle = idle;
    lastSum = sum;
    lastCpuUsage = cpuUsage;
    printf("\033[4;22H %.2f", cpuUsage);
    return ((int)(cpuUsage*100));
}

int main(){
    int round = 0;
    tableMake();
    while(0 == 0){
        if(round == 30){
           tableMake();
           round = 0;
        }else{
           round++;
        }
        tempDetect();
        freqDetect();
        cpuUsageDetect();
        fflush(stdout); 
        usleep(1000000); 
    }
}
