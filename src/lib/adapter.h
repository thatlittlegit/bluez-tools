/*
 *
 *  bluez-tools - a set of tools to manage bluetooth devices for linux
 *
 *  Copyright (C) 2010  Alexander Orlenko <zxteam@gmail.com>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __BZT_ADAPTER_H
#define __BZT_ADAPTER_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define BZT_ADAPTER_DBUS_SERVICE "org.bluez"
#define BZT_ADAPTER_DBUS_INTERFACE "org.bluez.Adapter1"

G_DECLARE_FINAL_TYPE(BztAdapter, bzt_adapter, BZT, ADAPTER, GDBusProxy)
struct dummy;

#define BZT_TYPE_ADAPTER bzt_adapter_get_type()

void bzt_adapter_remove_device(BztAdapter *self, const gchar *device, GError **error);
void bzt_adapter_start_discovery(BztAdapter *self, GError **error);
void bzt_adapter_stop_discovery(BztAdapter *self, GError **error);

GVariant *bzt_adapter_get_properties(BztAdapter *self, GError **error);
void bzt_adapter_set_property(BztAdapter *self, const gchar *name, const GVariant *value, GError **error);

const gchar *bzt_adapter_get_address(BztAdapter *self, GError **error);
const gchar *bzt_adapter_get_alias(BztAdapter *self, GError **error);
void bzt_adapter_set_alias(BztAdapter *self, const gchar *value, GError **error);
guint32 bzt_adapter_get_class(BztAdapter *self, GError **error);
gboolean bzt_adapter_get_discoverable(BztAdapter *self, GError **error);
void bzt_adapter_set_discoverable(BztAdapter *self, const gboolean value, GError **error);
guint32 bzt_adapter_get_discoverable_timeout(BztAdapter *self, GError **error);
void bzt_adapter_set_discoverable_timeout(BztAdapter *self, const guint32 value, GError **error);
gboolean bzt_adapter_get_discovering(BztAdapter *self, GError **error);
const gchar *bzt_adapter_get_modalias(BztAdapter *self, GError **error);
const gchar *bzt_adapter_get_name(BztAdapter *self, GError **error);
gboolean bzt_adapter_get_pairable(BztAdapter *self, GError **error);
void bzt_adapter_set_pairable(BztAdapter *self, const gboolean value, GError **error);
guint32 bzt_adapter_get_pairable_timeout(BztAdapter *self, GError **error);
void bzt_adapter_set_pairable_timeout(BztAdapter *self, const guint32 value, GError **error);
gboolean bzt_adapter_get_powered(BztAdapter *self, GError **error);
void bzt_adapter_set_powered(BztAdapter *self, const gboolean value, GError **error);
const gchar **bzt_adapter_get_uuids(BztAdapter *self, GError **error);

G_END_DECLS

#endif /* __BZT_ADAPTER_H */

