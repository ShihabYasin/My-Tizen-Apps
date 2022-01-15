#include "datetime.h"
#include <utils_i18n.h>

typedef struct appdata {
    Evas_Object *win;
    Evas_Object *conform;
    Evas_Object *label1;
    Evas_Object *label2;
    Evas_Object *label3;
    Evas_Object *label4;
    Evas_Object *label5;
    Evas_Object *slide;
    Ecore_Timer *timer;
    char *tzid;
    i18n_ucalendar_h ucal;
} appdata_s;

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_iconified_set(ad->win, EINA_TRUE);
}

static i18n_ucalendar_h
create_time(char *tzid)
{
    i18n_ucalendar_h ucal;
    i18n_uchar *_tzid = (i18n_uchar*)calloc(strlen(tzid) + 1, sizeof(i18n_uchar));
    // converts 'tzid' to unicode string
    i18n_ustring_copy_ua(_tzid, tzid);
    // gets length of '_tzid'
    int len = i18n_ustring_get_length(_tzid);
    // creates i18n_ucalendar_h
    int ret = i18n_ucalendar_create(_tzid, len, "en_US", I18N_UCALENDAR_TRADITIONAL, &ucal);
    if (ret != 0)
    {
    	dlog_print(DLOG_ERROR, LOG_TAG, "i18n_ucalendar_create() failed with err = %d", ret);
    	return NULL;
    }

    return ucal;
}

static void
update(appdata_s *ad)
{
    int year, month, day, hour, minute, second;
    i18n_udate udate;
    char buf[256];
    int diff;

    /* Current time */
    i18n_ucalendar_get_now(&udate);
    i18n_ucalendar_set_milliseconds(ad->ucal, udate);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_YEAR, &year);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_MONTH, &month);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_DATE, &day);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_HOUR_OF_DAY, &hour);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_MINUTE, &minute);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_SECOND, &second);
    snprintf(buf, sizeof(buf), "%d/%02d/%02d %02d:%02d:%02d", year, month + 1, day, hour, minute, second);
    elm_object_text_set(ad->label2, buf);

    /* POSIX time since EPOCH */
    snprintf(buf, sizeof(buf), "%llums", (unsigned long long) udate);
    elm_object_text_set(ad->label3, buf);

    /* 40 days later (label) */
    diff = (int) elm_slider_value_get(ad->slide);
    if (diff >= 0)
        snprintf(buf, sizeof(buf), "%d day%s later:", diff, (diff > 1) ? "s" : "");
    else if (diff < 0)
    	snprintf(buf, sizeof(buf), "%d day%s earlier:", -diff, (diff < -1) ? "s" : "");
    elm_object_text_set(ad->label4, buf);

    /* 40 days later (value) */
    i18n_ucalendar_add(ad->ucal, I18N_UCALENDAR_DATE, diff);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_YEAR, &year);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_MONTH, &month);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_DATE, &day);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_HOUR_OF_DAY, &hour);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_MINUTE, &minute);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_SECOND, &second);
    snprintf(buf, sizeof(buf), "%d/%02d/%02d %02d:%02d:%02d", year, month + 1, day, hour, minute, second);
    elm_object_text_set(ad->label5, buf);
}

static void
table_pack(Evas_Object *tbl, Evas_Object *sub,
		int col, int row, int colspan, int rowspan,
		double h_weight, double v_weight, double h_align, double v_align)
{
    evas_object_size_hint_weight_set(sub, h_weight, v_weight);
    evas_object_size_hint_align_set(sub, h_align, v_align);
    elm_table_pack(tbl, sub, col, row, colspan, rowspan);
    evas_object_show(sub);
}

static Eina_Bool
timer_cb(void *data)
{
    appdata_s *ad = data;
    update(ad);
    return ECORE_CALLBACK_RENEW;
}

static void
spinner_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	update(ad);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	const int rots[4] = { 0, 90, 180, 270 };
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);
	elm_win_wm_rotation_available_rotations_set(ad->win, rots, 4);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

    /* Conformant */
    ad->conform = elm_conformant_add(ad->win);
    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);

    /* Table */
    Evas_Object *table = elm_table_add(ad->win);
    elm_table_padding_set(table, 5 * elm_scale_get(), 5 * elm_scale_get());
    elm_object_content_set(ad->conform, table);
    evas_object_show(table);

    {
        Evas_Object *o;

        /* Timezone label */
        o = elm_label_add(table);
        elm_object_text_set(o, "Time Zone:");
        table_pack(table, o, 0, 0, 1, 1, 0.5, 1.0, 1.0, 1.0);

        ad->label1 = elm_label_add(table);
        table_pack(table, ad->label1, 1, 0, 1, 1, 0.5, 1.0, 0.0, 1.0);

        system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, &ad->tzid);
        elm_object_text_set(ad->label1, ad->tzid);

        /* Current time label */
        o = elm_label_add(table);
        elm_object_text_set(o, "Current Time:");
        table_pack(table, o, 0, 1, 1, 1, 0.5, 0.0, 1.0, 0.5);

        ad->label2 = elm_label_add(table);
        table_pack(table, ad->label2, 1, 1, 1, 1, 0.5, 0.0, 0.0, 0.5);

        /* Current time label */
        o = elm_label_add(table);
        elm_object_text_set(o, "Since Epoch:");
        table_pack(table, o, 0, 2, 1, 1, 0.5, 0.0, 1.0, 0.5);

        ad->label3 = elm_label_add(table);
        table_pack(table, ad->label3, 1, 2, 1, 1, 0.5, 0.0, 0.0, 0.5);

        /* Showcase datetime computation */
        ad->label4 = elm_label_add(table);
        elm_object_text_set(ad->label4, "40 days later:");
        table_pack(table, ad->label4, 0, 3, 1, 1, 0.5, 0.0, 1.0, 0.5);

        ad->label5 = elm_label_add(table);
        table_pack(table, ad->label5, 1, 3, 1, 1, 0.5, 0.0, 0.0, 0.5);

        /* Spinner for more time difference */
        ad->slide = elm_slider_add(table);
        elm_slider_min_max_set(ad->slide, -365, 365);
        elm_slider_value_set(ad->slide, 40);
        table_pack(table, ad->slide, 0, 4, 2, 1, 1.0, 2.0, -1.0, 0.0);
        evas_object_smart_callback_add(ad->slide, "changed", spinner_cb, ad);
    }

    /* Show window after base gui is set up */
    evas_object_show(ad->win);

    /* Prepare calendar object and 1-second timer */
    ad->ucal = create_time(ad->tzid);
    ad->timer = ecore_timer_add(1.0, timer_cb, ad);
    update(ad);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
