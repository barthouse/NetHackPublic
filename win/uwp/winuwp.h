/* NetHack 3.6	winuwp.h	$NHDT-Date:  $  $NHDT-Branch:  $:$NHDT-Revision:  $ */
/* Copyright (c) Bart House, 2016-2017. */
/* Nethack for the Universal Windows Platform (UWP) */
/* NetHack may be freely redistributed.  See license for details. */
#pragma once

#ifndef UWP_GRAPHICS
#error UWP_GRAPHICS must be defined
#endif

/* menu structure */
typedef struct tty_mi {
    anything identifier; /* user identifier */
    long count;          /* user count */
    std::string str;           /* description string (including accelerator) */
    int attr;            /* string attribute */
    boolean selected;    /* TRUE if selected by user */
    char selector;       /* keyboard accelerator */
    char gselector;      /* group accelerator */
} tty_menu_item;

typedef std::list<tty_menu_item>::iterator itemIter;

/* struct for windows */
struct CoreWindow {

    CoreWindow(int type, winid window);
    virtual ~CoreWindow();

    virtual void Init();

    virtual void Clear();
    virtual void Display(bool blocking) = 0;
    virtual void Dismiss();
    virtual void Destroy();
    virtual void Render(std::vector<Nethack::TextCell> & cells);

    void set_cursor(int x, int y);
    void clear_to_end_of_line();
    void clear_to_end_of_screen();
    void clear_whole_screen();
    void clear_window();

    virtual void Putstr(int attr, const char *str) = 0;
    virtual void Putsym(int x, int y, char ch);

    void core_putc(char ch, Nethack::TextColor textColor = Nethack::TextColor::NoColor, Nethack::TextAttribute textAttribute = Nethack::TextAttribute::None);
    void core_puts(const char *s, Nethack::TextColor textColor = Nethack::TextColor::NoColor, Nethack::TextAttribute textAttribute = Nethack::TextAttribute::None);

    int wait_for_response(const char *s, int dismiss_more = 0);

    const char *compress_str(const char *);

    void bail(const char *mesg);

    winid m_window;         /* winid */
    int m_flags;           /* window flags */
    xchar m_type;          /* type of window */
    bool m_active;      /* true if window is active */
    int m_offx, m_offy;    /* offset from topleft of display */
    long m_rows, m_cols;     /* dimensions */
    long m_curx, m_cury;     /* current cursor position */
    std::string m_morestr;         /* string to display instead of default */

    void cells_set_dimensions(int x, int y);
    void cells_puts(int x, int y, const char *, Nethack::TextColor textColor = Nethack::TextColor::NoColor, Nethack::TextAttribute textAttribute = Nethack::TextAttribute::None);
    void cells_put(int x, int y, const char c, Nethack::TextColor textColor = Nethack::TextColor::NoColor, Nethack::TextAttribute textAttribute = Nethack::TextAttribute::None);
    void cells_clear_to_end_of_window(int x, int y);
    void cells_clear_to_end_of_line(int x, int y);
    void cells_clear_window();

    int m_dimx, m_dimy; /* dimension of cells */
    int m_cell_count;
    std::vector<Nethack::TextCell> m_cells;
};

static const int kMaxMessageHistoryLength = 60;
static const int kMinMessageHistoryLength = 20;

typedef boolean FDECL((*getlin_hook_proc), (char *));

struct MessageWindow : public CoreWindow {

    MessageWindow();
    virtual ~MessageWindow();

    virtual void Init();

    virtual void Clear();
    virtual void Display(bool blocking);
    virtual void Dismiss();
    virtual void Putstr(int attr, const char *str);
    void Putstr(int attr, const char *str, bool isPrompt);
    virtual void Destroy();
    virtual void Prompt(const char *str);

    virtual void PrepareForInput();

    char yn_function(const char *query, const char *resp, char def);
    int doprev_message();
    int redotoplin(const char *str, int dismiss_more = 0, bool isPrompt = false);
    void put_topline(const char *str);
    void hooked_tty_getlin(const char * query, char * buf, getlin_hook_proc = NULL, int bufSize = BUFSZ);
    void putsyms(const char *str, Nethack::TextColor textColor, Nethack::TextAttribute textAttribute);
    void topl_putsym(char c, Nethack::TextColor color, Nethack::TextAttribute attribute);
    void remember_topl();
    void addtopl(const char *s);
    void addtopl(char c);
    int more(int dismiss_more = 0);
    char uwp_message_menu(char let, int how, const char *mesg);
    void erase_message();

    int handle_prev_message();
    int display_message_history(bool reverse = false);

    void uwp_putmsghistory(const char *msg, boolean restoring_msghist);
    char *uwp_getmsghistory(boolean init);
    void msghistory_snapshot(boolean purge);

    void compare_output();

    std::string m_toplines;
    std::list<std::string> m_snapshot_msgList;
    std::list<std::string> m_msgList;
    std::list<std::string>::iterator m_msgIter;
    bool m_mustBeSeen;      /* message must be seen */
    bool m_mustBeErased;    /* message must be erased */
    bool m_outputMessages;  /* output messages.  Outputting of message can be turned off.
                             * when the player hits escape when they are prompted to acknowledge a
                             * message, the player is indicating that
                             * the game should stop outputting messages and thus stop requiring
                             * acknowledgement for the remainder of the turn processing.
                             * messages are always recorded in message history.
                             * if the game needs player input, it must set output messages.
                             */
    std::vector<Nethack::TextCell> m_output;
};

struct MenuWindow : public CoreWindow {

    MenuWindow(winid window);
    virtual ~MenuWindow();

    virtual void Clear();
    virtual void Display(bool blocking);
    virtual void Dismiss();
    virtual void Putstr(int attr, const char *str);
    virtual void Destroy();

    void set_all_on_page(itemIter page_start, itemIter page_end);
    void unset_all_on_page(itemIter page_start, itemIter page_end);
    void invert_all_on_page(itemIter page_start, itemIter page_end, char acc);
    void invert_all(itemIter page_start, itemIter page_end, char acc);
    boolean toggle_menu_curr(tty_menu_item & curr, int lineno, boolean in_view, boolean counting, long count);
    void set_item_state(int lineno, tty_menu_item & item);

    void process_lines();
    void process_menu();

    int uwp_select_menu(int how, menu_item **menu_list);
    void uwp_add_menu(const anything *identifier, char ch, char gch, int attr, const char *str, boolean preselected);
    void uwp_end_menu(const char *prompt);

    int dmore(const char *s); /* valid responses */

    std::list<std::pair<int, std::string>> m_lines;

    std::list<tty_menu_item> m_items;
    std::vector<itemIter> m_pages;

    long m_npages;           /* number of pages in menu (MENU) */
    long m_nitems;           /* total number of items (MENU) */
    short m_how;             /* menu mode - pick 1 or N (MENU) */
    char m_menu_ch;          /* menu char (MENU) */

    bool m_cancelled;

};

struct BaseWindow : public CoreWindow {
    BaseWindow();
    virtual ~BaseWindow();

    virtual void Clear();
    virtual void Display(bool blocking);
    virtual void Dismiss();
    virtual void Putstr(int attr, const char *str);

};

struct StatusWindow : public CoreWindow {
    StatusWindow();
    virtual ~StatusWindow();

    virtual void Clear();
    virtual void Display(bool blocking);
    virtual void Dismiss();
    virtual void Putstr(int attr, const char *str);

    static const int kStatusHeight = 2;
    static const int kStatusWidth = 80;

    long m_statusWidth;
    long m_statusHeight;
    char m_statusLines[kStatusHeight][kStatusWidth];
};

struct MapWindow : public CoreWindow {
    MapWindow();
    virtual ~MapWindow();

    virtual void Clear();
    virtual void Display(bool blocking);
    virtual void Dismiss();
    virtual void Putstr(int attr, const char *str);
    virtual void Destroy();

};

struct TextWindow : public CoreWindow {
    TextWindow(winid window);
    virtual ~TextWindow();

    virtual void Clear();
    virtual void Display(bool blocking);
    virtual void Dismiss();
    virtual void Putstr(int attr, const char *str);

    int dmore(const char *s); /* valid responses */

    std::list<std::pair<Nethack::TextAttribute, std::string>> m_lines;

    bool m_cancelled;

};

extern std::list<CoreWindow *> g_render_list;

extern "C" {

/* window flags */
#define WIN_CANCELLED 1
#define WIN_STOP 1        /* for NHW_MESSAGE; stops output */

/* descriptor for tty-based displays -- all the per-display data */
struct DisplayDesc {
    int rows, cols; /* width and height of tty display */
    int rawprint;      /* number of raw_printed lines since synch */
    char dismiss_more; /* extra character accepted at --More-- */
};

#define MAXWIN 20 /* maximum number of windows, cop-out */

/* tty dependent window types */
#ifdef NHW_BASE
#undef NHW_BASE
#endif
#define NHW_BASE 6

/* It's still not clear I've caught all the cases for H2344.  #undef this
* to back out the changes. */
#define H2344_BROKEN


extern struct window_procs uwp_procs;

/* port specific variable declarations */

extern CoreWindow *g_wins[MAXWIN];

extern char defmorestr[]; /* default --more-- prompt */

/* port specific external function references */

/* ### termcap.c, video.c ### */

extern void FDECL(tty_startup, (int *, int *));
#ifndef NO_TERMS
extern void NDECL(tty_shutdown);
#endif
#if defined(apollo)
/* Apollos don't widen old-style function definitions properly -- they try to
 * be smart and use the prototype, or some such strangeness.  So we have to
 * define UNWIDENDED_PROTOTYPES (in tradstdc.h), which makes CHAR_P below a
 * char.  But the tputs termcap call was compiled as if xputc's argument
 * actually would be expanded.	So here, we have to make an exception. */
extern void FDECL(xputc, (int));
#else
void xputc(
    char ch,
    Nethack::TextColor textColor = Nethack::TextColor::NoColor,
    Nethack::TextAttribute textAttribute = Nethack::TextAttribute::None);
#endif
void xputs(
    const char *s,
    Nethack::TextColor textColor = Nethack::TextColor::NoColor,
    Nethack::TextAttribute textAttribute = Nethack::TextAttribute::None);
#if defined(SCREEN_VGA) || defined(SCREEN_8514)
extern void FDECL(xputg, (int, int, unsigned));
#endif
extern void NDECL(clear_screen);
extern void NDECL(backsp);
extern void NDECL(graph_on);
extern void NDECL(graph_off);

/* ### topl.c ### */

extern void NDECL(more);

/* ### wintty.c ### */
extern void FDECL(win_tty_init, (int));

/* external declarations */
extern void FDECL(tty_init_nhwindows, (int *, char **));
extern void NDECL(tty_player_selection);
extern void NDECL(tty_askname);
extern void NDECL(tty_get_nh_event);
extern void FDECL(tty_exit_nhwindows, (const char *));
extern void FDECL(tty_suspend_nhwindows, (const char *));
extern void NDECL(tty_resume_nhwindows);
extern winid FDECL(tty_create_nhwindow, (int));
extern void FDECL(tty_clear_nhwindow, (winid));
extern void FDECL(tty_display_nhwindow, (winid, BOOLEAN_P));
extern void FDECL(tty_dismiss_nhwindow, (winid));
extern void FDECL(tty_destroy_nhwindow, (winid));
extern void FDECL(tty_curs, (winid, int, int));
extern void FDECL(tty_putstr, (winid, int, const char *));
extern void FDECL(tty_putsym, (winid, int, int, CHAR_P));
extern void FDECL(tty_display_file, (const char *, BOOLEAN_P));
extern void FDECL(tty_start_menu, (winid));
extern void FDECL(tty_add_menu, (winid, int, const ANY_P *, CHAR_P, CHAR_P, int,
                            const char *, BOOLEAN_P));
extern void FDECL(tty_end_menu, (winid, const char *));
extern int FDECL(tty_select_menu, (winid, int, MENU_ITEM_P **));
extern char FDECL(tty_message_menu, (CHAR_P, int, const char *));
extern void NDECL(tty_update_inventory);
extern void NDECL(tty_mark_synch);
extern void NDECL(tty_wait_synch);
#ifdef CLIPPING
extern void FDECL(tty_cliparound, (int, int));
#endif
#ifdef POSITIONBAR
extern void FDECL(tty_update_positionbar, (char *));
#endif
extern void FDECL(tty_print_glyph, (winid, XCHAR_P, XCHAR_P, int, int));
extern void FDECL(tty_raw_print, (const char *));
extern void FDECL(tty_raw_print_bold, (const char *));
extern int NDECL(tty_nhgetch);
extern int FDECL(tty_nh_poskey, (int *, int *, int *));
extern void NDECL(tty_nhbell);
extern int NDECL(tty_doprev_message);
extern char FDECL(tty_yn_function, (const char *, const char *, CHAR_P));
extern void FDECL(tty_getlin, (const char *, char *));
extern int NDECL(tty_get_ext_cmd);
extern void FDECL(tty_number_pad, (int));
extern void NDECL(tty_delay_output);
#ifdef CHANGE_COLOR
extern void FDECL(tty_change_color, (int color, long rgb, int reverse));
#ifdef MAC
extern void FDECL(tty_change_background, (int white_or_black));
extern short FDECL(set_tty_font_name, (winid, char *));
#endif
extern char *NDECL(tty_get_color_string);
#endif
#ifdef STATUS_VIA_WINDOWPORT
extern void NDECL(tty_status_init);
extern void FDECL(tty_status_update, (int, genericptr_t, int, int));
#ifdef STATUS_HILITES
extern void FDECL(tty_status_threshold, (int, int, anything, int, int, int));
#endif
#endif

/* other defs that really should go away (they're tty specific) */
extern void NDECL(tty_start_screen);
extern void NDECL(tty_end_screen);

extern void FDECL(genl_outrip, (winid, int, time_t));

extern char *FDECL(tty_getmsghistory, (BOOLEAN_P));
extern void FDECL(tty_putmsghistory, (const char *, BOOLEAN_P));

void uwp_raw_printf(Nethack::TextAttribute textAttribute, const char *, ...);


CoreWindow * GetCoreWindow(winid window);
MessageWindow * GetMessageWindow();
MenuWindow * GetMenuWindow(winid window);

static const int kKillChar = 21;

extern int g_rawprint;

extern MessageWindow g_messageWindow;
extern StatusWindow g_statusWindow;
extern MapWindow g_mapWindow;

const winid MESSAGE_WINDOW = 1;
const winid STATUS_WINDOW = 2;
const winid MAP_WINDOW = 3;
const winid FIRST_FREE_WINDOW = 4;

void uwp_render_windows();

}
