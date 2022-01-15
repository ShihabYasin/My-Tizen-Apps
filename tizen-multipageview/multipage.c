#include "multipage.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
    Evas_Object *nf;
	Evas_Object *label;
} appdata_s;

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_iconified_set(ad->win, EINA_TRUE);
}

static Eina_Bool
naviframe_pop_cb(void *data, Elm_Object_Item *it)
{
    ui_app_exit();
    return EINA_FALSE;
}

static void
my_box_pack(Evas_Object *box, Evas_Object *child,
            double h_weight, double v_weight, double h_align, double v_align)
{
   /* create a frame we shall use as padding around the child widget */
   Evas_Object *frame = elm_frame_add(box);
   /* use the medium padding style. there is "pad_small", "pad_medium",
    * "pad_large" and "pad_huge" available as styles in addition to the
    * "default" frame style */
   elm_object_style_set(frame, "pad_medium");
   /* set the input weight/aling on the frame insted of the child */
   evas_object_size_hint_weight_set(frame, h_weight, v_weight);
   evas_object_size_hint_align_set(frame, h_align, v_align);
     {
        /* tell the child that is packed into the frame to be able to expand */
        evas_object_size_hint_weight_set(child, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        /* fill the expanded area (above) as opposaed to center in it */
        evas_object_size_hint_align_set(child, EVAS_HINT_FILL, EVAS_HINT_FILL);
        /* actually put the child in the frame and show it */
        evas_object_show(child);
        elm_object_content_set(frame, child);
     }
   /* put the frame into the box instead of the child directly */
   elm_box_pack_end(box, frame);
   /* show the frame */
   evas_object_show(frame);
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

    //eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

    /* Conformant */
    ad->conform = elm_conformant_add(ad->win);
    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);

    {
        /* Naviframe */
        ad->nf = elm_naviframe_add(ad->conform);
        elm_object_part_content_set(ad->conform, "elm.swallow.content", ad->nf);
        elm_object_content_set(ad->conform, ad->nf);

        Evas_Object *box = elm_box_add(ad->nf);
        elm_box_padding_set(box, 10 * elm_config_scale_get(), 10 * elm_config_scale_get());
        evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        elm_object_content_set(ad->nf, box);
        evas_object_show(box);

        {
            /* Label*/
            ad->label = elm_label_add(ad->conform);
            elm_object_text_set(ad->label, "Press Button");
            my_box_pack(box, ad->label, 1.0, 0.0, -1.0, 0.0);

            /* Button */
            Evas_Object *btn = elm_button_add(ad->conform);
            elm_object_text_set(btn, "Sub Window");
            evas_object_smart_callback_add(btn, "clicked", sub_view_cb, ad->nf);
            my_box_pack(box, btn, 1.0, 0.0, -1.0, 0.0);

            /* Header */
            Elm_Object_Item *nf_it;
            nf_it = elm_naviframe_item_push(ad->nf, "Main Window", NULL, NULL, box, NULL);
            eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
            elm_naviframe_item_pop_cb_set(nf_it, naviframe_pop_cb, ad->win);
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
