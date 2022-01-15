#include "calendarex.h"
#include <utils_i18n.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
    Evas_Object *label_day[6][7];
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
    i18n_ustring_copy_ua(_tzid, tzid);
    int len = i18n_ustring_get_length(_tzid);
    int ret = i18n_ucalendar_create(_tzid, len, "en_US", I18N_UCALENDAR_TRADITIONAL, &ucal);
    return ucal;
}

static char*
time2string(i18n_ucalendar_h ucal)
{
    int year, month, day, hour, minute, second;
    i18n_ucalendar_get(ucal, I18N_UCALENDAR_YEAR, &year);
    i18n_ucalendar_get(ucal, I18N_UCALENDAR_MONTH, &month);
    i18n_ucalendar_get(ucal, I18N_UCALENDAR_DATE, &day);
    i18n_ucalendar_get(ucal, I18N_UCALENDAR_HOUR, &hour);
    i18n_ucalendar_get(ucal, I18N_UCALENDAR_MINUTE, &minute);
    i18n_ucalendar_get(ucal, I18N_UCALENDAR_SECOND, &second);

    char *buf = malloc(100);
    sprintf(buf, "Now :%04d-%02d-%02d %02d:%02d:%02d", year, month + 1, day, hour, minute, second);
    return buf;
}

static void
draw_calendar(appdata_s *ad)
{
    int date, month, dow, days, is_leap;
    int max_day[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_DATE, &date);
    i18n_ucalendar_set(ad->ucal, I18N_UCALENDAR_DATE, 1);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_MONTH, &month);
    i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_DAY_OF_WEEK, &dow);
    days = max_day[month];

    if( month == 1 )
    {
        i18n_ucalendar_get(ad->ucal, I18N_UCALENDAR_IS_LEAP_MONTH, &is_leap);
        if( is_leap == 1 )
            days = 29;
    }

    int i=0, j=0;
    char buf[10];

    i = dow - 1;
    for(int d=1; d <= days; d++)
    {
        sprintf(buf, "%d", d);
        if( d == date )
            sprintf(buf, "[%d]", d);
        elm_object_text_set(ad->label_day[j][i], buf);

        i ++;
        if( i >= 7 )
        {
            i = 0;
            j ++;
        }
    }
}

static void
my_table_pack(Evas_Object *table, Evas_Object *child, int x, int y, int w, int h)
{
   evas_object_size_hint_align_set(child, 0.5, 0.5);
   evas_object_size_hint_weight_set(child, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_table_pack(table, child, x, y, w, h);
   evas_object_show(child);
}

static void
create_base_gui(appdata_s *ad)
{
	/* set up policy to exit when last window is closed */
	elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
	/* Window */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	int rots[4] = { 0, 90, 180, 270 };
	elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);

	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

    /* Conformant */
    /*ad->conform = elm_conformant_add(ad->win);
    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);*/

    /* Box to put the table in so we can bottom-align the table
     * window will stretch all resize object content to win size */
    Evas_Object *box = elm_box_add(ad->win);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, box);
    evas_object_show(box);

    /* Table */
    Evas_Object *table = elm_table_add(ad->win);
    /* Make table homogenous - every cell will be the same size */
    elm_table_homogeneous_set(table, EINA_TRUE);
    /* Set padding of 10 pixels multiplied by scale factor of UI */
    elm_table_padding_set(table, 10 * elm_config_scale_get(), 10 * elm_config_scale_get());
    /* Let the table child allocation area expand within in the box */
    evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    /* Set table to fiill width but align to bottom of box */
    evas_object_size_hint_align_set(table, EVAS_HINT_FILL, 0.0);
    elm_box_pack_end(box, table);
    evas_object_show(table);

    {
        /* Label*/
        ad->label = elm_label_add(ad->win);
        elm_object_text_set(ad->label, "<align=center>Hello EFL</align>");
        my_table_pack(table, ad->label, 0, 0, 7, 1);

        for(int j=0; j < 6; j++)
        {
            for(int i=0; i < 7; i++)
            {
                ad->label_day[j][i] = elm_label_add(ad->win);
                elm_object_text_set(ad->label_day[j][i], ".");
                my_table_pack(table, ad->label_day[j][i], i, j + 2, 1, 1);
            }
        }
    }

    /* Show window after base gui is set up */
    evas_object_show(ad->win);

    system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, &ad->tzid);
    ad->ucal = create_time(ad->tzid);
    char *buf = time2string(ad->ucal);
    elm_object_text_set(ad->label, buf);

    draw_calendar(ad);
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
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
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
