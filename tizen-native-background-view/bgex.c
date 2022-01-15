#include <tizen.h>
#include "bgex.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
} appdata_s;

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_iconified_set(ad->win, EINA_TRUE);
}

static void
app_get_resource(const char *res_file_in, char *res_path_out, int res_path_max)
{
    char *res_path = app_get_resource_path();
    if (res_path) {
        snprintf(res_path_out, res_path_max, "%s%s", res_path, res_file_in);
        free(res_path);
    }
}

static void
my_table_pack(Evas_Object *table, Evas_Object *child, int x, int y, int w, int h)
{
   evas_object_size_hint_align_set(child, EVAS_HINT_FILL, EVAS_HINT_FILL);
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

    /* Label*/
    /*ad->label = elm_label_add(ad->conform);
    elm_object_text_set(ad->label, "Hello EFL");
    evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(ad->conform, ad->label);
    evas_object_show(ad->label);*/

    /* Table */
    Evas_Object *table = elm_table_add(ad->win);
    /* Make table homogenous - every cell will be the same size */
    elm_table_homogeneous_set(table, EINA_TRUE);
    /* Let the table child allocation area expand within in the box */
    evas_object_size_hint_weight_set(table, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, table);
    evas_object_show(table);

    {
        /* Bg-1 Color */
        Evas_Object *bg = elm_bg_add(ad->win);
        my_table_pack(table, bg, 0, 0, 2, 2);
        elm_bg_color_set(bg, 66, 162, 206);
        evas_object_show(bg);

        /* Image path */
        char buf[PATH_MAX];
        app_get_resource("images/logo.png", buf, (int)PATH_MAX);

        /* Bg-2 Image Center */
        bg = elm_bg_add(ad->win);
        elm_bg_option_set(bg, ELM_BG_OPTION_CENTER);
        elm_bg_file_set(bg, buf, NULL);
        my_table_pack(table, bg, 2, 0, 2, 2);

        /* Bg-3 Image Scale */
        bg = elm_bg_add(ad->win);
        elm_bg_option_set(bg, ELM_BG_OPTION_SCALE);
        elm_bg_file_set(bg, buf, NULL);
        my_table_pack(table, bg, 0, 2, 2, 2);

        /* Bg-4 Image Stretch */
        bg = elm_bg_add(ad->win);
        elm_bg_option_set(bg, ELM_BG_OPTION_STRETCH);
        elm_bg_file_set(bg, buf, NULL);
        my_table_pack(table, bg, 2, 2, 2, 2);

        /* Bg-5 Image Tile */
        bg = elm_bg_add(ad->win);
        elm_bg_option_set(bg, ELM_BG_OPTION_TILE);
        elm_bg_file_set(bg, buf, NULL);
        my_table_pack(table, bg, 1, 1, 2, 2);
    }

    /* Show window after base gui is set up */
    evas_object_show(ad->win);
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
