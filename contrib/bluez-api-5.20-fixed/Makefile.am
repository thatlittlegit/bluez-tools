AM_CPPFLAGS = $(GLIB_CFLAGS) $(GIO_CFLAGS)
LDADD = $(GLIB_LIBS) $(GIO_LIBS)

noinst_LIBRARIES = libbluez-tools-api.a

nodist_libbluez_tools_api_a_SOURCES = \
	adapter.c adapter.h \
	agent_manager.c agent_manager.h \
	alert.c alert.h \
	alert_agent.c alert_agent.h \
	cycling_speed.c cycling_speed.h \
	cycling_speed_manager.c cycling_speed_manager.h \
	device.c device.h \
	health_channel.c health_channel.h \
	health_device.c health_device.h \
	health_manager.c health_manager.h \
	heart_rate.c heart_rate.h \
	heart_rate_manager.c heart_rate_manager.h \
	media.c media.h \
	media_control.c media_control.h \
	media_player.c media_player.h \
	network.c network.h \
	network_server.c network_server.h \
	obex_agent_manager.c obex_agent_manager.h \
	obex_client.c obex_client.h \
	obex_file_transfer.c obex_file_transfer.h \
	obex_message.c obex_message.h \
	obex_object_push.c obex_object_push.h \
	obex_phonebook_access.c obex_phonebook_access.h \
	obex_session.c obex_session.h \
	obex_synchronization.c obex_synchronization.h \
	obex_transfer.c obex_transfer.h \
	profile_manager.c profile_manager.h \
	proximity_monitor.c proximity_monitor.h \
	proximity_reporter.c proximity_reporter.h \
	sim_access.c sim_access.h \
	thermometer.c thermometer.h \
	thermometer_manager.c thermometer_manager.h

BUILT_SOURCES = $(nodist_libbluez_tools_api_a_SOURCES)

$(nodist_libbluez_tools_api_a_SOURCES): bindings.stamp
	mv "../out/$@" "$@"
	touch "$@"

libbluez_tools_api_a_text = adapter-api.txt \
	agent-api.txt \
	alert-api.txt \
	cyclingspeed-api.txt \
	device-api.txt \
	health-api.txt \
	heartrate-api.txt \
	media-api.txt \
	network-api.txt \
	obex-agent-api.txt \
	obex-api.txt \
	profile-api.txt \
	proximity-api.txt \
	sap-api.txt \
	thermometer-api.txt

bindings.stamp: $(libbluez_tools_api_a_text)
	mkdir -p ../out/obex
	cd .. && ./generate-bluez-gobjects.sh
	touch "$@"

clean-local:
	-rm -f $(BUILT_SOURCES)
	-rm bindings.stamp
	-rm -rv ../out
