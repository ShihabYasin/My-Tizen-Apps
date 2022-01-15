#include "multipage.h"

void
btn_back_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *nf = data;
    elm_naviframe_item_pop(nf);
}

static Evas_Object*
create_button_view(Evas_Object *parent, Evas_Object *nf)
{
    Evas_Object *btn, *img, *box;

    box = elm_box_add(parent);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);

    /* icon_reorder style */
    btn = elm_button_add(box);
    elm_object_style_set(btn, "icon_reorder");
    evas_object_smart_callback_add(btn, "clicked", btn_back_cb, nf);
    evas_object_show(btn);
    elm_box_pack_end(box, btn);

    return box;
}

void
sub_view_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *scroller, *layout;
    Evas_Object *nf = data;

    scroller = elm_scroller_add(nf);
    layout = create_button_view(scroller, nf);
    //layout = create_button_view(scroller);
    elm_object_content_set(scroller, layout);

    elm_naviframe_item_push(nf, "Sub Window", NULL, NULL, scroller, NULL);
}
