#include <tizen.h>
#include "boxex.h"

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

static Evas_Object*
create_box(Evas_Object *parent)
{
	Evas_Object *box = elm_box_add(parent);
	evas_object_show(box);
	return box;
}

static Evas_Object *
create_button(Evas_Object *parent, char *text)
{
	Evas_Object *button = elm_button_add(parent);
	elm_object_text_set(button, text);
	evas_object_show(button);
	return button;
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

    { /* child object - indent to how relationship */
        /* A box to put things in verticallly - default mode for box */
        Evas_Object *box = create_box(ad->win);
        elm_box_padding_set(box, 10, 10);
        evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_object_content_set(ad->conform, box);

        { /* child object - indent to how relationship */
            /* Create horizontal box */
            Evas_Object *horizontal_box = create_box(ad->win);
            elm_box_horizontal_set(horizontal_box, EINA_TRUE);
            elm_box_padding_set(horizontal_box, 10, 10);
            // 자기 자신의 크기를 지정
            evas_object_size_hint_weight_set(horizontal_box, EVAS_HINT_EXPAND, 0.0);
            evas_object_size_hint_align_set(horizontal_box, EVAS_HINT_FILL, 0.0);
            elm_box_pack_end(box, horizontal_box);

            { /* child object - indent to how relationship */
                Evas_Object *btn = create_button(horizontal_box, "Left");
                elm_box_pack_end(horizontal_box, btn);

                btn = create_button(horizontal_box, "Mid");
                evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, 0.0);
                evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
                elm_box_pack_end(horizontal_box, btn);

                btn = create_button(horizontal_box, "Right");
                elm_box_pack_end(horizontal_box, btn);
            }

            /* Create vertical box */
            Evas_Object *vertical_box = create_box(ad->win);
            elm_box_padding_set(vertical_box, 10, 10);
            // 자기 자신의 크기를 지정
            evas_object_size_hint_weight_set(vertical_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(vertical_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
            elm_box_pack_end(box, vertical_box);

            { /* child object - indent to how relationship */
            	Evas_Object *btn = create_button(vertical_box, "Top");
                evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, 0.0);
                evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
                elm_box_pack_end(vertical_box, btn);

                btn = create_button(vertical_box, "Center");
                evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
                elm_box_pack_end(vertical_box, btn);

                btn = create_button(vertical_box, "Bottom");
                evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, 0.0);
                evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
                elm_box_pack_end(vertical_box, btn);
            }
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
