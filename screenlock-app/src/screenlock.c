#include <tizen.h>

#include <device/display.h>
#include <device/callback.h>

#include "screenlock.h"

static bool app_create(void *data) {
	int rv;
	dlog_print(DLOG_VERBOSE, LOG_TAG, "app_create(): Entry");

	rv = device_display_change_state(DISPLAY_STATE_SCREEN_OFF);
	if (rv < 0)
		dlog_print(DLOG_ERROR, LOG_TAG,
				"Failed to set DISPLAY_STATE_SCREEN_OFF: [%d]", rv);

	app_exit();
	return false; /* Terminate Application */
}

static void app_terminate(void *data) {
	dlog_print(DLOG_VERBOSE, LOG_TAG, "app_terminate(): Entry");
}

int main(int argc, char *argv[]) {
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = { 0, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = NULL;
	event_callback.resume = NULL;
	event_callback.app_control = NULL;

	ret = ui_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
