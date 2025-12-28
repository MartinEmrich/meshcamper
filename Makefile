.PHONY: all fullclean clean build

build:
	pio run

all: fullclean build

upload:
	pio run -t upload

monitor:
	pio run -t monitor

clean:
	rm -rf .pio/ managed_components/ .dummy/  CMakeLists.txt sdkconfig.*
	pio run -t clean

fullclean:
	rm -f $(HOME)/.platformio/idf-env.json
	rm -f $(HOME)/.platformio/packages/framework-arduinoespressif32
	rm -f $(HOME)/.platformio/packages/framework-arduinoespressif32-libs
	rm -f $(HOME)/.platformio/platforms/espressif32

