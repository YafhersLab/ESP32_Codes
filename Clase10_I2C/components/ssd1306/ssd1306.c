#include "ssd1306.h"

#define PACK8 __attribute__((aligned( __alignof__( uint8_t ) ), packed ))

static uint8_t font8x8_basic_tr[128][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0000 (nul)
    { 0x00, 0x04, 0x02, 0xFF, 0x02, 0x04, 0x00, 0x00 },   // U+0001 (Up Allow)
    { 0x00, 0x20, 0x40, 0xFF, 0x40, 0x20, 0x00, 0x00 },   // U+0002 (Down Allow)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0003
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0004
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0005
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0006
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0007
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0008
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0009
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+000F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0010
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0011
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0012
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0013
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0014
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0015
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0016
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0017
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0018
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0019
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+001F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0020 (space)
    { 0x00, 0x00, 0x06, 0x5F, 0x5F, 0x06, 0x00, 0x00 },   // U+0021 (!)
    { 0x00, 0x03, 0x03, 0x00, 0x03, 0x03, 0x00, 0x00 },   // U+0022 (")
    { 0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F, 0x14, 0x00 },   // U+0023 (#)
    { 0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12, 0x00, 0x00 },   // U+0024 ($)
    { 0x46, 0x66, 0x30, 0x18, 0x0C, 0x66, 0x62, 0x00 },   // U+0025 (%)
    { 0x30, 0x7A, 0x4F, 0x5D, 0x37, 0x7A, 0x48, 0x00 },   // U+0026 (&)
    { 0x04, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 },   // U+0027 (')
    { 0x00, 0x1C, 0x3E, 0x63, 0x41, 0x00, 0x00, 0x00 },   // U+0028 (()
    { 0x00, 0x41, 0x63, 0x3E, 0x1C, 0x00, 0x00, 0x00 },   // U+0029 ())
    { 0x08, 0x2A, 0x3E, 0x1C, 0x1C, 0x3E, 0x2A, 0x08 },   // U+002A (*)
    { 0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08, 0x00, 0x00 },   // U+002B (+)
    { 0x00, 0x80, 0xE0, 0x60, 0x00, 0x00, 0x00, 0x00 },   // U+002C (,)
    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00 },   // U+002D (-)
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00 },   // U+002E (.)
    { 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00 },   // U+002F (/)
    { 0x3E, 0x7F, 0x71, 0x59, 0x4D, 0x7F, 0x3E, 0x00 },   // U+0030 (0)
    { 0x40, 0x42, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00 },   // U+0031 (1)
    { 0x62, 0x73, 0x59, 0x49, 0x6F, 0x66, 0x00, 0x00 },   // U+0032 (2)
    { 0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00 },   // U+0033 (3)
    { 0x18, 0x1C, 0x16, 0x53, 0x7F, 0x7F, 0x50, 0x00 },   // U+0034 (4)
    { 0x27, 0x67, 0x45, 0x45, 0x7D, 0x39, 0x00, 0x00 },   // U+0035 (5)
    { 0x3C, 0x7E, 0x4B, 0x49, 0x79, 0x30, 0x00, 0x00 },   // U+0036 (6)
    { 0x03, 0x03, 0x71, 0x79, 0x0F, 0x07, 0x00, 0x00 },   // U+0037 (7)
    { 0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00 },   // U+0038 (8)
    { 0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E, 0x00, 0x00 },   // U+0039 (9)
    { 0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00 },   // U+003A (:)
    { 0x00, 0x80, 0xE6, 0x66, 0x00, 0x00, 0x00, 0x00 },   // U+003B (;)
    { 0x08, 0x1C, 0x36, 0x63, 0x41, 0x00, 0x00, 0x00 },   // U+003C (<)
    { 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00 },   // U+003D (=)
    { 0x00, 0x41, 0x63, 0x36, 0x1C, 0x08, 0x00, 0x00 },   // U+003E (>)
    { 0x02, 0x03, 0x51, 0x59, 0x0F, 0x06, 0x00, 0x00 },   // U+003F (?)
    { 0x3E, 0x7F, 0x41, 0x5D, 0x5D, 0x1F, 0x1E, 0x00 },   // U+0040 (@)
    { 0x7C, 0x7E, 0x13, 0x13, 0x7E, 0x7C, 0x00, 0x00 },   // U+0041 (A)
    { 0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00 },   // U+0042 (B)
    { 0x1C, 0x3E, 0x63, 0x41, 0x41, 0x63, 0x22, 0x00 },   // U+0043 (C)
    { 0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E, 0x1C, 0x00 },   // U+0044 (D)
    { 0x41, 0x7F, 0x7F, 0x49, 0x5D, 0x41, 0x63, 0x00 },   // U+0045 (E)
    { 0x41, 0x7F, 0x7F, 0x49, 0x1D, 0x01, 0x03, 0x00 },   // U+0046 (F)
    { 0x1C, 0x3E, 0x63, 0x41, 0x51, 0x73, 0x72, 0x00 },   // U+0047 (G)
    { 0x7F, 0x7F, 0x08, 0x08, 0x7F, 0x7F, 0x00, 0x00 },   // U+0048 (H)
    { 0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00, 0x00, 0x00 },   // U+0049 (I)
    { 0x30, 0x70, 0x40, 0x41, 0x7F, 0x3F, 0x01, 0x00 },   // U+004A (J)
    { 0x41, 0x7F, 0x7F, 0x08, 0x1C, 0x77, 0x63, 0x00 },   // U+004B (K)
    { 0x41, 0x7F, 0x7F, 0x41, 0x40, 0x60, 0x70, 0x00 },   // U+004C (L)
    { 0x7F, 0x7F, 0x0E, 0x1C, 0x0E, 0x7F, 0x7F, 0x00 },   // U+004D (M)
    { 0x7F, 0x7F, 0x06, 0x0C, 0x18, 0x7F, 0x7F, 0x00 },   // U+004E (N)
    { 0x1C, 0x3E, 0x63, 0x41, 0x63, 0x3E, 0x1C, 0x00 },   // U+004F (O)
    { 0x41, 0x7F, 0x7F, 0x49, 0x09, 0x0F, 0x06, 0x00 },   // U+0050 (P)
    { 0x1E, 0x3F, 0x21, 0x71, 0x7F, 0x5E, 0x00, 0x00 },   // U+0051 (Q)
    { 0x41, 0x7F, 0x7F, 0x09, 0x19, 0x7F, 0x66, 0x00 },   // U+0052 (R)
    { 0x26, 0x6F, 0x4D, 0x59, 0x73, 0x32, 0x00, 0x00 },   // U+0053 (S)
    { 0x03, 0x41, 0x7F, 0x7F, 0x41, 0x03, 0x00, 0x00 },   // U+0054 (T)
    { 0x7F, 0x7F, 0x40, 0x40, 0x7F, 0x7F, 0x00, 0x00 },   // U+0055 (U)
    { 0x1F, 0x3F, 0x60, 0x60, 0x3F, 0x1F, 0x00, 0x00 },   // U+0056 (V)
    { 0x7F, 0x7F, 0x30, 0x18, 0x30, 0x7F, 0x7F, 0x00 },   // U+0057 (W)
    { 0x43, 0x67, 0x3C, 0x18, 0x3C, 0x67, 0x43, 0x00 },   // U+0058 (X)
    { 0x07, 0x4F, 0x78, 0x78, 0x4F, 0x07, 0x00, 0x00 },   // U+0059 (Y)
    { 0x47, 0x63, 0x71, 0x59, 0x4D, 0x67, 0x73, 0x00 },   // U+005A (Z)
    { 0x00, 0x7F, 0x7F, 0x41, 0x41, 0x00, 0x00, 0x00 },   // U+005B ([)
    { 0x01, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00 },   // U+005C (\)
    { 0x00, 0x41, 0x41, 0x7F, 0x7F, 0x00, 0x00, 0x00 },   // U+005D (])
    { 0x08, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x08, 0x00 },   // U+005E (^)
    { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },   // U+005F (_)
    { 0x00, 0x00, 0x03, 0x07, 0x04, 0x00, 0x00, 0x00 },   // U+0060 (`)
    { 0x20, 0x74, 0x54, 0x54, 0x3C, 0x78, 0x40, 0x00 },   // U+0061 (a)
    { 0x41, 0x7F, 0x3F, 0x48, 0x48, 0x78, 0x30, 0x00 },   // U+0062 (b)
    { 0x38, 0x7C, 0x44, 0x44, 0x6C, 0x28, 0x00, 0x00 },   // U+0063 (c)
    { 0x30, 0x78, 0x48, 0x49, 0x3F, 0x7F, 0x40, 0x00 },   // U+0064 (d)
    { 0x38, 0x7C, 0x54, 0x54, 0x5C, 0x18, 0x00, 0x00 },   // U+0065 (e)
    { 0x48, 0x7E, 0x7F, 0x49, 0x03, 0x02, 0x00, 0x00 },   // U+0066 (f)
    { 0x98, 0xBC, 0xA4, 0xA4, 0xF8, 0x7C, 0x04, 0x00 },   // U+0067 (g)
    { 0x41, 0x7F, 0x7F, 0x08, 0x04, 0x7C, 0x78, 0x00 },   // U+0068 (h)
    { 0x00, 0x44, 0x7D, 0x7D, 0x40, 0x00, 0x00, 0x00 },   // U+0069 (i)
    { 0x60, 0xE0, 0x80, 0x80, 0xFD, 0x7D, 0x00, 0x00 },   // U+006A (j)
    { 0x41, 0x7F, 0x7F, 0x10, 0x38, 0x6C, 0x44, 0x00 },   // U+006B (k)
    { 0x00, 0x41, 0x7F, 0x7F, 0x40, 0x00, 0x00, 0x00 },   // U+006C (l)
    { 0x7C, 0x7C, 0x18, 0x38, 0x1C, 0x7C, 0x78, 0x00 },   // U+006D (m)
    { 0x7C, 0x7C, 0x04, 0x04, 0x7C, 0x78, 0x00, 0x00 },   // U+006E (n)
    { 0x38, 0x7C, 0x44, 0x44, 0x7C, 0x38, 0x00, 0x00 },   // U+006F (o)
    { 0x84, 0xFC, 0xF8, 0xA4, 0x24, 0x3C, 0x18, 0x00 },   // U+0070 (p)
    { 0x18, 0x3C, 0x24, 0xA4, 0xF8, 0xFC, 0x84, 0x00 },   // U+0071 (q)
    { 0x44, 0x7C, 0x78, 0x4C, 0x04, 0x1C, 0x18, 0x00 },   // U+0072 (r)
    { 0x48, 0x5C, 0x54, 0x54, 0x74, 0x24, 0x00, 0x00 },   // U+0073 (s)
    { 0x00, 0x04, 0x3E, 0x7F, 0x44, 0x24, 0x00, 0x00 },   // U+0074 (t)
    { 0x3C, 0x7C, 0x40, 0x40, 0x3C, 0x7C, 0x40, 0x00 },   // U+0075 (u)
    { 0x1C, 0x3C, 0x60, 0x60, 0x3C, 0x1C, 0x00, 0x00 },   // U+0076 (v)
    { 0x3C, 0x7C, 0x70, 0x38, 0x70, 0x7C, 0x3C, 0x00 },   // U+0077 (w)
    { 0x44, 0x6C, 0x38, 0x10, 0x38, 0x6C, 0x44, 0x00 },   // U+0078 (x)
    { 0x9C, 0xBC, 0xA0, 0xA0, 0xFC, 0x7C, 0x00, 0x00 },   // U+0079 (y)
    { 0x4C, 0x64, 0x74, 0x5C, 0x4C, 0x64, 0x00, 0x00 },   // U+007A (z)
    { 0x08, 0x08, 0x3E, 0x77, 0x41, 0x41, 0x00, 0x00 },   // U+007B ({)
    { 0x00, 0x00, 0x00, 0x77, 0x77, 0x00, 0x00, 0x00 },   // U+007C (|)
    { 0x41, 0x41, 0x77, 0x3E, 0x08, 0x08, 0x00, 0x00 },   // U+007D (})
    { 0x02, 0x03, 0x01, 0x03, 0x02, 0x03, 0x01, 0x00 },   // U+007E (~)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }    // U+007F
};

//TAG
static const char *TAG = "ssd1306";

typedef union out_column_t {
	uint32_t u32;
	uint8_t  u8[4];
} PACK8 out_column_t;

void ssd1306_init(i2c_master_bus_handle_t master_bus_handle, SSD1306_t *dev, uint8_t addr, int width, int height)
{
	ssd_i2c_init(master_bus_handle, dev, addr, width, height);

	// Initialize internal buffer
	for (int i=0;i<dev->_pages;i++) {
		memset(dev->_page[i]._segs, 0, 128);
	}
}

int ssd1306_get_width(SSD1306_t * dev)
{
	return dev->_width;
}

int ssd1306_get_height(SSD1306_t * dev)
{
	return dev->_height;
}

int ssd1306_get_pages(SSD1306_t * dev)
{
	return dev->_pages;
}

void ssd1306_show_buffer(SSD1306_t * dev)
{
	for (int page=0; page<dev->_pages;page++) {
		ssd_i2c_display_image(dev, page, 0, dev->_page[page]._segs, dev->_width);
	}
}

void ssd1306_set_buffer(SSD1306_t * dev, uint8_t * buffer)
{
	int index = 0;
	for (int page=0; page<dev->_pages;page++) {
		memcpy(&dev->_page[page]._segs, &buffer[index], 128);
		index = index + 128;
	}
}

void ssd1306_get_buffer(SSD1306_t * dev, uint8_t * buffer)
{
	int index = 0;
	for (int page=0; page<dev->_pages;page++) {
		memcpy(&buffer[index], &dev->_page[page]._segs, 128);
		index = index + 128;
	}
}

void ssd1306_display_image(SSD1306_t * dev, int page, int seg, uint8_t * images, int width)
{

	ssd_i2c_display_image(dev, page, seg, images, width);

	// Set to internal buffer
	memcpy(&dev->_page[page]._segs[seg], images, width);
}

void ssd1306_display_text(SSD1306_t * dev, int page, char * text, int text_len, bool invert)
{
	if (page >= dev->_pages) return;
	int _text_len = text_len;
	if (_text_len > 16) _text_len = 16;

	uint8_t seg = 0;
	uint8_t image[8];
	for (uint8_t i = 0; i < _text_len; i++) {
		memcpy(image, font8x8_basic_tr[(uint8_t)text[i]], 8);
		if (invert) ssd1306_invert(image, 8);
		if (dev->_flip) ssd1306_flip(image, 8);
		ssd1306_display_image(dev, page, seg, image, 8);
		seg = seg + 8;
	}
}

// by Coert Vonk
void ssd1306_display_text_x3(SSD1306_t * dev, int page, char * text, int text_len, bool invert)
{
	if (page >= dev->_pages) return;
	int _text_len = text_len;
	if (_text_len > 5) _text_len = 5;

	uint8_t seg = 0;

	for (uint8_t nn = 0; nn < _text_len; nn++) {

		uint8_t const * const in_columns = font8x8_basic_tr[(uint8_t)text[nn]];

		// make the character 3x as high
		out_column_t out_columns[8];
		memset(out_columns, 0, sizeof(out_columns));

		for (uint8_t xx = 0; xx < 8; xx++) { // for each column (x-direction)

			uint32_t in_bitmask = 0b1;
			uint32_t out_bitmask = 0b111;

			for (uint8_t yy = 0; yy < 8; yy++) { // for pixel (y-direction)
				if (in_columns[xx] & in_bitmask) {
					out_columns[xx].u32 |= out_bitmask;
				}
				in_bitmask <<= 1;
				out_bitmask <<= 3;
			}
		}

		// render character in 8 column high pieces, making them 3x as wide
		for (uint8_t yy = 0; yy < 3; yy++)	{ // for each group of 8 pixels high (y-direction)

			uint8_t image[24];
			for (uint8_t xx = 0; xx < 8; xx++) { // for each column (x-direction)
				image[xx*3+0] =
				image[xx*3+1] =
				image[xx*3+2] = out_columns[xx].u8[yy];
			}
			if (invert) ssd1306_invert(image, 24);
			if (dev->_flip) ssd1306_flip(image, 24);

		    ssd_i2c_display_image(dev, page+yy, seg, image, 24);
			memcpy(&dev->_page[page+yy]._segs[seg], image, 24);
		}
		seg = seg + 24;
	}
}

void ssd1306_clear_screen(SSD1306_t * dev, bool invert)
{
	char space[16];
	memset(space, 0x00, sizeof(space));
	for (int page = 0; page < dev->_pages; page++) {
		ssd1306_display_text(dev, page, space, sizeof(space), invert);
	}
}

void ssd1306_clear_line(SSD1306_t * dev, int page, bool invert)
{
	char space[16];
	memset(space, 0x00, sizeof(space));
	ssd1306_display_text(dev, page, space, sizeof(space), invert);
}

void ssd1306_contrast(SSD1306_t * dev, int contrast)
{
	ssd_i2c_contrast(dev, contrast);
}

void ssd1306_software_scroll(SSD1306_t * dev, int start, int end)
{
	ESP_LOGD(TAG, "software_scroll start=%d end=%d _pages=%d", start, end, dev->_pages);
	if (start < 0 || end < 0) {
		dev->_scEnable = false;
	} else if (start >= dev->_pages || end >= dev->_pages) {
		dev->_scEnable = false;
	} else {
		dev->_scEnable = true;
		dev->_scStart = start;
		dev->_scEnd = end;
		dev->_scDirection = 1;
		if (start > end ) dev->_scDirection = -1;
	}
}


void ssd1306_scroll_text(SSD1306_t * dev, char * text, int text_len, bool invert)
{
	ESP_LOGD(TAG, "dev->_scEnable=%d", dev->_scEnable);
	if (dev->_scEnable == false) return;

	esp_err_t (*func)(SSD1306_t *dev, int page, int seg, uint8_t * images, int width);
	func = ssd_i2c_display_image;

	int srcIndex = dev->_scEnd - dev->_scDirection;
	while(1) {
		int dstIndex = srcIndex + dev->_scDirection;
		ESP_LOGD(TAG, "srcIndex=%d dstIndex=%d", srcIndex,dstIndex);
		for(int seg = 0; seg < dev->_width; seg++) {
			dev->_page[dstIndex]._segs[seg] = dev->_page[srcIndex]._segs[seg];
		}
		(*func)(dev, dstIndex, 0, dev->_page[dstIndex]._segs, sizeof(dev->_page[dstIndex]._segs));
		if (srcIndex == dev->_scStart) break;
		srcIndex = srcIndex - dev->_scDirection;
	}

	int _text_len = text_len;
	if (_text_len > 16) _text_len = 16;

	ssd1306_display_text(dev, srcIndex, text, text_len, invert);
}

void ssd1306_scroll_clear(SSD1306_t * dev)
{
	ESP_LOGD(TAG, "dev->_scEnable=%d", dev->_scEnable);
	if (dev->_scEnable == false) return;

	int srcIndex = dev->_scEnd - dev->_scDirection;
	while(1) {
		int dstIndex = srcIndex + dev->_scDirection;
		ESP_LOGD(TAG, "srcIndex=%d dstIndex=%d", srcIndex,dstIndex);
		ssd1306_clear_line(dev, dstIndex, false);
		if (dstIndex == dev->_scStart) break;
		srcIndex = srcIndex - dev->_scDirection;
	}
}


void ssd1306_hardware_scroll(SSD1306_t * dev, ssd1306_scroll_type_t scroll)
{
	ssd_i2c_hardware_scroll(dev, scroll);
}

// delay = 0 : display with no wait
// delay > 0 : display with wait
// delay < 0 : no display
void ssd1306_wrap_arround(SSD1306_t * dev, ssd1306_scroll_type_t scroll, int start, int end, int8_t delay)
{
	if (scroll == SCROLL_RIGHT) {
		int _start = start; // 0 to 7
		int _end = end; // 0 to 7
		if (_end >= dev->_pages) _end = dev->_pages - 1;
		uint8_t wk;
		//for (int page=0;page<dev->_pages;page++) {
		for (int page=_start;page<=_end;page++) {
			wk = dev->_page[page]._segs[127];
			for (int seg=127;seg>0;seg--) {
				dev->_page[page]._segs[seg] = dev->_page[page]._segs[seg-1];
			}
			dev->_page[page]._segs[0] = wk;
		}

	} else if (scroll == SCROLL_LEFT) {
		int _start = start; // 0 to 7
		int _end = end; // 0 to 7
		if (_end >= dev->_pages) _end = dev->_pages - 1;
		uint8_t wk;
		//for (int page=0;page<dev->_pages;page++) {
		for (int page=_start;page<=_end;page++) {
			wk = dev->_page[page]._segs[0];
			for (int seg=0;seg<127;seg++) {
				dev->_page[page]._segs[seg] = dev->_page[page]._segs[seg+1];
			}
			dev->_page[page]._segs[127] = wk;
		}

	} else if (scroll == SCROLL_UP) {
		int _start = start; // 0 to {width-1}
		int _end = end; // 0 to {width-1}
		if (_end >= dev->_width) _end = dev->_width - 1;
		uint8_t wk0;
		uint8_t wk1;
		uint8_t wk2;
		uint8_t save[128];
		// Save pages 0
		for (int seg=0;seg<128;seg++) {
			save[seg] = dev->_page[0]._segs[seg];
		}
		// Page0 to Page6
		for (int page=0;page<dev->_pages-1;page++) {
			//for (int seg=0;seg<128;seg++) {
			for (int seg=_start;seg<=_end;seg++) {
				wk0 = dev->_page[page]._segs[seg];
				wk1 = dev->_page[page+1]._segs[seg];
				if (dev->_flip) wk0 = ssd1306_rotate_byte(wk0);
				if (dev->_flip) wk1 = ssd1306_rotate_byte(wk1);
				if (seg == 0) {
					ESP_LOGD(TAG, "b page=%d wk0=%02x wk1=%02x", page, wk0, wk1);
				}
				wk0 = wk0 >> 1;
				wk1 = wk1 & 0x01;
				wk1 = wk1 << 7;
				wk2 = wk0 | wk1;
				if (seg == 0) {
					ESP_LOGD(TAG, "a page=%d wk0=%02x wk1=%02x wk2=%02x", page, wk0, wk1, wk2);
				}
				if (dev->_flip) wk2 = ssd1306_rotate_byte(wk2);
				dev->_page[page]._segs[seg] = wk2;
			}
		}
		// Page7
		int pages = dev->_pages-1;
		//for (int seg=0;seg<128;seg++) {
		for (int seg=_start;seg<=_end;seg++) {
			wk0 = dev->_page[pages]._segs[seg];
			wk1 = save[seg];
			if (dev->_flip) wk0 = ssd1306_rotate_byte(wk0);
			if (dev->_flip) wk1 = ssd1306_rotate_byte(wk1);
			wk0 = wk0 >> 1;
			wk1 = wk1 & 0x01;
			wk1 = wk1 << 7;
			wk2 = wk0 | wk1;
			if (dev->_flip) wk2 = ssd1306_rotate_byte(wk2);
			dev->_page[pages]._segs[seg] = wk2;
		}

	} else if (scroll == SCROLL_DOWN) {
		int _start = start; // 0 to {width-1}
		int _end = end; // 0 to {width-1}
		if (_end >= dev->_width) _end = dev->_width - 1;
		uint8_t wk0;
		uint8_t wk1;
		uint8_t wk2;
		uint8_t save[128];
		// Save pages 7
		int pages = dev->_pages-1;
		for (int seg=0;seg<128;seg++) {
			save[seg] = dev->_page[pages]._segs[seg];
		}
		// Page7 to Page1
		for (int page=pages;page>0;page--) {
			//for (int seg=0;seg<128;seg++) {
			for (int seg=_start;seg<=_end;seg++) {
				wk0 = dev->_page[page]._segs[seg];
				wk1 = dev->_page[page-1]._segs[seg];
				if (dev->_flip) wk0 = ssd1306_rotate_byte(wk0);
				if (dev->_flip) wk1 = ssd1306_rotate_byte(wk1);
				if (seg == 0) {
					ESP_LOGD(TAG, "b page=%d wk0=%02x wk1=%02x", page, wk0, wk1);
				}
				wk0 = wk0 << 1;
				wk1 = wk1 & 0x80;
				wk1 = wk1 >> 7;
				wk2 = wk0 | wk1;
				if (seg == 0) {
					ESP_LOGD(TAG, "a page=%d wk0=%02x wk1=%02x wk2=%02x", page, wk0, wk1, wk2);
				}
				if (dev->_flip) wk2 = ssd1306_rotate_byte(wk2);
				dev->_page[page]._segs[seg] = wk2;
			}
		}
		// Page0
		//for (int seg=0;seg<128;seg++) {
		for (int seg=_start;seg<=_end;seg++) {
			wk0 = dev->_page[0]._segs[seg];
			wk1 = save[seg];
			if (dev->_flip) wk0 = ssd1306_rotate_byte(wk0);
			if (dev->_flip) wk1 = ssd1306_rotate_byte(wk1);
			wk0 = wk0 << 1;
			wk1 = wk1 & 0x80;
			wk1 = wk1 >> 7;
			wk2 = wk0 | wk1;
			if (dev->_flip) wk2 = ssd1306_rotate_byte(wk2);
			dev->_page[0]._segs[seg] = wk2;
		}

	}

	if (delay >= 0) {
		for (int page=0;page<dev->_pages;page++) {

			ssd_i2c_display_image(dev, page, 0, dev->_page[page]._segs, 128);
			if (delay) vTaskDelay(delay);
		}
	}

}

void ssd1306_bitmaps(SSD1306_t *dev, int xpos, int ypos, uint8_t * bitmap, int width, int height, bool invert)
{
	if ( (width % 8) != 0) {
		ESP_LOGE(TAG, "width must be a multiple of 8");
		return;
	}
	int _width = width / 8;
	uint8_t wk0;
	uint8_t wk1;
	uint8_t wk2;
	uint8_t page = (ypos / 8);
	uint8_t _seg = xpos;
	uint8_t dstBits = (ypos % 8);
	ESP_LOGD(TAG, "ypos=%d page=%d dstBits=%d", ypos, page, dstBits);
	int offset = 0;
	for(int _height=0;_height<height;_height++) {
		for (int index=0;index<_width;index++) {
			for (int srcBits=7; srcBits>=0; srcBits--) {
				wk0 = dev->_page[page]._segs[_seg];
				if (dev->_flip) wk0 = ssd1306_rotate_byte(wk0);

				wk1 = bitmap[index+offset];
				if (invert) wk1 = ~wk1;

				//wk2 = ssd1306_copy_bit(bitmap[index+offset], srcBits, wk0, dstBits);
				wk2 = ssd1306_copy_bit(wk1, srcBits, wk0, dstBits);
				if (dev->_flip) wk2 = ssd1306_rotate_byte(wk2);

				ESP_LOGD(TAG, "index=%d offset=%d page=%d _seg=%d, wk2=%02x", index, offset, page, _seg, wk2);
				dev->_page[page]._segs[_seg] = wk2;
				_seg++;
			}
		}
		vTaskDelay(1);
		offset = offset + _width;
		dstBits++;
		_seg = xpos;
		if (dstBits == 8) {
			page++;
			dstBits=0;
		}
	}

	ssd1306_show_buffer(dev);
}


// Set pixel to internal buffer. Not show it.
void _ssd1306_pixel(SSD1306_t * dev, int xpos, int ypos, bool invert)
{
	uint8_t _page = (ypos / 8);
	uint8_t _bits = (ypos % 8);
	uint8_t _seg = xpos;
	uint8_t wk0 = dev->_page[_page]._segs[_seg];
	uint8_t wk1 = 1 << _bits;
	ESP_LOGD(TAG, "ypos=%d _page=%d _bits=%d wk0=0x%02x wk1=0x%02x", ypos, _page, _bits, wk0, wk1);
	if (invert) {
		wk0 = wk0 & ~wk1;
	} else {
		wk0 = wk0 | wk1;
	}
	if (dev->_flip) wk0 = ssd1306_rotate_byte(wk0);
	ESP_LOGD(TAG, "wk0=0x%02x wk1=0x%02x", wk0, wk1);
	dev->_page[_page]._segs[_seg] = wk0;
}

// Set line to internal buffer. Not show it.
void _ssd1306_line(SSD1306_t * dev, int x1, int y1, int x2, int y2,  bool invert)
{
	int i;
	int dx,dy;
	int sx,sy;
	int E;

	/* distance between two points */
	dx = ( x2 > x1 ) ? x2 - x1 : x1 - x2;
	dy = ( y2 > y1 ) ? y2 - y1 : y1 - y2;

	/* direction of two point */
	sx = ( x2 > x1 ) ? 1 : -1;
	sy = ( y2 > y1 ) ? 1 : -1;

	/* inclination < 1 */
	if ( dx > dy ) {
		E = -dx;
		for ( i = 0 ; i <= dx ; i++ ) {
			_ssd1306_pixel(dev, x1, y1, invert);
			x1 += sx;
			E += 2 * dy;
			if ( E >= 0 ) {
			y1 += sy;
			E -= 2 * dx;
		}
	}

	/* inclination >= 1 */
	} else {
		E = -dy;
		for ( i = 0 ; i <= dy ; i++ ) {
			_ssd1306_pixel(dev, x1, y1, invert);
			y1 += sy;
			E += 2 * dx;
			if ( E >= 0 ) {
				x1 += sx;
				E -= 2 * dy;
			}
		}
	}
}

void ssd1306_invert(uint8_t *buf, size_t blen)
{
	uint8_t wk;
	for(int i=0; i<blen; i++){
		wk = buf[i];
		buf[i] = ~wk;
	}
}

// Flip upside down
void ssd1306_flip(uint8_t *buf, size_t blen)
{
	for(int i=0; i<blen; i++){
		buf[i] = ssd1306_rotate_byte(buf[i]);
	}
}

uint8_t ssd1306_copy_bit(uint8_t src, int srcBits, uint8_t dst, int dstBits)
{
	ESP_LOGD(TAG, "src=%02x srcBits=%d dst=%02x dstBits=%d", src, srcBits, dst, dstBits);
	uint8_t smask = 0x01 << srcBits;
	uint8_t dmask = 0x01 << dstBits;
	uint8_t _src = src & smask;

	uint8_t _dst;
	if (_src != 0) {
		_dst = dst | dmask; // set bit
	} else {
		_dst = dst & ~(dmask); // clear bit
	}
	return _dst;
}


// Rotate 8-bit data
// 0x12-->0x48
uint8_t ssd1306_rotate_byte(uint8_t ch1) {
	uint8_t ch2 = 0;
	for (int j=0;j<8;j++) {
		ch2 = (ch2 << 1) + (ch1 & 0x01);
		ch1 = ch1 >> 1;
	}
	return ch2;
}


void ssd1306_fadeout(SSD1306_t * dev)
{
	esp_err_t (*func)(SSD1306_t * dev, int page, int seg, uint8_t * images, int width);
	func = ssd_i2c_display_image;

	uint8_t image[1];
	for(int page=0; page<dev->_pages; page++) {
		image[0] = 0xFF;
		for(int line=0; line<8; line++) {
			if (dev->_flip) {
				image[0] = image[0] >> 1;
			} else {
				image[0] = image[0] << 1;
			}
			for(int seg=0; seg<128; seg++) {
				(*func)(dev, page, seg, image, 1);
				dev->_page[page]._segs[seg] = image[0];
			}
		}
	}
}

void ssd1306_dump(SSD1306_t dev)
{
	printf("_width=%x\n",dev._width);
	printf("_height=%x\n",dev._height);
	printf("_pages=%x\n",dev._pages);
}

void ssd1306_dump_page(SSD1306_t * dev, int page, int seg)
{
	ESP_LOGI(TAG, "dev->_page[%d]._segs[%d]=%02x", page, seg, dev->_page[page]._segs[seg]);
}

