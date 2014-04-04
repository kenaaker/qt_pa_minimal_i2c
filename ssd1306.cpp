
//#include <linux/i2c-dev.h>
#include "i2c-dev.lm-sensors.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <QByteArray>
#include <QtCore/qdebug.h>
#include <QImage>
#include <QColor>
#include "ssd1306.h"

#define SSD1306_I2C_ADDRESS   0x3D	// 011110+SA0+RW

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDRESS 0x21
#define SSD1306_PAGEADDRESS 0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

void swap(int &x1, int &x2)
{
    int x;
    
    x = x1;
    x1 = x2;
    x2 = x;
}

SSD1306::SSD1306(unsigned char address, SSD1306_Type type)
{
    
    _address = address;
    _type = type;
    _i2cFileHandler = 0;
}

SSD1306::~SSD1306()
{
}

int SSD1306::openDevice(const char *bus)
{
	_i2cFileHandler = open(bus, O_RDWR);
	if (_i2cFileHandler <= 0) {
	    printf("problem to open the bus\n");
	    return 1;
	}
	
	if (ioctl(_i2cFileHandler, I2C_SLAVE, _address) != 0) {
	    printf("problem to set the slave address\n");
	    return 1;
	}
	return 0;
}

void SSD1306::closeDevice(void)
{
    this->sendCommand(SSD1306_DISPLAYOFF);                    // 0xAE
    close(_i2cFileHandler);
}

void SSD1306::sendCommand(unsigned char command)
{
    unsigned char bufferCommand[2] = { 0x00, command };
    
    if (write(_i2cFileHandler, bufferCommand, sizeof(bufferCommand)) != sizeof(bufferCommand)) {
        printf("problem with %d\n", command);
    }
}

int SSD1306::initDevice(void)
{
    int vccstate = SSD1306_SWITCHCAPVCC;
    
    // Init sequence for 128x64 OLED module
    this->sendCommand(SSD1306_DISPLAYOFF);                    // 0xAE
    this->sendCommand(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    this->sendCommand(0x80);                                  // the suggested ratio 0x80
    this->sendCommand(SSD1306_SETMULTIPLEX);                  // 0xA8
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x3F);
            break;
        case SSD1306_128_32:
            this->sendCommand(0x1F);
            break;
        case SSD1306_96_16:
            this->sendCommand(0x0F);
            break;
    }
    this->sendCommand(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    this->sendCommand(0x0);                                   // no offset
    if (_type == SSD1306_96_16) {
        this->sendCommand(SSD1306_SETSTARTLINE | 0x08);            // line #0
    } else {
        this->sendCommand(SSD1306_SETSTARTLINE | 0x0);            // line #0
    }
    this->sendCommand(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) {
        this->sendCommand(0x10);
    } else {
        this->sendCommand(0x14);
    }
    this->sendCommand(SSD1306_MEMORYMODE);                    // 0x20
    this->sendCommand(0x00);                                  // 0x0 act like ks0108
    this->sendCommand(SSD1306_COLUMNADDRESS);                 // 0x21
    this->sendCommand(0x00);                                  // start column
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x7F);                                  // end column 127
            break;
        case SSD1306_128_32:
            this->sendCommand(0x7F);                                  // end column 127
            break;
        case SSD1306_96_16:
            this->sendCommand(95);                                  // end column 95
            break;
    }
    this->sendCommand(SSD1306_PAGEADDRESS);                   // 0x22
    this->sendCommand(0x00);                                  // start page
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x07);                                  // end page 7
            break;
        case SSD1306_128_32:
            this->sendCommand(0x07);                                  // end page 7
            break;
        case SSD1306_96_16:
            this->sendCommand(0x01);                                  // end page 1
            break;
    }
    this->sendCommand(SSD1306_SEGREMAP | 0x1);
//    this->sendCommand(SSD1306_COMSCANDEC);
    this->sendCommand(SSD1306_COMSCANINC);
    this->sendCommand(SSD1306_SETCOMPINS);                    // 0xDA
    switch (_type) {
        case SSD1306_128_64:
            this->sendCommand(0x12);
            break;
        case SSD1306_128_32:
            this->sendCommand(0x02);
            break;
        case SSD1306_96_16:
            this->sendCommand(0x02);
            break;
    }
    this->sendCommand(SSD1306_SETCONTRAST);                   // 0x81
    switch (_type) {
        case SSD1306_128_64:
            if (vccstate == SSD1306_EXTERNALVCC) {
                this->sendCommand(0x9F);
            } else {
                this->sendCommand(0xCF);
            }
            break;
        case SSD1306_128_32:
            this->sendCommand(0x8F);
            break;
        case SSD1306_96_16:
            if (vccstate == SSD1306_EXTERNALVCC) {
                this->sendCommand(0x10);
            } else {
                this->sendCommand(0xAF);
            }
            break;
    }
    this->sendCommand(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) {
        this->sendCommand(0x22);
    } else {
        this->sendCommand(0xF1);
    }
    this->sendCommand(SSD1306_SETVCOMDETECT);                 // 0xDB
    this->sendCommand(0x40);
    this->sendCommand(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    this->sendCommand(SSD1306_NORMALDISPLAY);                 // 0xA6
    this->sendCommand(SSD1306_DISPLAYON);                     // 0xAF
    return 0;
}

void SSD1306::pushDisplay(const QImage &scr)
{
    unsigned short s_width = scr.width();
    unsigned short s_height = scr.height();
    QByteArray page;
    unsigned char accum_byte;
    int x;
    int y;
    int y_page;
    QRgb my_black = qRgb(0,0,0);

    this->sendCommand(SSD1306_SETLOWCOLUMN | 0x0);	// low col = 0
    this->sendCommand(SSD1306_SETHIGHCOLUMN | 0x0);	// hi col = 0
    this->sendCommand(SSD1306_SETSTARTLINE | 0x0);	// line #0
     // send a scan line in one write
    for (y_page = 0; y_page < s_height; y_page += 8) {
	page = QByteArray();
	for (x = 0; x < s_width; ++x) {
	    accum_byte = 0;
	    for (y = y_page; y < (y_page+8); ++y) {
		QRgb this_pixel = scr.pixel(x,y);
		if (this_pixel != my_black) {
		    accum_byte |= 1 << y%8;
		} /* endif */
	    } /* endfor */
	    page.prepend(accum_byte);
	} /* endfor */
	page.prepend(0x40);
	//qDebug() << "y_page" << y_page << "page size" << page.size() << " bits" << page.toHex();
        if (write(_i2cFileHandler, page.constData(), page.size()) != page.size()) {
            printf("problem with page %d\n", y_page);
        } /* endif */
    } /* endfor */
}

unsigned short SSD1306::getWidth()
{
    unsigned short width = 0;
    
    switch(_type) {
        case SSD1306_128_64:
            width = 128;
            break;
        case SSD1306_128_32:
            width = 128;
            break;
        case SSD1306_96_16:
            width = 96;
            break;
    }
    return width;
}

unsigned short SSD1306::getHeight()
{
    unsigned short height = 0;
    
    switch(_type) {
        case SSD1306_128_64:
            height = 64;
            break;
        case SSD1306_128_32:
            height = 32;
            break;
        case SSD1306_96_16:
            height = 16;
            break;
    }
    return height;
}
