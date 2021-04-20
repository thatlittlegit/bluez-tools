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

#include "adapter.h"

struct _BztAdapter {
	GDBusProxy parent;

	Properties *properties;
};

static void _bzt_adapter_implement_initable(GInitableIface* iface);

G_DEFINE_TYPE_WITH_CODE(BztAdapter, bzt_adapter, G_TYPE_DBUS_PROXY,
			G_IMPLEMENT_INTERFACE(G_TYPE_INITABLE, _bzt_adapter_implement_initable)) struct dummy;

enum {
	PROP_0,
};

static void _bzt_adapter_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void _bzt_adapter_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static gboolean _bzt_adapter_real_init(GInitable *object, GCancellable* cancel, GError** error);

static void _bzt_adapter_create_gdbus_proxy(BztAdapter *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error);

static void bzt_adapter_dispose(GObject *gobject)
{
	BztAdapter *self = BZT_ADAPTER(gobject);

	g_clear_object(&self->properties);

	G_OBJECT_CLASS(bzt_adapter_parent_class)->dispose(gobject);
}

static void bzt_adapter_finalize (GObject *gobject)
{
	BztAdapter *self = BZT_ADAPTER(gobject);
	G_OBJECT_CLASS(bzt_adapter_parent_class)->finalize(gobject);
}

static void bzt_adapter_class_init(BztAdapterClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = bzt_adapter_dispose;
	gobject_class->get_property = _bzt_adapter_get_property;
	gobject_class->set_property = _bzt_adapter_set_property;
}

static void _bzt_adapter_implement_initable(GInitableIface* iface)
{
	iface->init = _bzt_adapter_real_init;
}

static void bzt_adapter_init(BztAdapter *self)
{
}

static gboolean _bzt_adapter_real_init(GInitable *initable, GCancellable *cancel, GError **error)
{
	/* TODO currently, we just assert, but it might be a good idea to give
	 * out GErrors properly so the user doesn't see 'Assertion failed:
	 * (G_DBUS_PROXY(self))' or similar
	 *
	 * This isn't a problem now, but I plan to make changes that will make
	 * it a problem, best to get ready now.
	 */
	BztAdapter *self = BZT_ADAPTER(initable);

	self->properties = g_object_new(PROPERTIES_TYPE, "DBusType", "system", "DBusServiceName", BZT_ADAPTER_DBUS_SERVICE, "DBusObjectPath", g_dbus_proxy_get_object_path(G_DBUS_PROXY(self)), NULL);
	g_assert(self->properties != NULL);
}

static void _bzt_adapter_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	BztAdapter *self = BZT_ADAPTER(object);

	switch (property_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void _bzt_adapter_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	BztAdapter *self = BZT_ADAPTER(object);

	switch (property_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

/* void RemoveDevice(object device) */
void bzt_adapter_remove_device(BztAdapter *self, const gchar *device, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "RemoveDevice", g_variant_new ("(o)", device), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void StartDiscovery() */
void bzt_adapter_start_discovery(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "StartDiscovery", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void StopDiscovery() */
void bzt_adapter_stop_discovery(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_dbus_proxy_call_sync(G_DBUS_PROXY(self), "StopDiscovery", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* Properties access methods */
GVariant *bzt_adapter_get_properties(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	return properties_get_all(self->properties, BZT_ADAPTER_DBUS_INTERFACE, error);
}

void bzt_adapter_set_property(BztAdapter *self, const gchar *name, const GVariant *value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, name, value, error);
}

const gchar *bzt_adapter_get_address(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Address", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_adapter_get_alias(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Alias", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

void bzt_adapter_set_alias(BztAdapter *self, const gchar *value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Alias", g_variant_new_string(value), error);
}

guint32 bzt_adapter_get_class(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Class", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_adapter_get_discoverable(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Discoverable", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_adapter_set_discoverable(BztAdapter *self, const gboolean value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Discoverable", g_variant_new_boolean(value), error);
}

guint32 bzt_adapter_get_discoverable_timeout(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "DiscoverableTimeout", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_adapter_set_discoverable_timeout(BztAdapter *self, const guint32 value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "DiscoverableTimeout", g_variant_new_uint32(value), error);
}

gboolean bzt_adapter_get_discovering(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Discovering", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_adapter_get_modalias(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Modalias", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

const gchar *bzt_adapter_get_name(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Name", error);
	if(prop == NULL)
		return NULL;
	const gchar *ret = g_variant_get_string(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

gboolean bzt_adapter_get_pairable(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Pairable", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_adapter_set_pairable(BztAdapter *self, const gboolean value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Pairable", g_variant_new_boolean(value), error);
}

guint32 bzt_adapter_get_pairable_timeout(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "PairableTimeout", error);
	if(prop == NULL)
		return 0;
	guint32 ret = g_variant_get_uint32(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_adapter_set_pairable_timeout(BztAdapter *self, const guint32 value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "PairableTimeout", g_variant_new_uint32(value), error);
}

gboolean bzt_adapter_get_powered(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Powered", error);
	if(prop == NULL)
		return FALSE;
	gboolean ret = g_variant_get_boolean(prop);
	g_variant_unref(prop);
	return ret;
}

void bzt_adapter_set_powered(BztAdapter *self, const gboolean value, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	properties_set(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "Powered", g_variant_new_boolean(value), error);
}

const gchar **bzt_adapter_get_uuids(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->properties != NULL);
	GVariant *prop = properties_get(self->properties, BZT_ADAPTER_DBUS_INTERFACE, "UUIDs", error);
	if(prop == NULL)
		return NULL;
	const gchar **ret = g_variant_get_strv(prop, NULL);
	g_variant_unref(prop);
	return ret;
}

