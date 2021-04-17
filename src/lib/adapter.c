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
	GObject parent;
	GDBusProxy *proxy;
	Properties *properties;
	gchar *object_path;
};

G_DEFINE_TYPE(BztAdapter, bzt_adapter, G_TYPE_OBJECT);

enum {
	PROP_0,
	PROP_DBUS_OBJECT_PATH /* readwrite, construct only */
};

static void _bzt_adapter_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void _bzt_adapter_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);

static void _bzt_adapter_create_gdbus_proxy(BztAdapter *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error);

static void bzt_adapter_dispose(GObject *gobject)
{
	BztAdapter *self = BZT_ADAPTER(gobject);

	/* Proxy free */
	g_clear_object (&self->proxy);
	/* Properties free */
	g_clear_object(&self->properties);
	/* Object path free */
	g_free(self->object_path);
	/* Chain up to the parent class */
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

	/* Properties registration */
	GParamSpec *pspec = NULL;

	gobject_class->get_property = _bzt_adapter_get_property;
	gobject_class->set_property = _bzt_adapter_set_property;
	
	/* object DBusObjectPath [readwrite, construct only] */
	pspec = g_param_spec_string("DBusObjectPath", "dbus_object_path", "BztAdapter D-Bus object path", NULL, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property(gobject_class, PROP_DBUS_OBJECT_PATH, pspec);
	if (pspec)
		g_param_spec_unref(pspec);
}

static void bzt_adapter_init(BztAdapter *self)
{
	/* filler */
	/* filler */
	g_assert(system_conn != NULL);
}

static void _bzt_adapter_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	BztAdapter *self = BZT_ADAPTER(object);

	switch (property_id) {
	case PROP_DBUS_OBJECT_PATH:
		g_value_set_string(value, bzt_adapter_get_dbus_object_path(self));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}
}

static void _bzt_adapter_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	BztAdapter *self = BZT_ADAPTER(object);
	GError *error = NULL;

	switch (property_id) {
	case PROP_DBUS_OBJECT_PATH:
		self->object_path = g_value_dup_string(value);
		_bzt_adapter_create_gdbus_proxy(self, BZT_ADAPTER_DBUS_SERVICE, self->object_path, &error);
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
		break;
	}

	if (error != NULL)
		g_critical("%s", error->message);

	g_assert(error == NULL);
}

/* Constructor */
BztAdapter *bzt_adapter_new(const gchar *dbus_object_path)
{
	return g_object_new(BZT_TYPE_ADAPTER, "DBusObjectPath", dbus_object_path, NULL);
}

/* Private DBus proxy creation */
static void _bzt_adapter_create_gdbus_proxy(BztAdapter *self, const gchar *dbus_service_name, const gchar *dbus_object_path, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	self->proxy = g_dbus_proxy_new_sync(system_conn, G_DBUS_PROXY_FLAGS_NONE, NULL, dbus_service_name, dbus_object_path, BZT_ADAPTER_DBUS_INTERFACE, NULL, error);

	if(self->proxy == NULL)
		return;

	self->properties = g_object_new(PROPERTIES_TYPE, "DBusType", "system", "DBusServiceName", dbus_service_name, "DBusObjectPath", dbus_object_path, NULL);
	g_assert(self->properties != NULL);
}

/* Methods */

/* Get DBus object path */
const gchar *bzt_adapter_get_dbus_object_path(BztAdapter *self)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_assert(self->proxy != NULL);
	return g_dbus_proxy_get_object_path(self->proxy);
}

/* void RemoveDevice(object device) */
void bzt_adapter_remove_device(BztAdapter *self, const gchar *device, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_dbus_proxy_call_sync(self->proxy, "RemoveDevice", g_variant_new ("(o)", device), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void StartDiscovery() */
void bzt_adapter_start_discovery(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_dbus_proxy_call_sync(self->proxy, "StartDiscovery", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
}

/* void StopDiscovery() */
void bzt_adapter_stop_discovery(BztAdapter *self, GError **error)
{
	g_assert(BZT_IS_ADAPTER(self));
	g_dbus_proxy_call_sync(self->proxy, "StopDiscovery", NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, error);
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

