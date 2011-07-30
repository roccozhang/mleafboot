/*
 * File      : lcd_t35.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2010, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-01-01     bernard      first version from QiuYi's driver
 * 2011-05-29     swkyer       tiny6410
 */
#include <rtthread.h>
#include "s3c6410.h"
#include "tiny6410.h"
#include "lcd.h"



#define LCD_XSIZE					LCD_WIDTH
#define LCD_YSIZE					LCD_HEIGHT
#define SCR_XSIZE					LCD_WIDTH
#define SCR_YSIZE					LCD_HEIGHT

#define RT_HW_LCD_WIDTH				LCD_WIDTH
#define RT_HW_LCD_HEIGHT			LCD_HEIGHT

#define MVAL						(13)
#define MVAL_USED 					(0)		//0=each frame   1=rate by MVAL
#define INVVDEN						(1)		//0=normal       1=inverted
#define BSWP						(0)		//Byte swap control
#define HWSWP						(1)		//Half word swap control

#define GPB1_TO_OUT()				(GPBPUD &= 0xfffd, GPBCON &= 0xfffffff3, GPBCON |= 0x00000004)
#define GPB1_TO_1()					(GPBDAT |= 0x0002)
#define GPB1_TO_0()					(GPBDAT &= 0xfffd)

#define S3C2410_LCDCON1_CLKVAL(x)	((x) << 8)
#define S3C2410_LCDCON1_MMODE		(1<<7)
#define S3C2410_LCDCON1_DSCAN4		(0<<5)
#define S3C2410_LCDCON1_STN4		(1<<5)
#define S3C2410_LCDCON1_STN8		(2<<5)
#define S3C2410_LCDCON1_TFT			(3<<5)

#define S3C2410_LCDCON1_STN1BPP		(0<<1)
#define S3C2410_LCDCON1_STN2GREY	(1<<1)
#define S3C2410_LCDCON1_STN4GREY	(2<<1)
#define S3C2410_LCDCON1_STN8BPP		(3<<1)
#define S3C2410_LCDCON1_STN12BPP	(4<<1)

#define S3C2410_LCDCON1_TFT1BPP		(8<<1)
#define S3C2410_LCDCON1_TFT2BPP		(9<<1)
#define S3C2410_LCDCON1_TFT4BPP		(10<<1)
#define S3C2410_LCDCON1_TFT8BPP		(11<<1)
#define S3C2410_LCDCON1_TFT16BPP	(12<<1)
#define S3C2410_LCDCON1_TFT24BPP	(13<<1)

#define S3C2410_LCDCON1_ENVID		(1)

#define S3C2410_LCDCON1_MODEMASK	0x1E

#define S3C2410_LCDCON2_VBPD(x)		((x) << 24)
#define S3C2410_LCDCON2_LINEVAL(x)  ((x) << 14)
#define S3C2410_LCDCON2_VFPD(x)	    ((x) << 6)
#define S3C2410_LCDCON2_VSPW(x)	    ((x) << 0)

#define S3C2410_LCDCON2_GET_VBPD(x) ( ((x) >> 24) & 0xFF)
#define S3C2410_LCDCON2_GET_VFPD(x) ( ((x) >>  6) & 0xFF)
#define S3C2410_LCDCON2_GET_VSPW(x) ( ((x) >>  0) & 0x3F)

#define S3C2410_LCDCON3_HBPD(x)	    ((x) << 19)
#define S3C2410_LCDCON3_WDLY(x)	    ((x) << 19)
#define S3C2410_LCDCON3_HOZVAL(x)   ((x) << 8)
#define S3C2410_LCDCON3_HFPD(x)	    ((x) << 0)
#define S3C2410_LCDCON3_LINEBLANK(x)((x) << 0)

#define S3C2410_LCDCON3_GET_HBPD(x) ( ((x) >> 19) & 0x7F)
#define S3C2410_LCDCON3_GET_HFPD(x) ( ((x) >>  0) & 0xFF)

#define S3C2410_LCDCON4_MVAL(x)	    ((x) << 8)
#define S3C2410_LCDCON4_HSPW(x)	    ((x) << 0)
#define S3C2410_LCDCON4_WLH(x)	    ((x) << 0)

#define S3C2410_LCDCON4_GET_HSPW(x) ( ((x) >>  0) & 0xFF)

#define S3C2410_LCDCON5_BPP24BL	    (1<<12)
#define S3C2410_LCDCON5_FRM565	    (1<<11)
#define S3C2410_LCDCON5_INVVCLK	    (1<<10)
#define S3C2410_LCDCON5_INVVLINE    (1<<9)
#define S3C2410_LCDCON5_INVVFRAME   (1<<8)
#define S3C2410_LCDCON5_INVVD	    (1<<7)
#define S3C2410_LCDCON5_INVVDEN	    (1<<6)
#define S3C2410_LCDCON5_INVPWREN    (1<<5)
#define S3C2410_LCDCON5_INVLEND	    (1<<4)
#define S3C2410_LCDCON5_PWREN	    (1<<3)
#define S3C2410_LCDCON5_ENLEND	    (1<<2)
#define S3C2410_LCDCON5_BSWP	    (1<<1)
#define S3C2410_LCDCON5_HWSWP	    (1<<0)

#define	S3C2410_LCDINT_FRSYNC		(1<<1)

static volatile rt_uint16_t _rt_framebuffer[RT_HW_LCD_HEIGHT][RT_HW_LCD_WIDTH];
static volatile rt_uint16_t _rt_hw_framebuffer[RT_HW_LCD_HEIGHT][RT_HW_LCD_WIDTH];


struct rtgui_lcd_device
{
	struct rt_device parent;

	/* byte per pixel */
	rt_uint16_t byte_per_pixel;

	/* screen width and height */
	rt_uint16_t width;
	rt_uint16_t height;

	void *hw_framebuffer;
};
static struct rtgui_lcd_device *lcd = RT_NULL;


/*
 * GPF15 = LCD backlight control
 * GPF13 => Panel power
 * GPN5 = LCD nRESET signal
 * PWM_TOUT1 => backlight brightness
 *
 */
static void lcd_power_set(int power)
{
	rt_uint32_t regv;

	if (power)
	{
		//gpio_direction_output(S3C64XX_GPF(13), 1);
		//gpio_direction_output(S3C64XX_GPF(15), 1);
		regv = s3c_readl(GPFCON) & 0x33FFFFFF;
		regv |= 0x44000000;
		s3c_write(regv, GPFCON);
		regv = s3c_readl(GPFPUD) & 0x33FFFFFF;
		s3c_write(regv, GPFPUD);
		regv = s3c_readl(GPFDAT);
		regv |= (1<<13)|(1<<15);
		s3c_writel(regv, GPFDAT);

		/* fire nRESET on power up */
		//gpio_direction_output(S3C64XX_GPN(5), 0);
		//msleep(10);
		//gpio_direction_output(S3C64XX_GPN(5), 1);
		//msleep(1);
		regv = s3c_readl(GPNCON) & ~(0x03<<10);
		regv |= (0x01<<10);
		s3c_write(regv, GPNCON);
		regv = s3c_readl(GPNDAT);
		regv &= ~(1<<5);
		s3c_writel(regv, GPNDAT);
		//msleep(10);
		regv |= (1<<5);
		s3c_writel(regv, GPNDAT);
		//msleep(1);
	}
	else
	{
		//gpio_direction_output(S3C64XX_GPF(15), 0);
		//gpio_direction_output(S3C64XX_GPF(13), 0);
		regv = s3c_readl(GPFCON) & 0x33FFFFFF;
		regv |= 0x44000000;
		s3c_write(regv, GPFCON);
		regv = s3c_readl(GPFPUD) & 0x33FFFFFF;
		s3c_write(regv, GPFPUD);
		regv = s3c_readl(GPFDAT);
		regv &= ~((1<<13)|(1<<15));
		s3c_writel(regv, GPFDAT);
	}
}


#ifdef RT_USING_RTGUI

#include <rtgui/driver.h>
#include <rtgui/color.h>

static void rt_hw_lcd_update(rtgui_rect_t *rect)
{
	rt_uint32_t i, j;

	for (i = rect->y1; i < rect->y2; i ++)
	{
		for(j = rect->x1; j < rect->x2; j++)
			_rt_hw_framebuffer[i][j] = _rt_framebuffer[i][j];
	}
}

static rt_uint8_t * rt_hw_lcd_get_framebuffer(void)
{
	return (rt_uint8_t *)_rt_framebuffer;
}

static void rt_hw_lcd_set_pixel(rtgui_color_t *c, rt_base_t x, rt_base_t y)
{
    if (x < RT_HW_LCD_WIDTH && y < RT_HW_LCD_HEIGHT)
	{
		_rt_framebuffer[(y)][(x)] = rtgui_color_to_565p(*c);
	}
}

static void rt_hw_lcd_get_pixel(rtgui_color_t *c, rt_base_t x, rt_base_t y)
{
    if (x < RT_HW_LCD_WIDTH && y < RT_HW_LCD_HEIGHT)
	{
		*c = rtgui_color_from_565p(_rt_framebuffer[(y)][(x)]);
	}
}

static void rt_hw_lcd_draw_hline(rtgui_color_t *c, rt_base_t x1, rt_base_t x2, rt_base_t y)
{
	rt_uint32_t idx;
	rt_uint16_t color;

	/* get color pixel */
	color = rtgui_color_to_565p(*c);

	for (idx = x1; idx < x2; idx ++)
	{
		_rt_framebuffer[y][idx] = color;
	}
}

static void rt_hw_lcd_draw_vline(rtgui_color_t *c, rt_base_t x, rt_base_t y1, rt_base_t y2)
{
    rt_uint32_t idy;
	rt_uint16_t color;

	/* get color pixel */
	color = rtgui_color_to_565p(*c);

	for (idy = y1; idy < y2; idy ++)
	{
		_rt_framebuffer[idy][x] = color;
	}
}

static void rt_hw_lcd_draw_raw_hline(rt_uint8_t *pixels, rt_base_t x1, rt_base_t x2, rt_base_t y)
{
    rt_memcpy((void*)&_rt_framebuffer[y][x1], pixels, (x2 - x1) * 2);
}

struct rtgui_graphic_driver _rtgui_lcd_driver =
{
	"lcd",
	2,
	RT_HW_LCD_WIDTH,
	RT_HW_LCD_HEIGHT,
	rt_hw_lcd_update,
	rt_hw_lcd_get_framebuffer,
	rt_hw_lcd_set_pixel,
	rt_hw_lcd_get_pixel,
	rt_hw_lcd_draw_hline,
	rt_hw_lcd_draw_vline,
	rt_hw_lcd_draw_raw_hline
};

#include "finsh.h"
void hline(rt_uint32_t c, int x1, int x2, int y)
{
    rtgui_color_t color = (rtgui_color_t)c;
    rt_hw_lcd_draw_hline(&color, x1, x2, y);
}
FINSH_FUNCTION_EXPORT(hline, draw a hline);

void vline(rt_uint32_t c, int x, int y1, int y2)
{
    rtgui_color_t color = (rtgui_color_t)c;
    rt_hw_lcd_draw_vline(&color, x, y1, y2);
}
FINSH_FUNCTION_EXPORT(vline, draw a vline);

void clear()
{
    int y;

	for (y = 0; y < LCD_HEIGHT; y ++)
	{
		rt_hw_lcd_draw_hline((rtgui_color_t*)&white, 0, LCD_WIDTH, y);
	}
}
FINSH_FUNCTION_EXPORT(clear, clear screen);

#endif

/* RT-Thread Device Interface */
static rt_err_t rt_lcd_init (rt_device_t dev)
{
/*	GPB1_TO_OUT();
	GPB1_TO_1();

	GPCUP  = 0x00000000;
	GPCCON = 0xaaaa02a9;

	GPDUP  = 0x00000000;
	GPDCON = 0xaaaaaaaa;

#define	M5D(n)	((n)&0x1fffff)
#define LCD_ADDR ((rt_uint32_t)_rt_hw_framebuffer)
	LCDCON1 = (LCD_PIXCLOCK << 8) | (3 <<  5) | (12 << 1);
   	LCDCON2 = (LCD_UPPER_MARGIN << 24) | ((LCD_HEIGHT - 1) << 14) | (LCD_LOWER_MARGIN << 6) | (LCD_VSYNC_LEN << 0);
   	LCDCON3 = (LCD_RIGHT_MARGIN << 19) | ((LCD_WIDTH  - 1) <<  8) | (LCD_LEFT_MARGIN << 0);
   	LCDCON4 = (13 <<  8) | (LCD_HSYNC_LEN << 0);
#if !defined(LCD_CON5)
    #define LCD_CON5 ((1<<11) | (1 << 9) | (1 << 8) | (1 << 3) | (1 << 0))
#endif
    LCDCON5   =  LCD_CON5;

    LCDSADDR1 = ((LCD_ADDR >> 22) << 21) | ((M5D(LCD_ADDR >> 1)) <<  0);
    LCDSADDR2 = M5D((LCD_ADDR + LCD_WIDTH * LCD_HEIGHT * 2) >> 1);
    LCDSADDR3 = LCD_WIDTH;

	LCDINTMSK |= (3);
	LPCSEL &= (~7) ;
	TPAL=0;

	LcdBkLtSet(70) ;
	lcd_power_enable(0, 1);
	lcd_envid_on_off(1);
*/
	return RT_EOK;
}

static rt_err_t rt_lcd_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
	switch (cmd)
	{
	case RT_DEVICE_CTRL_LCD_GET_WIDTH:
		*((rt_uint16_t*)args) = lcd->width;
		break;

	case RT_DEVICE_CTRL_LCD_GET_HEIGHT:
		*((rt_uint16_t*)args) = lcd->height;
		break;

	case RT_DEVICE_CTRL_LCD_GET_BPP:
		*((rt_uint16_t*)args) = lcd->byte_per_pixel;
		break;

	case RT_DEVICE_CTRL_LCD_GET_FRAMEBUFFER:
		*((rt_uint16_t*)args) = lcd->hw_framebuffer;
		break;
	}

	return RT_EOK;
}

void rtgui_lcd_hw_init(void)
{
	lcd = (struct rtgui_lcd_device*)rt_malloc(sizeof(struct rtgui_lcd_device));
	if (lcd == RT_NULL) return; /* no memory yet */

	/* init device structure */
	lcd->parent.type = RT_Device_Class_Unknown;
	lcd->parent.init = rt_lcd_init;
	lcd->parent.control = rt_lcd_control;
	lcd->parent.user_data = RT_NULL;
	lcd->byte_per_pixel = 2;
	lcd->width = LCD_WIDTH;
	lcd->height = LCD_HEIGHT;
	lcd->hw_framebuffer = (void*)_rt_hw_framebuffer;

	/* register touch device to RT-Thread */
	rt_device_register(&(lcd->parent), "lcd", RT_DEVICE_FLAG_RDWR);

#ifdef RT_USING_RTGUI
	/* add lcd driver into graphic driver */
	rtgui_graphic_driver_add(&_rtgui_lcd_driver);
#endif
}

