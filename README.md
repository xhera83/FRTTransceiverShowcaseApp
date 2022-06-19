# FRTTransceiverShowcaseAppl-

ESP32 Applikation 1 und 2 kombiniert.

Kann mit platformio geöffnet, gebaut und geflasht werden: (https://github.com/xhera83/FreeRTOS-Transceiver/tree/main/examples/examples-esp32ArduinoFramework)

Eventuell muss man platform.io anpassen.

In FRTTransceiverShowcaseAppl-/lib/ShowcaseWebserver/ShowcaseWebserver.cpp ganz oben ssidAP und passAP ausfüllen, um den ESP32 mit dem persönlichen Router zu verbinden.
Um 'Summary' für jeden Task sehen zu können, muss man in FRTTransceiverSettings.h 'FRTTRANSCEIVER_ANALYTICS_ENABLE' anschalten, nachdem PlatformIO die Bibliothek installiert hat.



