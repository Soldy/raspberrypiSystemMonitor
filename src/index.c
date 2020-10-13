#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sqlite3.h>    // SQLite header (from /usr/include)



sqlite3 *db;        // database connection
sqlite3_stmt *stmt;                                                                         /* 1 */
char *zErrMsg = 0;
int rc;             // return code
char *errmsg;       // pointer to an error string
float cpuUsage, lastCpuUsage;
float memUsage, lastMemUsage;
time_t rawtime;
char str[100];
const char d[2] = " ";
char* token;
int i = 0,times,lag;
long int sum = 0, idle, lastSum = 0,lastIdle = 0;
long double idleFraction;

static int sqlcallback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
     return 0;
}

void sqlconnect(){
    char *sql = "CREATE TABLE IF NOT EXISTS performance " \
        "("  \
        "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
        "timestamp INTEGER UNIQUE, " \
        "cputemp INTEGER NOT NULL, " \
         "cpuusage INTEGER NOT NULL, " \
        "cpuclock INTEGER NOT NULL" \
    ");";
    rc = sqlite3_open("test.db", &db);
    rc = sqlite3_exec(db, sql, sqlcallback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        sqlite3_free(zErrMsg);
    } else {
    }
    sqlite3_close(db);
}

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
    char buffer[50] = "";
    char *sql = "INSERT INTO performance " \
        "(timestamp, cputemp, cpuusage, cpuclock)" \
        " VALUES " \
        "(%d, %d, %d, %d);";
    sqlconnect();
    tableMake();
    while(0 == 0){
        if(round == 30){
           tableMake();
           round = 0;
        }else{
           round++;
        }
        rc = sqlite3_open("test.db", &db);
        sprintf(
            buffer, 
            sql,
            (int)time(NULL),
            tempDetect(), 
            cpuUsageDetect(),
            freqDetect()
        );  // faster than the prepare statemanet and that just numbers
        fflush(stdout); 
        rc = sqlite3_exec(db, buffer, sqlcallback, 0, &zErrMsg);
        sqlite3_close(db);
        usleep(1000000); 
    }
}
