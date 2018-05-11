#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "minminbus.h"
#include "vinay245bus.h"

/* Determines if to continue the timer or not */
static gboolean continue_timer = FALSE;

/* Determines if the timer has started */
static gboolean start_timer = FALSE;

static gboolean
on_handle_hello_world (MinMinBusGDBUS *interface, GDBusMethodInvocation *invocation,
					const gchar *greeting, gpointer user_data)
{
	gchar *response;
	response = g_strdup_printf ("Hello world %s!!.", greeting);
	min_min_bus_gdbus_complete_hello_world (interface, invocation, response);
	g_print("%s\n", response);
	g_free (response);
	return TRUE;
}

static void
on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	MinMinBusGDBUS *interface;
	GError *error;

	interface = min_min_bus_gdbus_skeleton_new();
	g_signal_connect (interface, "handle-hello-world", G_CALLBACK (on_handle_hello_world), NULL);
	error = NULL;
	!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (interface), connection, "/com/fatminmin/GDBUS", &error);
}

static void AsyncCall_back(vinay245BusGDBUS *proxy, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL; // important 
    gchar **buf1;
    gboolean result;
    
    result = vinay245_bus_gdbus_call_hello_client_finish(proxy, NULL, res, &error);

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
    vinay245BusGDBUS *proxy = (vinay245BusGDBUS *) user_data;
    vinay245_bus_gdbus_call_hello_client(proxy, "Vinay245-Timer-Async", NULL, (GAsyncReadyCallback) AsyncCall_back, "Timer- Vinay245");

    return continue_timer;
}

int main()
{
	GMainLoop *loop;

	loop = g_main_loop_new (NULL, FALSE);

	g_bus_own_name(G_BUS_TYPE_SESSION, "com.fatminmin", G_BUS_NAME_OWNER_FLAGS_NONE, NULL,
				on_name_acquired, NULL, NULL, NULL);

    // get a proxy for the bus
    vinay245BusGDBUS *proxy;
    GError *error;
    error = NULL;
    gchar **buf;

    proxy = vinay245_bus_gdbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, G_DBUS_PROXY_FLAGS_NONE,
            "com.vinay245", "/com/vinay245/GDBUS", NULL, &error);

    vinay245_bus_gdbus_call_hello_client_sync(proxy, "sync - vinay245Test", buf, NULL, &error);

   // g_print("resp: %s\n", *buf);

    g_timeout_add_seconds(1,(GSourceFunc) timer_cb,proxy);

    continue_timer = TRUE;
    start_timer = TRUE;

	g_main_loop_run (loop);

	return 0;
}
