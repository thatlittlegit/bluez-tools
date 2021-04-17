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

#ifndef __BZT_DEVICE_H
#define __BZT_DEVICE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <glib-object.h>

#define BZT_DEVICE_DBUS_SERVICE "org.bluez"
#define BZT_DEVICE_DBUS_INTERFACE "org.bluez.Device1"


G_DECLARE_FINAL_TYPE(BztDevice, bzt_device, BZT, DEVICE, GObject)
struct dummy;

#define BZT_TYPE_DEVICE bzt_device_get_type()

/*
 * Constructor
 */
BztDevice *bzt_device_new(const gchar *dbus_object_path);

/*
 * Method definitions
 */
const gchar *bzt_device_get_dbus_object_path(BztDevice *self);

void bzt_device_cancel_pairing(BztDevice *self, GError **error);
void bzt_device_connect(BztDevice *self, GError **error);
void bzt_device_connect_profile(BztDevice *self, const gchar *uuid, GError **error);
void bzt_device_disconnect(BztDevice *self, GError **error);
void bzt_device_disconnect_profile(BztDevice *self, const gchar *uuid, GError **error);
void bzt_device_pair(BztDevice *self, GError **error);

GVariant *bzt_device_get_properties(BztDevice *self, GError **error);
void bzt_device_set_property(BztDevice *self, const gchar *name, const GVariant *value, GError **error);

const gchar *bzt_device_get_adapter(BztDevice *self, GError **error);
const gchar *bzt_device_get_address(BztDevice *self, GError **error);
const gchar *bzt_device_get_alias(BztDevice *self, GError **error);
void bzt_device_set_alias(BztDevice *self, const gchar *value, GError **error);
guint16 bzt_device_get_appearance(BztDevice *self, GError **error);
gboolean bzt_device_get_blocked(BztDevice *self, GError **error);
void bzt_device_set_blocked(BztDevice *self, const gboolean value, GError **error);
guint32 bzt_device_get_class(BztDevice *self, GError **error);
gboolean bzt_device_get_connected(BztDevice *self, GError **error);
const gchar *bzt_device_get_icon(BztDevice *self, GError **error);
gboolean bzt_device_get_legacy_pairing(BztDevice *self, GError **error);
const gchar *bzt_device_get_modalias(BztDevice *self, GError **error);
const gchar *bzt_device_get_name(BztDevice *self, GError **error);
gboolean bzt_device_get_paired(BztDevice *self, GError **error);
gint16 bzt_device_get_rssi(BztDevice *self, GError **error);
gboolean bzt_device_get_trusted(BztDevice *self, GError **error);
void bzt_device_set_trusted(BztDevice *self, const gboolean value, GError **error);
const gchar **bzt_device_get_uuids(BztDevice *self, GError **error);

#ifdef	__cplusplus
}
#endif

#endif /* __BZT_DEVICE_H */

