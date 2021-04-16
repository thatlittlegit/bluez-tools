#ifndef MANAGER_H
#define	MANAGER_H

#include <glib-object.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(BztManager, bzt_manager, BZT, MANAGER, GObject)
struct dummy;

#define BZT_TYPE_MANAGER bzt_manager_get_type()

#define MANAGER_DBUS_PATH       "/"
#define MANAGER_DBUS_INTERFACE  "org.freedesktop.DBus.ObjectManager"

BztManager *bzt_manager_new();
    
GVariant *bzt_manager_get_managed_objects(BztManager *self, GError **error);
const gchar *bzt_manager_find_adapter(BztManager *self, const gchar *pattern, GError **error);
GPtrArray *bzt_manager_get_adapters(BztManager *self);
const gchar **bzt_manager_get_devices(BztManager *self, const gchar *adapter_pattern);

G_END_DECLS
#endif	/* MANAGER_H */

