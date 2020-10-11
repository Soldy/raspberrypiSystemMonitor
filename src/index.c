#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


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

float tempDetect(){
    FILE *fpf = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    int tempNow;
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &tempNow);
        fclose(fpf);
        return  ((float) tempNow / (float) 1000);
    }
}

float freqDetect(){
    FILE *fpf = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq", "r");
    int freqNow;
    if((fpf == NULL)){
        printf("Err");
    }else{
        fscanf(fpf,"%d", &freqNow);
        fclose(fpf);
        return  ((float) freqNow / (float) 1000);
    }

}

float cpuUsageDetect(){
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
    return cpuUsage;
}

int main(){
    while(0 == 0){
        clearScreen();
        printf("%.2f\n", tempDetect());
        printf("%.2f\n", freqDetect());
        printf("%.2f\n", cpuUsageDetect());
        fflush(stdout); 
        usleep(500000); 
    }
}
