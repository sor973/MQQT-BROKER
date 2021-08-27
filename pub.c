/*
 use this to compile
 $ gcc -Wall -g funmqqt.c -o funmqqt `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0` -I/usr/include/json-c -ljson-c -lmosquitto -lpthread
*/ 
////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mosquitto.h>
#include <time.h>
#include <inttypes.h>
#include <gtk/gtk.h>
#include "json.h"
#include "json_inttypes.h"
#include "json_object.h"
#include "json_tokener.h"
#define MQTT_SERVER     "localhost"
#define KEEP_ALIVE      60
#define MQTT_PORT       1883
#define MQTT_PUB_TOPIC  "/funmqqt"
GtkWidget *button1, *button2;
 GtkWidget *entry1, *entry2, *entry3, *entry4;
static void button1_callback( GtkWidget *widget, gpointer data ) 
{
 bool clean_session = true;
    struct mosquitto *mosq = NULL;
printf( "Mosquitto - MQTT Client Demo...\n" );
    
    mosquitto_lib_init();
mosq = mosquitto_new( NULL, clean_session, NULL );
    if( !mosq ) 
 {
        printf( "Create client failed..\n");
        mosquitto_lib_cleanup( );
        return 1;
    }
 if (mosquitto_connect(mosq, MQTT_SERVER, MQTT_PORT, KEEP_ALIVE)) 
 {
        fprintf( stderr, "Unable to connect.\n" );
        return 1;
    }
 const gchar *name = gtk_entry_get_text(GTK_ENTRY(entry1));
 const gchar *message = gtk_entry_get_text(GTK_ENTRY(entry2));
 const gchar *name2 = gtk_entry_get_text(GTK_ENTRY(entry3));
 const gchar *message2 = gtk_entry_get_text(GTK_ENTRY(entry4));
 
 json_object *json_obj;
 json_obj = json_tokener_parse( "{ }");
char buff[128];
snprintf( buff, sizeof(buff), "%s", message);
json_object_object_add( json_obj, name, json_object_new_string(buff));
snprintf( buff, sizeof(buff), "%s", message2);
json_object_object_add( json_obj, name2, json_object_new_string(buff));
const char *json_str;
json_str = json_object_get_string( json_obj );
int result;
result = mosquitto_publish( mosq, NULL, MQTT_PUB_TOPIC,strlen(json_str), json_str, 0, 0 );
    if (result != 0) 
 {
  fprintf( stderr, "MQTT publish error: %d\n", result );
 }
 if (mosq) 
 {
        mosquitto_destroy( mosq );
    }
    mosquitto_lib_cleanup();
    printf( "Done..." );
}
static void button2_callback( GtkWidget *widget, gpointer data ) {
 g_print( "Exit\n" );
}
static void activate( GtkApplication *app, gpointer user_data ) {
 GtkWidget *window;
 GtkWidget *box;
window = gtk_application_window_new( app );
 gtk_window_set_title( GTK_WINDOW(window), "MQQT CLIENT" );
 gtk_window_set_default_size( GTK_WINDOW(window), 300, 200 );
 gtk_window_set_position( GTK_WINDOW(window), GTK_WIN_POS_CENTER );
 gtk_container_set_border_width( GTK_CONTAINER(window), 4 );
box = gtk_box_new( GTK_ORIENTATION_VERTICAL, 0 );
    gtk_container_add( GTK_CONTAINER(window), box );
//create button
 button1 = gtk_button_new_with_label( "Send" );
 g_signal_connect( button1, "clicked", G_CALLBACK(button1_callback), button1 );
button2 = gtk_button_new_with_label( "Exit" );
 g_signal_connect( button2, "clicked", G_CALLBACK(button2_callback), NULL );
 g_signal_connect_swapped( button2, "clicked", G_CALLBACK( gtk_widget_destroy), window );
    
 //create entry
 entry1 = gtk_entry_new();
 entry2 = gtk_entry_new();
 entry3 = gtk_entry_new();
 entry4 = gtk_entry_new();
gtk_box_pack_start( GTK_BOX(box), entry1, TRUE, TRUE, 4 );
 gtk_box_pack_start( GTK_BOX(box), entry2, TRUE, TRUE, 4 );
 gtk_box_pack_start( GTK_BOX(box), entry3, TRUE, TRUE, 4 );
 gtk_box_pack_start( GTK_BOX(box), entry4, TRUE, TRUE, 4 );
 
 gtk_box_pack_start( GTK_BOX(box), button1, TRUE, TRUE, 4 );
 gtk_box_pack_start( GTK_BOX(box), button2, TRUE, TRUE, 4 );
gtk_widget_show_all( window );
}
int main( int argc, char **argv ) {
    GtkApplication *app;
    app = gtk_application_new( NULL, G_APPLICATION_FLAGS_NONE );
    g_signal_connect( app, "activate", G_CALLBACK(activate), NULL );
    // start the application main loop (blocking call)
 g_application_run( G_APPLICATION(app), argc, argv );
 // decrease the reference count to the object
 g_object_unref( app );
}
