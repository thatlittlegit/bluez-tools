#include <gio/gio.h>
#include "bluez-api.h"
#include "dbus-common.h"
#include "manager.h"

struct _BztManager
{
    GObject parent_instance;

    GDBusProxy *proxy;
};

static void bzt_manager_initable_iface(GInitableIface* iface);
static gboolean bzt_manager_real_init(GInitable *initable, GCancellable *cancel, GError **error);

G_DEFINE_TYPE_WITH_CODE(BztManager, bzt_manager, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(G_TYPE_INITABLE, bzt_manager_initable_iface))
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
    // g_clear_object (&self->priv->an_object);
    g_clear_object (&self->proxy);


    /* Always chain up to the parent class; there is no need to check if
     * the parent class implements the dispose() virtual function: it is
     * always guaranteed to do so
     */
    G_OBJECT_CLASS (bzt_manager_parent_class)->dispose (gobject);
}

static void bzt_manager_finalize (GObject *gobject)
{
    BztManager *self = BZT_MANAGER(gobject);

    // g_free(self->priv->a_string);

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

static void bzt_manager_initable_iface(GInitableIface* iface)
{
    iface->init = bzt_manager_real_init;
}

static void bzt_manager_init(BztManager *self)
{
}

static gboolean bzt_manager_real_init(GInitable *initable, GCancellable *cancel, GError **error)
{
    g_assert(system_conn != NULL);
    BztManager *self = BZT_MANAGER(initable);

    self->proxy = g_dbus_proxy_new_sync(system_conn, G_DBUS_PROXY_FLAGS_NONE, NULL,
					BLUEZ_DBUS_SERVICE_NAME, MANAGER_DBUS_PATH,
					MANAGER_DBUS_INTERFACE, cancel, error);

    return self->proxy != NULL;
}

BztManager *bzt_manager_new(GCancellable *cancel, GError **error)
{
    return g_initable_new(BZT_TYPE_MANAGER, cancel, error, NULL);
}

GVariant *bzt_manager_get_managed_objects(BztManager *self, GError **error)
{
    g_assert(BZT_IS_MANAGER(self));

    GVariant *retVal = NULL;
    retVal = g_dbus_proxy_call_sync(self->proxy, "GetManagedObjects", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);

    if (retVal != NULL)
        retVal = g_variant_get_child_value(retVal, 0);

    return retVal;
}

const gchar *bzt_manager_find_adapter(BztManager *self, const gchar *pattern, GError **error)
{
    g_assert(BZT_IS_MANAGER(self));

    GVariant *objects = NULL;
    objects = bzt_manager_get_managed_objects(self, error);
    if (objects == NULL)
        return NULL;

    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;

    gchar *pattern_lowercase = NULL;
    if (pattern != NULL)
    {
        pattern_lowercase = g_ascii_strdown(pattern, -1);
    }

    g_variant_iter_init(&i, objects);
    gboolean still_looking = TRUE;
    while (still_looking && g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariantIter ii;
        GVariant* properties;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            gchar *interface_name_lowercase = g_ascii_strdown(interface_name, -1);
            if (strstr(interface_name_lowercase, "adapter"))
            {
                g_free(interface_name_lowercase);

                if (!pattern_lowercase)
                {
                    still_looking = FALSE;
                    break;
                }

                gchar *object_base_name_original = g_path_get_basename(object_path);
                gchar *object_base_name = g_ascii_strdown(interface_name, -1);
                g_free(object_base_name_original);

                if (strstr(object_base_name, pattern_lowercase))
                {
                    still_looking = FALSE;
                    g_free(object_base_name);
                    break;
                }

                g_free(object_base_name);

                const gchar *address_original = g_variant_get_string(g_variant_lookup_value(properties, "Address", NULL), NULL);
                gchar *address = g_ascii_strdown(address_original, -1);

                if (strstr(address, pattern_lowercase))
                {
                    still_looking = FALSE;
                    g_free(address);
                    break;
                }
                g_free(address);
            }
            else
            {
                g_free(interface_name_lowercase);
            }

            g_variant_unref(properties);
        }

        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);
    g_free(pattern_lowercase);

    if (still_looking)
    {
        return NULL;
    }
    else
    {
        return object_path;
    }
}

GPtrArray *bzt_manager_get_adapters(BztManager *self)
{
    g_assert(BZT_IS_MANAGER(self));

    GVariant *objects = NULL;
    GError *error = NULL;
    objects = bzt_manager_get_managed_objects(self, &error);
    if (objects == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
        return NULL;
    }

    GPtrArray *adapter_array = g_ptr_array_new();

    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;

    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            char* interface_name_lowercase = g_ascii_strdown(interface_name, -1);
            if (strstr(interface_name_lowercase, "adapter"))
                g_ptr_array_add(adapter_array, (gpointer) g_strdup(object_path));

            g_free(interface_name_lowercase);
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);

    return adapter_array;
}

const gchar **bzt_manager_get_devices(BztManager *self, const gchar *adapter_pattern)
{
    g_assert(BZT_IS_MANAGER(self));

    GVariant *objects = NULL;
    GError *error = NULL;
    objects = bzt_manager_get_managed_objects(self, &error);
    if (objects == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
        return NULL;
    }
    
    GRegex *adapter_regex = g_regex_new(adapter_pattern, 0, 0, &error);
    if (adapter_regex == NULL)
    {
        g_critical("%s", error->message);
        g_error_free(error);
    }
    
    GPtrArray *device_array = g_ptr_array_new();
    
    const gchar *object_path;
    GVariant *ifaces_and_properties;
    GVariantIter i;
    
    g_variant_iter_init(&i, objects);
    while (g_variant_iter_next(&i, "{&o@a{sa{sv}}}", &object_path, &ifaces_and_properties))
    {
        const gchar *interface_name;
        GVariant *properties;
        GVariantIter ii;
        g_variant_iter_init(&ii, ifaces_and_properties);
        while (g_variant_iter_next(&ii, "{&s@a{sv}}", &interface_name, &properties))
        {
            if (g_strcmp0(interface_name, "org.bluez.Device1") == 0)
            {
                const gchar *adapter_prop = g_variant_get_string(g_variant_lookup_value(properties, "Adapter", G_VARIANT_TYPE_OBJECT_PATH), NULL);
                if(g_regex_match(adapter_regex, adapter_prop, 0, NULL))
                    g_ptr_array_add(device_array, (gpointer) g_strdup(object_path));
            }
            g_variant_unref(properties);
        }
        g_variant_unref(ifaces_and_properties);
    }
    g_variant_unref(objects);

    g_regex_unref(adapter_regex);
    
    if(device_array->len > 0)
    {
        // Top it off with a NULL pointer
        g_ptr_array_add(device_array, (gpointer) NULL);
        return (const gchar**) g_ptr_array_free(device_array, FALSE);
    }
    else
        return NULL;
}
