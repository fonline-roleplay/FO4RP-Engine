#include "StdAfx.h"
#include "ImGuiHelpers.h"
#include "MsgStr.h"

static ImU32 ImGuiFOnlineColor( unsigned int color )
{
    return IM_COL32( ( color >> 16 ) & 0xFF, ( color >> 8 ) & 0xFF, color & 0xFF, ( color >> 24 ) & 0xFF );
}

void ImGuiDrawFOnlineText( const char* text, unsigned int default_color, float wrap_width )
{
    if( !text || !text[ 0 ] )
        return;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImFont* font = ImGui::GetFont();
    float font_size = ImGui::GetFontSize();
    float line_height = ImGui::GetTextLineHeight();
    ImVec2 start = ImGui::GetCursorScreenPos();
    ImVec2 pos = start;
    float max_x = start.x + ( wrap_width > 0.0f ? wrap_width : ImGui::GetContentRegionAvail().x );
    float widest = 0.0f;
    uint color = default_color;
    const char* cursor = text;

    while( *cursor )
    {
        if( *cursor == '|' )
        {
            char* color_end = NULL;
            uint parsed_color = (uint) strtoul( cursor + 1, &color_end, 0 );
            if( color_end && color_end > cursor + 1 && *color_end == ' ' )
            {
                color = parsed_color & 0xFF000000 ? parsed_color : ( color & 0xFF000000 ) | parsed_color;
                cursor = color_end + 1;
                continue;
            }
        }

        if( *cursor == '\n' )
        {
            widest = MAX( widest, pos.x - start.x );
            pos.x = start.x;
            pos.y += line_height;
            cursor++;
            continue;
        }

        const char* word_begin = cursor;
        while( *cursor && *cursor != '|' && *cursor != '\n' && *cursor != ' ' && *cursor != '\t' && (uchar) *cursor != TEXT_SYMBOL_DOT )
            cursor++;

        const char* word_end = cursor;
        const char* replacement = NULL;
        if( (uchar) *cursor == TEXT_SYMBOL_DOT )
        {
            replacement = TEXT_SYMBOL_DOT_UTF8;
            word_begin = replacement;
            word_end = replacement + strlen( replacement );
            cursor++;
        }
        else if( *cursor == ' ' || *cursor == '\t' )
        {
            cursor++;
            word_end = cursor;
        }
        else if( word_begin == word_end )
        {
            // A malformed marker is ordinary text.
            cursor++;
            word_end = cursor;
        }

        ImVec2 word_size = font->CalcTextSizeA( font_size, FLT_MAX, 0.0f, word_begin, word_end );
        if( pos.x > start.x && pos.x + word_size.x > max_x )
        {
            widest = MAX( widest, pos.x - start.x );
            pos.x = start.x;
            pos.y += line_height;
            while( word_begin < word_end && ( *word_begin == ' ' || *word_begin == '\t' ) )
                word_begin++;
            word_size = font->CalcTextSizeA( font_size, FLT_MAX, 0.0f, word_begin, word_end );
        }

        if( word_begin < word_end )
            draw_list->AddText( font, font_size, pos, ImGuiFOnlineColor( color ), word_begin, word_end );
        pos.x += word_size.x;
    }

    widest = MAX( widest, pos.x - start.x );
    bool ends_with_newline = cursor > text && cursor[ -1 ] == '\n';
    float height = pos.y - start.y + ( ends_with_newline ? 0.0f : line_height );
    ImGui::Dummy( ImVec2( widest, MAX( height, line_height ) ) );
}
