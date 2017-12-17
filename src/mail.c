#include "global.h"
#include "mail.h"
#include "constants/items.h"
#include "main.h"
#include "overworld.h"
#include "task.h"
#include "unknown_task.h"
#include "palette.h"
#include "text.h"
#include "menu.h"
#include "menu_helpers.h"
#include "text_window.h"
#include "string_util.h"
#include "international_string_util.h"
#include "strings.h"
#include "gpu_regs.h"
#include "bg.h"
#include "pokemon_icon.h"
#include "constants/species.h"
#include "malloc.h"
#include "easy_chat.h"

extern const u16 gUnknown_08DBE818[];
extern const u16 gUnknown_08DBE838[];
extern const u16 gUnknown_08DBE858[];
extern const u16 gUnknown_08DBE878[];
extern const u16 gUnknown_08DBE898[];
extern const u16 gUnknown_08DBE8B8[];
extern const u16 gUnknown_08DBE8D8[];
extern const u16 gUnknown_08DBE8F8[];
extern const u16 gUnknown_08DBE918[];
extern const u16 gUnknown_08DBE938[];
extern const u16 gUnknown_08DBE958[];
extern const u16 gUnknown_08DBE978[];
extern const u8 gUnknown_08DBE998[];
extern const u8 gUnknown_08DBFBA4[];
extern const u8 gUnknown_08DBEB38[];
extern const u8 gUnknown_08DBFC7C[];
extern const u8 gUnknown_08DBEC74[];
extern const u8 gUnknown_08DBFD5C[];
extern const u8 gUnknown_08DBEE84[];
extern const u8 gUnknown_08DBFE68[];
extern const u8 gUnknown_08DBEF5C[];
extern const u8 gUnknown_08DBFF44[];
extern const u8 gUnknown_08DBF154[];
extern const u8 gUnknown_08DC0034[];
extern const u8 gUnknown_08DBF2D4[];
extern const u8 gUnknown_08DC0114[];
extern const u8 gUnknown_08DBF37C[];
extern const u8 gUnknown_08DC01F4[];
extern const u8 gUnknown_08DBF50C[];
extern const u8 gUnknown_08DC0300[];
extern const u8 gUnknown_08DBF64C[];
extern const u8 gUnknown_08DC03F0[];
extern const u8 gUnknown_08DBF7B4[];
extern const u8 gUnknown_08DC04E8[];
extern const u8 gUnknown_08DBF904[];
extern const u8 gUnknown_08DC0600[];

// Static type declarations

struct UnkMailStruct
{
    u32 numEasyChatWords:2;
    u32 xOffset:6;
    u32 lineHeight:8;
};

struct MailLayout
{
    u8 numSubStructs;
    u8 signatureYPos;
    u8 signatureWidth;
    u8 wordsXPos;
    u8 wordsYPos;
    const struct UnkMailStruct *var8;
};

struct MailGraphics
{
    const u16 *palette;
    const u8 *tiles;
    const u8 *tileMap;
    u16 var0C;
    u16 var0E;
    u16 color10;
    u16 color12;
};

struct MailRead
{
    /*0x0000*/ u8 strbuf[8][64];
    /*0x0200*/ u8 playerName[12];
    /*0x020C*/ MainCallback callback;
    /*0x0210*/ MainCallback callback2;
    /*0x0214*/ struct MailStruct *mail;
    /*0x0218*/ bool8 flag;
    /*0x0219*/ u8 signatureWidth;
    /*0x021a*/ u8 mailType;
    /*0x021b*/ u8 animsActive;
    /*0x021c*/ u8 monIconSprite;
    /*0x021d*/ u8 language;
    /*0x021e*/ bool8 playerIsSender;
    /*0x0220*/ void (*parserSingle)(u8 *dest, u16 word);
    /*0x0224*/ void (*parserMultiple)(u8 *dest, const u16 *src, u16 length1, u16 length2);
    /*0x0228*/ const struct MailLayout *layout;
    /*0x022c*/ u8 bg1TilemapBuffer[0x1000];
    /*0x122c*/ u8 bg2TilemapBuffer[0x1000];
};

// Static RAM declarations

static EWRAM_DATA struct MailRead *sMailRead = NULL;

// Static ROM declarations

static void CB2_InitMailRead(void);
static void sub_8121A1C(void);
static void sub_8121B1C(void);
static void VBlankCB_MailRead(void);
static void CB2_MailRead(void);
static void CB2_WaitForPaletteExitOnKeyPress(void);
static void CB2_ExitOnKeyPress(void);
static void CB2_ExitMailReadFreeVars(void);

// .rodata

static const struct BgTemplate sUnknown_0859F290[] = {
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .priority = 0
    }, {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .priority = 1
    }, {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .priority = 2
    }
};

static const struct WindowTemplate sUnknown_0859F29C[] = {
    {
        .priority = 0,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 26,
        .height = 15,
        .paletteNum = 15,
        .baseBlock = 1
    },
    DUMMY_WIN_TEMPLATE
};

static const u8 sUnknown_0859F2AC[] = {
     0,
    10,
    11
};

static const u16 sUnknown_0859F2B0[][2] = {
    { 0x6ACD, 0x51A5 },
    { 0x45FC, 0x38D4 }
};

static const struct MailGraphics sUnknown_0859F2B8[] = {
    {
        gUnknown_08DBE818, gUnknown_08DBE998, gUnknown_08DBFBA4, 0x02c0, 0x0000, 0x294a, 0x6739
    }, {
        gUnknown_08DBE838, gUnknown_08DBEB38, gUnknown_08DBFC7C, 0x02e0, 0x0000, 0x7fff, 0x4631
    }, {
        gUnknown_08DBE858, gUnknown_08DBEC74, gUnknown_08DBFD5C, 0x0400, 0x0000, 0x294a, 0x6739
    }, {
        gUnknown_08DBE878, gUnknown_08DBEE84, gUnknown_08DBFE68, 0x01e0, 0x0000, 0x7fff, 0x4631
    }, {
        gUnknown_08DBE898, gUnknown_08DBEF5C, gUnknown_08DBFF44, 0x02e0, 0x0000, 0x7fff, 0x4631
    }, {
        gUnknown_08DBE8B8, gUnknown_08DBF154, gUnknown_08DC0034, 0x0300, 0x0000, 0x294a, 0x6739
    }, {
        gUnknown_08DBE8D8, gUnknown_08DBF2D4, gUnknown_08DC0114, 0x0140, 0x0000, 0x7fff, 0x4631
    }, {
        gUnknown_08DBE8F8, gUnknown_08DBF37C, gUnknown_08DC01F4, 0x0300, 0x0000, 0x7fff, 0x4631
    }, {
        gUnknown_08DBE918, gUnknown_08DBF50C, gUnknown_08DC0300, 0x0220, 0x0000, 0x294a, 0x6739
    }, {
        gUnknown_08DBE938, gUnknown_08DBF64C, gUnknown_08DC03F0, 0x0340, 0x0000, 0x294a, 0x6739
    }, {
        gUnknown_08DBE958, gUnknown_08DBF7B4, gUnknown_08DC04E8, 0x02a0, 0x0000, 0x294a, 0x6739
    }, {
        gUnknown_08DBE978, gUnknown_08DBF904, gUnknown_08DC0600, 0x0520, 0x0000, 0x294a, 0x6739
    }
};

static const struct UnkMailStruct Unknown_0859F3A8[] = {
    { .numEasyChatWords = 3, .lineHeight = 16 },
    { .numEasyChatWords = 3, .lineHeight = 16 },
    { .numEasyChatWords = 3, .lineHeight = 16 }
};

static const struct MailLayout sUnknown_0859F3B4[] = {
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x08, 0x00, 0x02, 0x04, Unknown_0859F3A8 },
    { 0x03, 0x00, 0x00, 0x02, 0x00, Unknown_0859F3A8 }
};

static const struct UnkMailStruct Unknown_0859F444[] = {
    { .numEasyChatWords = 2, .lineHeight = 16 },
    { .numEasyChatWords = 2, .lineHeight = 16 },
    { .numEasyChatWords = 2, .lineHeight = 16 },
    { .numEasyChatWords = 2, .lineHeight = 16 },
    { .numEasyChatWords = 1, .lineHeight = 16 }
};

static const struct MailLayout sUnknown_0859F458[] = {
    { 0x05, 0x07, 0x58, 0x0b, 0x1e, Unknown_0859F444 },
    { 0x05, 0x0a, 0x60, 0x09, 0x1e, Unknown_0859F444 },
    { 0x05, 0x0c, 0x68, 0x05, 0x1e, Unknown_0859F444 },
    { 0x05, 0x05, 0x60, 0x08, 0x1e, Unknown_0859F444 },
    { 0x05, 0x0a, 0x60, 0x09, 0x1e, Unknown_0859F444 },
    { 0x05, 0x09, 0x70, 0x05, 0x1e, Unknown_0859F444 },
    { 0x05, 0x0c, 0x68, 0x09, 0x1e, Unknown_0859F444 },
    { 0x05, 0x0d, 0x68, 0x0d, 0x1e, Unknown_0859F444 },
    { 0x05, 0x09, 0x60, 0x09, 0x1e, Unknown_0859F444 },
    { 0x05, 0x09, 0x60, 0x09, 0x1e, Unknown_0859F444 },
    { 0x05, 0x11, 0x68, 0x0f, 0x1e, Unknown_0859F444 },
    { 0x05, 0x09, 0x60, 0x05, 0x1e, Unknown_0859F444 }
};

// What the heck are these meant to be? Call them u16 for now.

static const u16 Unknown_0859F4E8[] = {
    0x00, 0x4000, 0x00, 0x00
};

static const u16 Unknown_0859F4F0[] = {
    0x00, 0x00, -1, 0x00
};

static const u16 Unknown_0859F4F8[] = {
    0x04, 0x00, -1, 0x00
};

static const u16 Unknown_0859F500[] = {
    0x00, 0x40, -1, 0x00
};

static const u16 *const sUnknown_0859F508[] = {
    Unknown_0859F4F0,
    Unknown_0859F4F8,
    Unknown_0859F500
};

// .text

void ReadMail(struct MailStruct *mail, void (*callback)(void), bool8 flag)
{
    u16 buffer[2];
    u16 species;

    sMailRead = calloc(1, sizeof(*sMailRead));
    sMailRead->language = LANGUAGE_ENGLISH;
    sMailRead->playerIsSender = TRUE;
    sMailRead->parserSingle = CopyEasyChatWord;
    sMailRead->parserMultiple = ConvertEasyChatWordsToString;
    if (IS_ITEM_MAIL(mail->itemId))
    {
        sMailRead->mailType = mail->itemId - ITEM_ORANGE_MAIL;
    }
    else
    {
        sMailRead->mailType = 0;
        flag = FALSE;
    }
    switch (sMailRead->playerIsSender)
    {
        case FALSE:
        default:
            sMailRead->layout = &sUnknown_0859F3B4[sMailRead->mailType];
            break;
        case TRUE:
            sMailRead->layout = &sUnknown_0859F458[sMailRead->mailType];
            break;
    }
    species = MailSpeciesToSpecies(mail->species, buffer);
    if (species >= SPECIES_BULBASAUR && species < NUM_SPECIES)
    {
        switch (sMailRead->mailType)
        {
            default:
                sMailRead->animsActive = 0;
                break;
            case ITEM_BEAD_MAIL - ITEM_ORANGE_MAIL:
                sMailRead->animsActive = 1;
                break;
            case ITEM_DREAM_MAIL - ITEM_ORANGE_MAIL:
                sMailRead->animsActive = 2;
                break;
        }
    }
    else
    {
        sMailRead->animsActive = 0;
    }
    sMailRead->mail = mail;
    sMailRead->callback = callback;
    sMailRead->flag = flag;
    SetMainCallback2(CB2_InitMailRead);
}

static bool8 MailReadBuildGraphics(void)
{
    u16 icon;

    switch (gMain.state)
    {
        case 0:
            SetVBlankCallback(NULL);
            remove_some_task();
            SetGpuReg(REG_OFFSET_DISPCNT, 0x0000);
            break;
        case 1:
            CpuFill16(0, (void *)OAM, OAM_SIZE);
            break;
        case 2:
            ResetPaletteFade();
            break;
        case 3:
            ResetTasks();
            break;
        case 4:
            ResetSpriteData();
            break;
        case 5:
            FreeAllSpritePalettes();
            reset_temp_tile_data_buffers();
            SetGpuReg(REG_OFFSET_BG0HOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG0VOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG1HOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG1VOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG2VOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG2HOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG3HOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BG3VOFS, 0x0000);
            SetGpuReg(REG_OFFSET_BLDCNT,  0x0000);
            SetGpuReg(REG_OFFSET_BLDALPHA, 0x0000);
            break;
        case 6:
            ResetBgsAndClearDma3BusyFlags(0);
            InitBgsFromTemplates(0, sUnknown_0859F290, 3);
            SetBgTilemapBuffer(1, sMailRead->bg1TilemapBuffer);
            SetBgTilemapBuffer(2, sMailRead->bg2TilemapBuffer);
            break;
        case 7:
            InitWindows(sUnknown_0859F29C);
            DeactivateAllTextPrinters();
            break;
        case 8:
            decompress_and_copy_tile_data_to_vram(1, sUnknown_0859F2B8[sMailRead->mailType].tiles, 0, 0, 0);
            break;
        case 9:
            if (free_temp_tile_data_buffers_if_possible())
            {
                return FALSE;
            }
            break;
        case 10:
            FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 30, 20);
            FillBgTilemapBufferRect_Palette0(2, 1, 0, 0, 30, 20);
            CopyToBgTilemapBuffer(1, sUnknown_0859F2B8[sMailRead->mailType].tileMap, 0, 0);
            break;
        case 11:
            CopyBgTilemapBufferToVram(0);
            CopyBgTilemapBufferToVram(1);
            CopyBgTilemapBufferToVram(2);
            break;
        case 12:
            LoadPalette(GetOverworldTextboxPalettePtr(), 240, 32);
            gPlttBufferUnfaded[250] = sUnknown_0859F2B8[sMailRead->mailType].color10;
            gPlttBufferFaded[250] = sUnknown_0859F2B8[sMailRead->mailType].color10;
            gPlttBufferUnfaded[251] = sUnknown_0859F2B8[sMailRead->mailType].color12;
            gPlttBufferFaded[251] = sUnknown_0859F2B8[sMailRead->mailType].color12;
            LoadPalette(sUnknown_0859F2B8[sMailRead->mailType].palette, 0, 32);
            gPlttBufferUnfaded[10] = sUnknown_0859F2B0[gSaveBlock2Ptr->playerGender][0];
            gPlttBufferFaded[10] = sUnknown_0859F2B0[gSaveBlock2Ptr->playerGender][0];
            gPlttBufferUnfaded[11] = sUnknown_0859F2B0[gSaveBlock2Ptr->playerGender][1];
            gPlttBufferFaded[11] = sUnknown_0859F2B0[gSaveBlock2Ptr->playerGender][1];
            break;
        case 13:
            if (sMailRead->flag)
            {
                sub_8121A1C();
            }
            break;
        case 14:
            if (sMailRead->flag)
            {
                sub_8121B1C();
                RunTextPrinters();
            }
            break;
        case 15:
            if (sub_8087598() == TRUE)
            {
                return FALSE;
            }
            break;
        case 16:
            SetVBlankCallback(VBlankCB_MailRead);
            gPaletteFade.bufferTransferDisabled = TRUE;
            break;
        case 17:
            icon = sub_80D2E84(sMailRead->mail->species);
            switch (sMailRead->animsActive)
            {
                case 1:
                    sub_80D2F68(icon);
                    sMailRead->monIconSprite = sub_80D2D78(icon, SpriteCallbackDummy, 0x60, 0x80, 0, 0);
                    break;
                case 2:
                    sub_80D2F68(icon);
                    sMailRead->monIconSprite = sub_80D2D78(icon, SpriteCallbackDummy, 0x28, 0x80, 0, 0);
                    break;
            }
            break;
        case 18:
            SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
            ShowBg(0);
            ShowBg(1);
            ShowBg(2);
            BeginNormalPaletteFade(-1, 0, 16, 0, 0);
            gPaletteFade.bufferTransferDisabled = FALSE;
            sMailRead->callback2 = CB2_WaitForPaletteExitOnKeyPress;
            return TRUE;
        default:
            return FALSE;
    }
    gMain.state++;
    return FALSE;
}

static void CB2_InitMailRead(void)
{
    do
    {
        if (MailReadBuildGraphics() == TRUE)
        {
            SetMainCallback2(CB2_MailRead);
            break;
        }
    } while (sub_81221AC() != TRUE);
}

static void sub_8121A1C(void)
{
    u16 i;
    u8 total;
    u8 *ptr;

    total = 0;
    for (i = 0; i < sMailRead->layout->numSubStructs; i ++)
    {
        ConvertEasyChatWordsToString(sMailRead->strbuf[i], &sMailRead->mail->words[total], sMailRead->layout->var8[i].numEasyChatWords, 1);
        total += sMailRead->layout->var8[i].numEasyChatWords;
    }
    ptr = StringCopy(sMailRead->playerName, sMailRead->mail->playerName);
    if (!sMailRead->playerIsSender)
    {
        StringCopy(ptr, gText_FromSpace);
        sMailRead->signatureWidth = sMailRead->layout->signatureWidth - (StringLength(sMailRead->playerName) * 8 - 0x60);
    }
    else
    {
        sub_81DB52C(sMailRead->playerName);
        sMailRead->signatureWidth = sMailRead->layout->signatureWidth;
    }
}

static void sub_8121B1C(void)
{
    u16 i;
    u8 strbuf[0x20];
    u8 y;
    u8 *bufptr;
    s32 box_x;
    s32 box_y;

    y = 0;
    PutWindowTilemap(0);
    PutWindowTilemap(1);
    FillWindowPixelBuffer(0, 0);
    FillWindowPixelBuffer(1, 0);
    for (i = 0; i < sMailRead->layout->numSubStructs; i ++)
    {
        if (sMailRead->strbuf[i][0] == EOS || sMailRead->strbuf[i][0] == CHAR_SPACE)
        {
            continue;
        }
        box_print(0, 1, sMailRead->layout->var8[i].xOffset + sMailRead->layout->wordsYPos, y + sMailRead->layout->wordsXPos, sUnknown_0859F2AC, 0, sMailRead->strbuf[i]);
        y += sMailRead->layout->var8[i].lineHeight;
    }
    bufptr = StringCopy(strbuf, gText_FromSpace);
    StringCopy(bufptr, sMailRead->playerName);
    box_x = GetStringCenterAlignXOffset(1, strbuf, sMailRead->signatureWidth) + 0x68;
    box_y = sMailRead->layout->signatureYPos + 0x58;
    box_print(0, 1, box_x, box_y, sUnknown_0859F2AC, 0, strbuf);
    CopyWindowToVram(0, 3);
    CopyWindowToVram(1, 3);
}

static void VBlankCB_MailRead(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_MailRead(void)
{
    if (sMailRead->animsActive != 0)
    {
        AnimateSprites();
        BuildOamBuffer();
    }
    sMailRead->callback2();
}

static void CB2_WaitForPaletteExitOnKeyPress(void)
{
    if (!UpdatePaletteFade())
    {
        sMailRead->callback2 = CB2_ExitOnKeyPress;
    }
}

static void CB2_ExitOnKeyPress(void)
{
    if (gMain.newKeys & (A_BUTTON | B_BUTTON))
    {
        BeginNormalPaletteFade(-1, 0, 0, 16, 0);
        sMailRead->callback2 = CB2_ExitMailReadFreeVars;
    }
}

static void CB2_ExitMailReadFreeVars(void)
{
    if (!UpdatePaletteFade())
    {
        SetMainCallback2(sMailRead->callback);
        switch (sMailRead->animsActive)
        {
            case 1:
            case 2:
                sub_80D2FF0(sub_80D2E84(sMailRead->mail->species));
                sub_80D2EF8(&gSprites[sMailRead->monIconSprite]);
        }
        memset(sMailRead, 0, sizeof(*sMailRead));
        ResetPaletteFade();
        UnsetBgTilemapBuffer(0);
        UnsetBgTilemapBuffer(1);
        ResetBgsAndClearDma3BusyFlags(0);
        FreeAllWindowBuffers();
        FREE_AND_SET_NULL(sMailRead);
    }
}
