#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "minminbus.h"
#include "vinay245bus.h"

/*guint
g_timeout_add (guint interval,
               GSourceFunc function,
               gpointer data);*/


/* Determines if to continue the timer or not */
static gboolean continue_timer = FALSE;

/* Determines if the timer has started */
static gboolean start_timer = FALSE;

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
    min_min_bus_gdbus_call_hello_world(proxy, "Timer", NULL, (GAsyncReadyCallback) AsyncCall_back, "Timer");

    return continue_timer;
}


// Client Interfaces
static gboolean
on_handle_hello_world (vinay245BusGDBUS *interface, GDBusMethodInvocation *invocation,
                    const gchar *greeting, gpointer user_data)
{
    gchar *response;
    response = g_strdup_printf ("Client ==== Vinay245 Hello world %s!!.", greeting);
    vinay245_bus_gdbus_complete_hello_client (interface, invocation, response);
    g_print("%s\n", response);
    g_free (response);
    return TRUE;
}

static void
on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    vinay245BusGDBUS *interface;
    GError *error;

    interface = vinay245_bus_gdbus_skeleton_new();
    g_print(" Client ==== Acquired Bus on Vinay245 ");
    g_signal_connect (interface, "handle-hello-client", G_CALLBACK (on_handle_hello_world), NULL);
    error = NULL;
    !g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (interface), connection, "/com/vinay245/GDBUS", &error);
}

int main()
{
    MinMinBusGDBUS *proxy;
    //

    //

    GMainLoop *loop;

    loop = g_main_loop_new (NULL, FALSE);

    GError *error;
    gchar **buf;

    error = NULL;
    proxy = min_min_bus_gdbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE,
            "com.fatminmin", "/com/fatminmin/GDBUS", NULL, &error);    

    min_min_bus_gdbus_call_hello_world_sync(proxy, "fatminmin1", buf, NULL, &error);

  // g_print("resp: %s\n", *buf);
    
    min_min_bus_gdbus_call_hello_world_sync(proxy,"test1",buf, NULL,&error);

   // g_print("resp: %s\n", *buf);

    min_min_bus_gdbus_call_hello_world(proxy, "AsyncCall1", NULL, (GAsyncReadyCallback) AsyncCall_back, "call1");
    min_min_bus_gdbus_call_hello_world(proxy, "AsyncCall2", NULL, (GAsyncReadyCallback) AsyncCall_back, "call2");
    min_min_bus_gdbus_call_hello_world(proxy, "AsyncCall3", NULL, (GAsyncReadyCallback) AsyncCall_back, "call3");
    min_min_bus_gdbus_call_hello_world(proxy, "AsyncCall4", NULL, (GAsyncReadyCallback) AsyncCall_back, "call4");

    g_timeout_add_seconds(1,(GSourceFunc) timer_cb,proxy);
    continue_timer = TRUE;
    start_timer = TRUE;


    // Acquire new bus

    g_bus_own_name(G_BUS_TYPE_SESSION, "com.vinay245", G_BUS_NAME_OWNER_FLAGS_NONE, NULL,
                on_name_acquired, NULL, NULL, NULL);

    g_main_loop_run (loop);

    g_object_unref(proxy);
    return 0;
}
