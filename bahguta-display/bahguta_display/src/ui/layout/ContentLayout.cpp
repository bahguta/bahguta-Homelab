#include "ContentLayout.h"
#include "../themes/Theme.h"
#include "../../fonts/Fonts.h"

int ContentLayout::contentWidth(int bodyWidth)
{
    return bodyWidth - LEFT - RIGHT;
}

int ContentLayout::cardWidth(int bodyWidth)
{
    return (contentWidth(bodyWidth) - GAP) / 2;
}


int ContentLayout::cardWidth(int bodyWidth, int columns)
{
    return (contentWidth(bodyWidth) - GAP * (columns - 1)) / columns;
}

int ContentLayout::screenContentWidth(lv_obj_t* screen)
{
    lv_obj_update_layout(screen);
    return contentWidth(lv_obj_get_width(screen));
}

lv_obj_t* ContentLayout::createTitle(lv_obj_t* screen, const char* text)
{
    lv_obj_t* title = lv_label_create(screen);
    lv_label_set_text(title, text);
    lv_obj_set_style_text_font(title, &montserrat_28_es_28, 0);
    lv_obj_set_style_text_color(title, Theme::Text(), 0);
    lv_obj_set_pos(title, LEFT, TITLE_TOP);
    return title;
}
