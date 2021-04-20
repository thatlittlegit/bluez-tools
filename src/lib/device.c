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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gio/gio.h>
#include <glib.h>
#include <string.h>

#include "dbus-common.h"
#include "properties.h"

#include "device.h"

struct _BztDevice {
	GDBusProxy parent;

	Properties *properties;
};

static void _bzt_device_init_initable(GInitableIface *iface);

G_DEFINE_TYPE_WITH_CODE(BztDevice, bzt_device, G_TYPE_DBUS_PROXY,
	G_IMPLEMENT_INTERFACE(G_TYPE_INITABLE, _bzt_device_init_initable) struct dummy;);

enum {
	PROP_0,
};

static void _bzt_device_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void _bzt_device_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean _bzt_device_real_init(GInitable *initable, GCancellable *cancel, GError **error);

static void bzt_device_dispose(GObject *gobject)
{
	BztDevice *self = BZT_DEVICE(gobject);

	g_clear_object(&self->properties);

	G_OBJECT_CLASS(bzt_device_parent_class)->dispose(gobject);
}

static void bzt_device_finalize (GObject *gobject)
{
	BztDevice *self = BZT_DEVICE(gobject);
	G_OBJECT_CLASS(bzt_device_parent_class)->finalize(gobject);
}

static void bzt_device_class_init(BztDeviceClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = bzt_device_dispose;

	gobject_class->get_property = _bzt_device_get_property;
	gobject_class->set_property = _bzt_device_set_property;
}

static void _bzt_device_init_initable(GInitableIface *iface)
{
	iface->init = _bzt_device_real_init;
}

static void bzt_device_init(BztDevice *self)
{
}

static gboolean _bzt_device_real_init(GInitable *initable, GCancellable *cancel, GError **error)
{
	/* TODO currently, we just assert, but it might be a good idea to give
	 * out GErrors properly so the user doesn't see 'Assertion failed:
	 * (G_DBUS_PROXY(self))' or similar
	 *
	 * This isn't a problem now, but I plan to make changes that will make
	 * it a problem, best to get ready now.
	 */
	BztDevice *self = BZT_DEVICE(initable);

	self->properties = g_object_new(PROPERTIES_TYPE, "DBusType", "system", "DBusServiceName", BZT_DEVICE_DBUS_SERVICE, "DBusObjectPath", g_dbus_proxy_get_object_path(G_DBUS_PROXY(self)), NULL);
	g_assert(self->properties != NULL);
}

static void _bzt_device_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	BztDevice *self = BZT_DEVICE(object);

	switch (property_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void _bzt_device_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	BztDevice *self = BZT_DEVICE(object);

	switch (property_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

/* void CancelPairing() */
void bzt_device_cancel_pairing(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "CancelPairing", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void Connect() */
void bzt_device_connect(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "Connect", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void ConnectProfile(string uuid) */
void bzt_device_connect_profile(BztDevice *self, const gchar *uuid, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "ConnectProfile", g_variant_new ("(s)", uuid), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void Disconnect() */
void bzt_device_disconnect(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "Disconnect", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void DisconnectProfile(string uuid) */
void bzt_device_disconnect_profile(BztDevice *self, const gchar *uuid, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "DisconnectProfile", g_variant_new ("(s)", uuid), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void Pair() */
void bzt_device_pair(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "Pair", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* Properties access methods */
GVariant *bzt_device_get_properties(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	return properties_get_all(self->properties, BZT_DEVICE_DBUS_INTERFACE, error);
}

void bzt_device_set_property(BztDevice *self, const gchar *name, const GVariant *value, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_DEVICE_DBUS_INTERFACE, name, value, error);
}

const gchar *bzt_device_get_adapter(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Adapter", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_device_get_address(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Address", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_device_get_alias(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Alias", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

void bzt_device_set_alias(BztDevice *self, const gchar *value, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Alias", g_variant_new_string(value), error);
}

guint16 bzt_device_get_appearance(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Appearance", error);
	if(prop == NULL)
		return 0;
	guint16 ret = g_variant_get_uint16(prop);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_device_get_blocked(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Blocked", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_device_set_blocked(BztDevice *self, const gboolean value, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Blocked", g_variant_new_boolean(value), error);
}

guint32 bzt_device_get_class(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Class", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_device_get_connected(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Connected", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_device_get_icon(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Icon", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_device_get_legacy_pairing(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "LegacyPairing", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_device_get_modalias(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Modalias", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_device_get_name(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Name", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_device_get_paired(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Paired", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

gint16 bzt_device_get_rssi(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "RSSI", error);
	if(prop == NULL)
		return 0;
	gint16 ret = g_variant_get_int16(prop);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_device_get_trusted(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Trusted", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_device_set_trusted(BztDevice *self, const gboolean value, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_DEVICE_DBUS_INTERFACE, "Trusted", g_variant_new_boolean(value), error);
}

const gchar **bzt_device_get_uuids(BztDevice *self, GError **error)
{
	g_assert(BZT_IS_DEVICE(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_DEVICE_DBUS_INTERFACE, "UUIDs", error);
	if(prop == NULL)
		return NULL;
	const gchar **ret = g_variant_get_strv(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

