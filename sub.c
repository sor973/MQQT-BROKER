/*
 use this to compile
 $ gcc -Wall -g funmqqt2.c -o funmqqt2 `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -I/usr/include/json-c -ljson-c -lmosquitto -lpthread
*/ 
////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mosquitto.h>
#define MQTT_SERVER     "localhost"
#define MQTT_PORT       1883
#define KEEP_ALIVE      60
#define MQTT_QOS_LEVEL  2
#define MQTT_SUB_TOPIC  "/funmqqt"
bool is_connected = false;
bool is_subscribed = false;
bool is_running = true;
// forward declarations
void on_connect_cb( 
    struct mosquitto *mosq, void *userdata, int result );
    
void on_subscribe_cb(
    struct mosquitto *mosq, void *userdata, int mid, 
    int qos_count, const int *granted_qos );
void on_message_cb( 
    struct mosquitto *mosq, void *userdata, 
    const struct mosquitto_message *msg );
int main( int argc, char *argv[]) {
 bool clean_session = true;
    struct mosquitto *mosq = NULL;
    printf( "Mosquitto - MQTT Client Demo...\n" );
    mosquitto_lib_init();
    mosq = mosquitto_new( NULL, clean_session, NULL );
    if( !mosq ) {
        printf( "Create client failed..\n");
        mosquitto_lib_cleanup( );
        return 1;
    }
    
    mosquitto_connect_callback_set( mosq, on_connect_cb );
    mosquitto_subscribe_callback_set( mosq, on_subscribe_cb );
    mosquitto_message_callback_set( mosq, on_message_cb );
if (mosquitto_connect(mosq, MQTT_SERVER, MQTT_PORT, KEEP_ALIVE)) {
        fprintf( stderr, "Unable to connect.\n" );
        return 1;
    }
    
    int result;
    result = mosquitto_loop_start(mosq); 
    if (result != MOSQ_ERR_SUCCESS) {
        printf("mosquitto loop error\n");
        return 1;
    }
while (!is_connected) {
  usleep(100000);
    }
    
    mosquitto_subscribe( mosq, NULL, MQTT_SUB_TOPIC, MQTT_QOS_LEVEL );
    
    while (!is_subscribed) {
  usleep(100000);
    }
    
    while(is_running) {
        usleep(100000);
    }
  
    
    if (mosq) {
       mosquitto_destroy( mosq );
    }
    mosquitto_lib_cleanup();
 return 0;
}
void on_connect_cb(
    struct mosquitto *mosq, void *userdata, int result )
{
    if (result!=0) {
        fprintf( stderr, "Connect failed\n" );
    } else{
  is_connected = true;
        printf( "Connect OK\n" );
    }
}
    
void on_subscribe_cb(
    struct mosquitto *mosq, void *userdata, int mid, 
    int qos_count, const int *granted_qos )
{
    is_subscribed = true;
 printf( "Subscribe OK, (QOS=%d)\n", granted_qos[0] );
}
void on_message_cb( 
    struct mosquitto *mosq, void *userdata, 
    const struct mosquitto_message *msg )
{
    FILE *fp;
    fp = fopen("log.txt","a");
    int len = msg->payloadlen;
    if ( len > 0 ) {
        printf( "%s\n",(char *)msg->payload );
        fprintf(fp, "%s\n", (char *)msg->payload);
    }
    else {
        is_running = false;
    }
    fclose(fp);
    fflush( stdout );
}
