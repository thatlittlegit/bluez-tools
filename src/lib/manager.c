#include <gio/gio.h>
#include "bluez-api.h"
#include "dbus-common.h"
#include "manager.h"

struct _BztManager
{
    GDBusObjectManagerClient parent_instance;
};

static GType _bzt_manager_get_proxy_type(GDBusObjectManagerClient *client, const char *object_path, const char *interface_name, void *data);

G_DEFINE_TYPE(BztManager, bzt_manager, G_TYPE_DBUS_OBJECT_MANAGER_CLIENT)
struct dummy;

static void bzt_manager_dispose (GObject *gobject)
{
    BztManager *self = BZT_MANAGER (gobject);

    /* In dispose(), you are supposed to free all types referenced from this
     * object which might themselves hold a reference to self. Generally,
     * the most simple solution is to unref all members on which you own a 
     * reference.
     */

    /* dispose() might be called multiple times, so we must guard against
     * calling g_object_unref() on an invalid GObject by setting the member
     * NULL; g_clear_object() does this for us, atomically.
     */

    /* Always chain up to the parent class; there is no need to check if
     * the parent class implements the dispose() virtual function: it is
     * always guaranteed to do so
     */
    G_OBJECT_CLASS (bzt_manager_parent_class)->dispose (gobject);
}

static void bzt_manager_finalize (GObject *gobject)
{
    BztManager *self = BZT_MANAGER(gobject);

    /* Always chain up to the parent class; as with dispose(), finalize()
     * is guaranteed to exist on the parent's class virtual function table
     */
    G_OBJECT_CLASS (bzt_manager_parent_class)->finalize (gobject);
}

static void bzt_manager_class_init(BztManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = bzt_manager_dispose;
    object_class->finalize = bzt_manager_finalize;
}

static void bzt_manager_init(BztManager *self)
{
}

BztManager *bzt_manager_new(GCancellable *cancel, GError **error)
{
    return g_initable_new(BZT_TYPE_MANAGER, cancel, error,
			  "connection", system_conn,
			  "flags", G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
			  "get-proxy-type-func", _bzt_manager_get_proxy_type,
			  "get-proxy-type-user-data", NULL,
			  "name", "org.bluez",
			  "object-path", BZT_MANAGER_DBUS_PATH,
			  NULL);
}

static GType _bzt_manager_get_proxy_type(GDBusObjectManagerClient *client, const char *object_path, const char *interface_name, void *userdata)
{
    if (interface_name == NULL)
        return G_TYPE_DBUS_OBJECT_PROXY;

    if (strcmp(interface_name, BZT_ADAPTER_DBUS_INTERFACE) == 0)
	return BZT_TYPE_ADAPTER;

    if (strcmp(interface_name, BZT_DEVICE_DBUS_INTERFACE) == 0)
	return BZT_TYPE_DEVICE;

    return G_TYPE_DBUS_PROXY;
}

BztAdapter *bzt_manager_find_adapter(BztManager *self, const gchar *pattern, GError **error)
{
    g_assert(BZT_IS_MANAGER(self));

    GList *objects = g_dbus_object_manager_get_objects(G_DBUS_OBJECT_MANAGER(self));
    if (objects == NULL)
        return NULL;

    gchar *pattern_lowercase = NULL;
    if (pattern != NULL)
    {
        pattern_lowercase = g_ascii_strdown(pattern, -1);
    }

    GList *current = objects;
    BztAdapter *found = NULL;
    do {
        BztAdapter *processing = BZT_ADAPTER(g_dbus_object_get_interface(G_DBUS_OBJECT(current->data), BZT_ADAPTER_DBUS_INTERFACE));
	if (processing == NULL)
	    continue;

        if (pattern_lowercase == NULL)
        {
            found = g_object_ref(processing);
            break;
        }

        const char *name = bzt_adapter_get_name(processing, error);
        char *lowercase_name = g_ascii_strdown(name, -1);

        if (strstr(lowercase_name, pattern_lowercase))
        {
            found = g_object_ref(processing);
            g_free(lowercase_name);
            break;
        }

        g_free(lowercase_name);

        const char *address = bzt_adapter_get_address(processing, error);
        char *lowercase_address = g_ascii_strdown(name, -1);

        if (strstr(lowercase_address, pattern_lowercase))
        {
            found = g_object_ref(processing);
            g_free(lowercase_address);
            break;
        }

        g_free(lowercase_address);
    } while ((current = current->next));

    g_free(pattern_lowercase);
    g_list_free_full(objects, g_object_unref);
    return found;
}

GList *bzt_manager_get_adapters(BztManager *self)
{
    g_assert(BZT_IS_MANAGER(self));

    GList *objects = g_dbus_object_manager_get_objects(G_DBUS_OBJECT_MANAGER(self));
    if (objects == NULL)
    {
        return NULL;
    }

    GList *result = NULL;
    GList *current = objects;
    do
    {
        BztAdapter *processing = BZT_ADAPTER(g_dbus_object_get_interface(G_DBUS_OBJECT(current->data), BZT_ADAPTER_DBUS_INTERFACE));
        if (processing == NULL)
            continue;

        result = g_list_prepend(result, processing);
    } while ((current = current->next));

    g_list_free_full(objects, g_object_unref);
    return result;
}

GList *bzt_manager_get_devices(BztManager *self, const gchar *adapter_pattern)
{
    g_assert(BZT_IS_MANAGER(self));

    GError *error = NULL;

    GList *children = g_dbus_object_manager_get_objects(G_DBUS_OBJECT_MANAGER(self));
    if (children == NULL)
    {
        return NULL;
    }
    
    GRegex *adapter_regex = g_regex_new(adapter_pattern, 0, 0, &error);
    if (adapter_regex == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
    }
    
    GList *devices = NULL;
    GList *current = children;
    do
    {
        BztDevice *processing = BZT_DEVICE(g_dbus_object_get_interface(G_DBUS_OBJECT(current->data), BZT_DEVICE_DBUS_INTERFACE));

        if (processing == NULL)
            continue;

        if (!g_regex_match(adapter_regex, bzt_device_get_adapter(processing, &error), 0, NULL))
        {
            g_assert(error == NULL);
            continue;
        }

        devices = g_list_prepend(devices, g_object_ref(processing));
    } while ((current = current->next));

    g_regex_unref(adapter_regex);
    g_list_free_full(children, g_object_unref);

    return devices;
}
