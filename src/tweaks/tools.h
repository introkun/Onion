#ifndef TWEAKS_TOOLS_H__
#define TWEAKS_TOOLS_H__

#include <stdio.h>
#include <sys/wait.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "utils/msleep.h"
#include "components/list.h"
#include "theme/theme.h"
#include "./appstate.h"

int _runCommand(const char *cmd)
{
    int ret = system(cmd);
    if (WEXITSTATUS(ret) == 0x10)
        return 0;
    else
        return 1;
}

void _blackScreenWithText(const char *text_str)
{
    int popup_width = 640, popup_height = 100;
    SDL_Rect popup_rect = {320 - popup_width / 2, 240 - popup_height / 2, popup_width, popup_height};
    SDL_FillRect(screen, &popup_rect, 0);
    SDL_Surface *text = TTF_RenderUTF8_Blended(resource_getFont(TITLE), text_str, (SDL_Color){255, 255, 255});
    if (text) {
        SDL_Rect text_rect = {320 - text->w / 2, 240 - text->h / 2};
        SDL_BlitSurface(text, NULL, screen, &text_rect);
        SDL_FreeSurface(text);
    }
    SDL_BlitSurface(screen, NULL, video, NULL);
    SDL_Flip(video);
}

void _runCommandPopup(const char *cmd)
{
    keys_enabled = false;
    _blackScreenWithText("Applying tool...");
    int ec = _runCommand(cmd);
    _blackScreenWithText(ec == 0 ? "Done" : "Tool failed");
    msleep(300);
    keys_enabled = true;
    all_changed = true;
}

void tool_favoritesSortAlpha(void *pt)
{
    _runCommandPopup("./bin/tools favsort");
}

void tool_favoritesSortSystem(void *pt)
{
    _runCommandPopup("./bin/tools favsort2");
}

void tool_favoritesFixThumbnails(void *pt)
{
    _runCommandPopup("./bin/tools boxart");
}

void tool_recentsRemoveApps(void *pt)
{
    _runCommandPopup("./bin/tools recents --clean_all");
}

void tool_removeDotUnderscoreFiles(void *pt)
{
    _runCommandPopup("find /mnt/SDCARD -depth -name \"._*\" -name \".DS_Store\" -exec rm {} \\;");
}

#endif // TWEAKS_TOOLS_H__
