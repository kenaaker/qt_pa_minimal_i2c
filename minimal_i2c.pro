TARGET = qminimal_i2c

PLUGIN_TYPE = platforms
PLUGIN_CLASS_NAME = QMinimal_i2cIntegrationPlugin
#load(qt_plugin)

TEMPLATE = lib
CONFIG += plugin
CONFIG += qpa/genericunixfontdatabase


QT += core-private gui-private platformsupport-private

SOURCES =   main.cpp \
            qminimal_i2cintegration.cpp \
            ssd1306.cpp \
            qminimal_i2cbackingstore.cpp

HEADERS =   qminimal_i2cintegration.h \
            ssd1306.h \
            i2c-dev.lm-sensors.h \
            qminimal_i2cbackingstore.h

OTHER_FILES += minimal_i2c.json

target.path += /home/kdaaker/test_code/platforms
INSTALLS += target
