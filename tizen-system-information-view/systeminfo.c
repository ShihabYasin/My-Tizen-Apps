#include "systeminfo.h"
#include <system_info.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
    Evas_Object *label1;
    Evas_Object *label2;
    Evas_Object *label3;
    Evas_Object *label4;
    Evas_Object *label5;
    Evas_Object *label6;
} appdata_s;

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_iconified_set(ad->win, EINA_TRUE);
}

static void
btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    appdata_s *ad = data;
    char buf[100];
    char *sValue = NULL;
    bool bValue = false;
    int nValue = 0;
    int ret;

    ret = system_info_get_platform_bool("http://tizen.org/feature/camera.back", &bValue);
    if (ret == SYSTEM_INFO_ERROR_NONE)
    {
        elm_object_text_set(ad->label1, bValue ? "True" : "False");
    }

    ret = system_info_get_platform_bool("http://tizen.org/feature/camera.front", &bValue);
    if (ret == SYSTEM_INFO_ERROR_NONE)
    {
        elm_object_text_set(ad->label2, bValue ? "True" : "False");
    }

    ret = system_info_get_platform_bool("http://tizen.org/feature/network.telephony", &bValue);
    if (ret == SYSTEM_INFO_ERROR_NONE)
    {
        elm_object_text_set(ad->label3, bValue ? "True" : "False");
    }

    ret = system_info_get_platform_int("tizen.org/feature/screen.width", &nValue);
    if (ret == SYSTEM_INFO_ERROR_NONE)
    {
        sprintf(buf, "%d px", nValue);
        elm_object_text_set(ad->label4, buf);
    }

    ret = system_info_get_platform_int("tizen.org/feature/screen.height", &nValue);
    if (ret == SYSTEM_INFO_ERROR_NONE)
    {
        sprintf(buf, "%d px", nValue);
        elm_object_text_set(ad->label5, buf);
    }

    ret = system_info_get_platform_string("http://tizen.org/feature/platform.version", &sValue);
    if (ret == SYSTEM_INFO_ERROR_NONE)
    {
        elm_object_text_set(ad->label6, sValue);
        free(sValue);
    }
}

static void
my_table_pack(Evas_Object *table, Evas_Object *child, int col, int row, int spanx, int spany,
		      bool h_expand, bool v_expand, double h_align, double v_align)
{
    /* Create a frame around the child, for padding */
    Evas_Object *frame = elm_frame_add(table);
    elm_object_style_set(frame, "pad_small");

    evas_object_size_hint_weight_set(frame, h_expand ? EVAS_HINT_EXPAND : 0, v_expand ? EVAS_HINT_EXPAND : 0);
    evas_object_size_hint_align_set(frame, h_align, v_align);

    /* place child in its box */
    {
        evas_object_size_hint_weight_set(child, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_size_hint_align_set(child, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_object_content_set(frame, child);
        evas_object_show(child);
    }

    elm_table_pack(table, frame, col, row, spanx, spany);
    evas_object_show(frame);
}

static Evas_Object *
my_button_add(Evas_Object *parent, const char *text, Evas_Smart_Cb cb, void *cb_data)
{
    Evas_Object *btn;

    btn = elm_button_add(parent);
    elm_object_text_set(btn, text);
    evas_object_smart_callback_add(btn, "clicked", cb, cb_data);

    return btn;
}

static Evas_Object *
my_label_add(Evas_Object *parent, const char *text)
{
    Evas_Object *btn;

    btn = elm_label_add(parent);
    elm_object_text_set(btn, text);

    return btn;
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
    ad->conform = elm_conformant_add(ad->win);
    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);

    {
        Evas_Object *tbl, *btn, *frame, *o;

        /* Frame */
        frame = elm_frame_add(ad->win);
        elm_object_style_set(frame, "pad_medium");
        elm_object_content_set(ad->conform, frame);
        evas_object_show(frame);

        /* Container: standard table */
        tbl = elm_table_add(ad->win);
        /* Make this table homogeneous for nicer, fixed layout */
        elm_table_homogeneous_set(tbl, EINA_TRUE);
        elm_object_content_set(frame, tbl);
        evas_object_show(tbl);

        {
            /* Button */
            btn = my_button_add(tbl, "Load System Info", btn_clicked_cb, ad);
            my_table_pack(tbl, btn, 0, 0, 2, 1, EVAS_HINT_EXPAND, 0, EVAS_HINT_FILL, EVAS_HINT_FILL);

            o = my_label_add(tbl, "Back Camera:");
            my_table_pack(tbl, o, 0, 1, 1, 1, EVAS_HINT_EXPAND, 0, 1.0, EVAS_HINT_FILL);

            ad->label1 = my_label_add(tbl, "");
            my_table_pack(tbl, ad->label1, 1, 1, 1, 1, EVAS_HINT_EXPAND, 0, 0.0, EVAS_HINT_FILL);

            o = my_label_add(tbl, "Front Camera:");
            my_table_pack(tbl, o, 0, 2, 1, 1, EVAS_HINT_EXPAND, 0, 1.0, EVAS_HINT_FILL);

            ad->label2 = my_label_add(tbl, "");
            my_table_pack(tbl, ad->label2, 1, 2, 1, 1, EVAS_HINT_EXPAND, 0, 0.0, EVAS_HINT_FILL);

            o = my_label_add(tbl, "Telephony:");
            my_table_pack(tbl, o, 0, 3, 1, 1, EVAS_HINT_EXPAND, 0, 1.0, EVAS_HINT_FILL);

            ad->label3 = my_label_add(tbl, "");
            my_table_pack(tbl, ad->label3, 1, 3, 1, 1, EVAS_HINT_EXPAND, 0, 0.0, EVAS_HINT_FILL);

            o = my_label_add(tbl, "Screen Width:");
            my_table_pack(tbl, o, 0, 4, 1, 1, EVAS_HINT_EXPAND, 0, 1.0, EVAS_HINT_FILL);

            ad->label4 = my_label_add(tbl, "");
            my_table_pack(tbl, ad->label4, 1, 4, 1, 1, EVAS_HINT_EXPAND, 0, 0.0, EVAS_HINT_FILL);

            o = my_label_add(tbl, "Screen Height:");
            my_table_pack(tbl, o, 0, 5, 1, 1, EVAS_HINT_EXPAND, 0, 1.0, EVAS_HINT_FILL);

            ad->label5 = my_label_add(tbl, "");
            my_table_pack(tbl, ad->label5, 1, 5, 1, 1, EVAS_HINT_EXPAND, 0, 0.0, EVAS_HINT_FILL);

            o = my_label_add(tbl, "Platform Version:");
            my_table_pack(tbl, o, 0, 6, 1, 1, EVAS_HINT_EXPAND, 0, 1.0, EVAS_HINT_FILL);

            ad->label6 = my_label_add(tbl, "");
            my_table_pack(tbl, ad->label6, 1, 6, 1, 1, EVAS_HINT_EXPAND, 0, 0.0, EVAS_HINT_FILL);
        }
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
