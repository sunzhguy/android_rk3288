/*
 * MPC823 and PXA LCD Controller
 *
 * Modeled after video interface by Paolo Scaffardi
 *
 *
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _LCD_H_
#define _LCD_H_

extern char lcd_is_enabled;

extern int lcd_line_length;

extern struct vidinfo panel_info;

void lcd_ctrl_init(void *lcdbase);
void lcd_enable(void);

/* setcolreg used in 8bpp/16bpp; initcolregs used in monochrome */
void lcd_setcolreg(ushort regno, ushort red, ushort green, ushort blue);
void lcd_initcolregs(void);

/* gunzip_bmp used if CONFIG_VIDEO_BMP_GZIP */
struct bmp_image *gunzip_bmp(unsigned long addr, unsigned long *lenp,
			     void **alloc_addr);
int bmp_display(ulong addr, int x, int y);

/**
 * Set whether we need to flush the dcache when changing the LCD image. This
 * defaults to off.
 *
 * @param flush		non-zero to flush cache after update, 0 to skip
 */
void lcd_set_flush_dcache(int flush);

#if defined CONFIG_MPC823
/*
 * LCD controller stucture for MPC823 CPU
 */
typedef struct vidinfo {
	ushort	vl_col;		/* Number of columns (i.e. 640) */
	ushort	vl_row;		/* Number of rows (i.e. 480) */
	ushort	vl_width;	/* Width of display area in millimeters */
	ushort	vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u_char	vl_clkp;	/* Clock polarity */
	u_char	vl_oep;		/* Output Enable polarity */
	u_char	vl_hsp;		/* Horizontal Sync polarity */
	u_char	vl_vsp;		/* Vertical Sync polarity */
	u_char	vl_dp;		/* Data polarity */
	u_char	vl_bpix;	/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8 */
	u_char	vl_lbw;		/* LCD Bus width, 0 = 4, 1 = 8 */
	u_char	vl_splt;	/* Split display, 0 = single-scan, 1 = dual-scan */
	u_char	vl_clor;	/* Color, 0 = mono, 1 = color */
	u_char	vl_tft;		/* 0 = passive, 1 = TFT */

	/* Horizontal control register. Timing from data sheet */
	ushort	vl_wbl;		/* Wait between lines */

	/* Vertical control register */
	u_char	vl_vpw;		/* Vertical sync pulse width */
	u_char	vl_lcdac;	/* LCD AC timing */
	u_char	vl_wbf;		/* Wait between frames */
} vidinfo_t;

#elif defined(CONFIG_CPU_PXA25X) || defined(CONFIG_CPU_PXA27X) || \
	defined CONFIG_CPU_MONAHANS
/*
 * PXA LCD DMA descriptor
 */
struct pxafb_dma_descriptor {
	u_long	fdadr;		/* Frame descriptor address register */
	u_long	fsadr;		/* Frame source address register */
	u_long	fidr;		/* Frame ID register */
	u_long	ldcmd;		/* Command register */
};

/*
 * PXA LCD info
 */
struct pxafb_info {

	/* Misc registers */
	u_long	reg_lccr3;
	u_long	reg_lccr2;
	u_long	reg_lccr1;
	u_long	reg_lccr0;
	u_long	fdadr0;
	u_long	fdadr1;

	/* DMA descriptors */
	struct	pxafb_dma_descriptor *	dmadesc_fblow;
	struct	pxafb_dma_descriptor *	dmadesc_fbhigh;
	struct	pxafb_dma_descriptor *	dmadesc_palette;

	u_long	screen;		/* physical address of frame buffer */
	u_long	palette;	/* physical address of palette memory */
	u_int	palette_size;
};

/*
 * LCD controller stucture for PXA CPU
 */
typedef struct vidinfo {
	ushort	vl_col;		/* Number of columns (i.e. 640) */
	ushort	vl_row;		/* Number of rows (i.e. 480) */
	ushort	vl_width;	/* Width of display area in millimeters */
	ushort	vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u_char	vl_clkp;	/* Clock polarity */
	u_char	vl_oep;		/* Output Enable polarity */
	u_char	vl_hsp;		/* Horizontal Sync polarity */
	u_char	vl_vsp;		/* Vertical Sync polarity */
	u_char	vl_dp;		/* Data polarity */
	u_char	vl_bpix;	/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16 */
	u_char	vl_lbw;		/* LCD Bus width, 0 = 4, 1 = 8 */
	u_char	vl_splt;	/* Split display, 0 = single-scan, 1 = dual-scan */
	u_char	vl_clor;	/* Color, 0 = mono, 1 = color */
	u_char	vl_tft;		/* 0 = passive, 1 = TFT */

	/* Horizontal control register. Timing from data sheet */
	ushort	vl_hpw;		/* Horz sync pulse width */
	u_char	vl_blw;		/* Wait before of line */
	u_char	vl_elw;		/* Wait end of line */

	/* Vertical control register. */
	u_char	vl_vpw;		/* Vertical sync pulse width */
	u_char	vl_bfw;		/* Wait before of frame */
	u_char	vl_efw;		/* Wait end of frame */

	/* PXA LCD controller params */
	struct	pxafb_info pxa;
} vidinfo_t;

#elif defined(CONFIG_ATMEL_LCD) || defined(CONFIG_ATMEL_HLCD)

typedef struct vidinfo {
	ushort vl_col;		/* Number of columns (i.e. 640) */
	ushort vl_row;		/* Number of rows (i.e. 480) */
	u_long vl_clk;	/* pixel clock in ps    */

	/* LCD configuration register */
	u_long vl_sync;		/* Horizontal / vertical sync */
	u_long vl_bpix;		/* Bits per pixel, 0 = 1, 1 = 2, 2 = 4, 3 = 8, 4 = 16 */
	u_long vl_tft;		/* 0 = passive, 1 = TFT */
	u_long vl_cont_pol_low;	/* contrast polarity is low */
	u_long vl_clk_pol;	/* clock polarity */

	/* Horizontal control register. */
	u_long vl_hsync_len;	/* Length of horizontal sync */
	u_long vl_left_margin;	/* Time from sync to picture */
	u_long vl_right_margin;	/* Time from picture to sync */

	/* Vertical control register. */
	u_long vl_vsync_len;	/* Length of vertical sync */
	u_long vl_upper_margin;	/* Time from sync to picture */
	u_long vl_lower_margin;	/* Time from picture to sync */

	u_long	mmio;		/* Memory mapped registers */
} vidinfo_t;

#elif defined(CONFIG_EXYNOS_FB)

enum {
	FIMD_RGB_INTERFACE = 1,
	FIMD_CPU_INTERFACE = 2,
};

enum exynos_fb_rgb_mode_t {
	MODE_RGB_P = 0,
	MODE_BGR_P = 1,
	MODE_RGB_S = 2,
	MODE_BGR_S = 3,
};

typedef struct vidinfo {
	ushort vl_col;		/* Number of columns (i.e. 640) */
	ushort vl_row;		/* Number of rows (i.e. 480) */
	ushort vl_width;	/* Width of display area in millimeters */
	ushort vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u_char vl_freq;		/* Frequency */
	u_char vl_clkp;		/* Clock polarity */
	u_char vl_oep;		/* Output Enable polarity */
	u_char vl_hsp;		/* Horizontal Sync polarity */
	u_char vl_vsp;		/* Vertical Sync polarity */
	u_char vl_dp;		/* Data polarity */
	u_char vl_bpix;		/* Bits per pixel */

	/* Horizontal control register. Timing from data sheet */
	u_char vl_hspw;		/* Horz sync pulse width */
	u_char vl_hfpd;		/* Wait before of line */
	u_char vl_hbpd;		/* Wait end of line */

	/* Vertical control register. */
	u_char	vl_vspw;	/* Vertical sync pulse width */
	u_char	vl_vfpd;	/* Wait before of frame */
	u_char	vl_vbpd;	/* Wait end of frame */
	u_char  vl_cmd_allow_len; /* Wait end of frame */

	unsigned int win_id;
	unsigned int init_delay;
	unsigned int power_on_delay;
	unsigned int reset_delay;
	unsigned int interface_mode;
	unsigned int mipi_enabled;
	unsigned int dp_enabled;
	unsigned int cs_setup;
	unsigned int wr_setup;
	unsigned int wr_act;
	unsigned int wr_hold;
	unsigned int logo_on;
	unsigned int logo_width;
	unsigned int logo_height;
	int logo_x_offset;
	int logo_y_offset;
	unsigned long logo_addr;
	unsigned int rgb_mode;
	unsigned int resolution;

	/* parent clock name(MPLL, EPLL or VPLL) */
	unsigned int pclk_name;
	/* ratio value for source clock from parent clock. */
	unsigned int sclk_div;

	unsigned int dual_lcd_enabled;
} vidinfo_t;

void init_panel_info(vidinfo_t *vid);

#elif defined(CONFIG_RK_FB)

#ifdef CONFIG_RK_3168_FB
enum exynos_fb_data_format_t {
	RGB888 = 0,
	RGB565 = 1,
	YUV422 = 2,
	YUV420 = 3,
	YUV444 = 5,
	AYUV = 6,
};
#else
enum exynos_fb_data_format_t {
	ARGB888 = 0,
	RGB888 = 1,
	RGB565 = 2,
	YUV420 = 4,
	YUV422 = 5,
	YUV444 = 6,
};

#endif

enum lay_id {
	WIN0 = 0,
	WIN1,
	NUM_LAYERS,
};

struct fb_dsp_info{
	enum lay_id layer_id;
	enum exynos_fb_data_format_t format;
	u32 yaddr;
	u32 cbraddr;  // Cbr memory start address
	u32 xpos;         //start point in panel  --->LCDC_WINx_DSP_ST
	u32 ypos;
	u16 xsize;        // display window width/height  -->LCDC_WINx_DSP_INFO
	u16 ysize;          
	u16 xact;        //origin display window size -->LCDC_WINx_ACT_INFO
	u16 yact;
	u16 xvir;       //virtual width/height     -->LCDC_WINx_VIR
	u16 yvir;
};
struct layer_par {
	int id;
	bool state; 	//on or off
	struct fb_dsp_info fb_info;
	u8 fmt_cfg;
};
typedef struct vidinfo {
	u_char lcd_face;    /* lcd rgb tye (i.e. RGB888) */
	ushort vl_col;		/* Number of columns (i.e. 640) */
	ushort vl_row;		/* Number of rows (i.e. 480) */
	ushort vl_width;	/* Width of display area in millimeters */
	ushort vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	int vl_freq;		/* Frequency */
	u_char vl_clkp;		/* Clock polarity */
	u_char vl_oep;		/* Output Enable polarity */
	u_char vl_hsp;		/* Horizontal Sync polarity */
	u_char vl_vsp;		/* Vertical Sync polarity */
	u_char vl_bpix;		/* Bits per pixel */

	/* Horizontal control register. Timing from data sheet */
	ushort vl_hspw;		/* Horz sync pulse width */
	ushort vl_hfpd;		/* Wait before of line */
	ushort vl_hbpd;		/* Wait end of line */

	/* Vertical control register. */
	ushort	vl_vspw;	/* Vertical sync pulse width */
	ushort	vl_vfpd;	/* Wait before of frame */
	ushort	vl_vbpd;	/* Wait end of frame */
	u_char  vl_swap_rb;
	struct layer_par par[NUM_LAYERS];

	unsigned int lcdc_id;
	unsigned int init_delay;
	unsigned int power_on_delay;
	unsigned int reset_delay;
	unsigned int interface_mode;
	unsigned int lvds_format;
	unsigned int lvds_ttl_en;
	unsigned int cs_setup;
	unsigned int wr_setup;
	unsigned int logo_width;
	unsigned int logo_height;
	unsigned long logo_addr;
	unsigned int logo_rgb_mode;
	unsigned int resolution;
	unsigned int real_freq;
	unsigned int pixelrepeat;
	unsigned int vmode;//interlace mode
	unsigned int color_mode;
    
	/* parent clock name(MPLL, EPLL or VPLL) */
	unsigned int pclk_name;
	/* ratio value for source clock from parent clock. */
	unsigned int sclk_div;

	unsigned int dual_lcd_enabled;
		

    	u_char screen_type;
    	u_char lvds_ch_nr;

} vidinfo_t;

void init_panel_info(vidinfo_t *vid);
void rk_lcdc_set_par(struct fb_dsp_info *fb_info, vidinfo_t *vid);
int rk_lcdc_load_screen(vidinfo_t *vid);
int rk_lcdc_init(int lcdc_id);
void get_rk_logo_info(vidinfo_t *vid);

#else

typedef struct vidinfo {
	ushort	vl_col;		/* Number of columns (i.e. 160) */
	ushort	vl_row;		/* Number of rows (i.e. 100) */

	u_char	vl_bpix;	/* Bits per pixel, 0 = 1 */

	ushort	*cmap;		/* Pointer to the colormap */

	void	*priv;		/* Pointer to driver-specific data */
} vidinfo_t;

#endif /* CONFIG_MPC823, CONFIG_CPU_PXA25X, CONFIG_MCC200, CONFIG_ATMEL_LCD */

extern vidinfo_t panel_info;

/* Video functions */

void	lcd_putc(const char c);
void	lcd_puts(const char *s);
void	lcd_printf(const char *fmt, ...);
void	lcd_clear(void);
int	lcd_display_bitmap(ulong bmp_image, int x, int y);

int lcd_display_bitmap_center(ulong bmp_image);
void lcd_enable_logo(bool enable);

/**
 * Get the width of the LCD in pixels
 *
 * @return width of LCD in pixels
 */
int lcd_get_pixel_width(void);

/**
 * Get the height of the LCD in pixels
 *
 * @return height of LCD in pixels
 */
int lcd_get_pixel_height(void);

/**
 * Get the number of text lines/rows on the LCD
 *
 * @return number of rows
 */
int lcd_get_screen_rows(void);

/**
 * Get the number of text columns on the LCD
 *
 * @return number of columns
 */
int lcd_get_screen_columns(void);

/**
 * Set the position of the text cursor
 *
 * @param col	Column to place cursor (0 = left side)
 * @param row	Row to place cursor (0 = top line)
 */
void lcd_position_cursor(unsigned col, unsigned row);

/* Allow boards to customize the information displayed */
void lcd_show_board_info(void);

/* Return the size of the LCD frame buffer, and the line length */
int lcd_get_size(int *line_length);

int lcd_dt_simplefb_add_node(void *blob);
int lcd_dt_simplefb_enable_existing_node(void *blob);

/* Update the LCD / flush the cache */
void lcd_sync(void);

/************************************************************************/
/* ** BITMAP DISPLAY SUPPORT						*/
/************************************************************************/
#if defined(CONFIG_CMD_BMP) || defined(CONFIG_SPLASH_SCREEN)
# include <bmp_layout.h>
# include <asm/byteorder.h>
#endif

/*
 *  Information about displays we are using. This is for configuring
 *  the LCD controller and memory allocation. Someone has to know what
 *  is connected, as we can't autodetect anything.
 */
#define CONFIG_SYS_HIGH	0	/* Pins are active high			*/
#define CONFIG_SYS_LOW	1	/* Pins are active low			*/

#define LCD_MONOCHROME	0
#define LCD_COLOR2	1
#define LCD_COLOR4	2
#define LCD_COLOR8	3
#define LCD_COLOR16	4
#define LCD_COLOR32	5
/*----------------------------------------------------------------------*/
#if defined(CONFIG_LCD_INFO_BELOW_LOGO)
# define LCD_INFO_X		0
# define LCD_INFO_Y		(BMP_LOGO_HEIGHT + VIDEO_FONT_HEIGHT)
#elif defined(CONFIG_LCD_LOGO)
# define LCD_INFO_X		(BMP_LOGO_WIDTH + 4 * VIDEO_FONT_WIDTH)
# define LCD_INFO_Y		VIDEO_FONT_HEIGHT
#else
# define LCD_INFO_X		VIDEO_FONT_WIDTH
# define LCD_INFO_Y		VIDEO_FONT_HEIGHT
#endif

/* Default to 8bpp if bit depth not specified */
#ifndef LCD_BPP
# define LCD_BPP			LCD_COLOR8
#endif
#ifndef LCD_DF
# define LCD_DF			1
#endif

/* Calculate nr. of bits per pixel  and nr. of colors */
#define NBITS(bit_code)		(1 << (bit_code))
#define NCOLORS(bit_code)	(1 << NBITS(bit_code))

/************************************************************************/
/* ** CONSOLE CONSTANTS							*/
/************************************************************************/
#if LCD_BPP == LCD_COLOR8

/*
 * 8bpp color definitions
 */
# define CONSOLE_COLOR_BLACK	0
# define CONSOLE_COLOR_RED	1
# define CONSOLE_COLOR_GREEN	2
# define CONSOLE_COLOR_YELLOW	3
# define CONSOLE_COLOR_BLUE	4
# define CONSOLE_COLOR_MAGENTA	5
# define CONSOLE_COLOR_CYAN	6
# define CONSOLE_COLOR_GREY	14
# define CONSOLE_COLOR_WHITE	15	/* Must remain last / highest	*/

#elif LCD_BPP == LCD_COLOR32
/*
 * 32bpp color definitions
 */
# define CONSOLE_COLOR_RED	0x00ff0000
# define CONSOLE_COLOR_GREEN	0x0000ff00
# define CONSOLE_COLOR_YELLOW	0x00ffff00
# define CONSOLE_COLOR_BLUE	0x000000ff
# define CONSOLE_COLOR_MAGENTA	0x00ff00ff
# define CONSOLE_COLOR_CYAN	0x0000ffff
# define CONSOLE_COLOR_GREY	0x00aaaaaa
# define CONSOLE_COLOR_BLACK	0x00000000
# define CONSOLE_COLOR_WHITE	0x00ffffff	/* Must remain last / highest*/
# define NBYTES(bit_code)	(NBITS(bit_code) >> 3)

#else

/*
 * 16bpp color definitions
 */
#define CONSOLE_COLOR_BLACK	0x0000
#define CONSOLE_COLOR_WHITE	0xffff	/* Must remain last / highest	*/

#endif /* color definitions */

/************************************************************************/
#ifndef PAGE_SIZE
# define PAGE_SIZE	4096
#endif

/************************************************************************/

#endif	/* _LCD_H_ */
