AM_CPPFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS) -I../contrib
LDADD = $(GLIB_LIBS) $(GIO_LIBS) ../contrib/bluez-api-5.20-fixed/libbluez-tools-api.a

lib_sources = 	lib/adapter.c lib/adapter.h \
		lib/agent-helper.c lib/agent-helper.h \
		lib/dbus-common.c lib/dbus-common.h \
		lib/device.c lib/device.h \
		lib/helpers.c lib/helpers.h \
		lib/manager.c lib/manager.h \
		lib/obex_agent.c lib/obex_agent.h \
		lib/properties.c lib/properties.h \
		lib/sdp.c lib/sdp.h \
		lib/bluez-api.h

bin_PROGRAMS = bt-adapter bt-agent bt-device bt-network bt-obex
bt_adapter_SOURCES = $(lib_sources) $(bluez_sources) bt-adapter.c
bt_agent_SOURCES = $(lib_sources) $(bluez_sources) bt-agent.c
bt_device_SOURCES = $(lib_sources) $(bluez_sources) bt-device.c
bt_network_SOURCES = $(lib_sources) $(bluez_sources) bt-network.c
bt_obex_SOURCES = $(lib_sources) $(bluez_sources) bt-obex.c
bt_obex_LDADD = $(LDADD) $(LIBREADLINE)

dist_man_MANS = bt-adapter.1 bt-agent.1 bt-device.1 bt-network.1 bt-obex.1
