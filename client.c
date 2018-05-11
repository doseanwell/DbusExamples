#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "minminbus.h"


/* Determines if to continue the timer or not */
static gboolean continue_timer = FALSE;

static void AsyncCall_back(MinMinBusGDBUS *proxy, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL; // important 
    gchar **buf1;
    gboolean result;
    
    result = min_min_bus_gdbus_call_hello_world_finish(proxy, NULL, res, &error);

    g_assert_no_error (error);
    
    if(result==TRUE)
    {
        char* res1 = (char*) user_data;
        g_print("CALL SUCCESS\n");
        g_print("Call user_data : %s\n", res1);
    }
}

static gboolean timer_cb(gpointer user_data)
{
    MinMinBusGDBUS *proxy = (MinMinBusGDBUS *) user_data;
    min_min_bus_gdbus_call_hello_world(proxy, "Timer-Client1", NULL, (GAsyncReadyCallback) AsyncCall_back, "Timer-Client1");

    return continue_timer;
}


int main()
{
    MinMinBusGDBUS *proxy;

    GMainLoop *loop;

    loop = g_main_loop_new (NULL, FALSE);

    GError *error;
    gchar **buf;

    error = NULL;
    proxy = min_min_bus_gdbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE,
            "com.fatminmin", "/com/fatminmin/GDBUS", NULL, &error);

    g_timeout_add_seconds(2,(GSourceFunc) timer_cb,proxy); // every two seconds
    continue_timer = TRUE;

    g_main_loop_run (loop);

    g_object_unref(proxy);
    return 0;
}
