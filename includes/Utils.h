#pragma once

// #define EDITOR_DEBUG 

#ifdef EDITOR_DEBUG
    #define DRAW_DEBUG_RECTANGLE(rect, color) \
        DrawRectangleRec(rect, color)
#else
    #define DRAW_DEBUG_RECTANGLE(rect, color) \
        ((void)0)
#endif