.PHONY: all fullclean clean build

build:
	pio run

all: fullclean build

upload:
	pio run -t upload

monitor:
	pio run -t monitor

clean:
	rm -rf .pio/ managed_components/ .dummy/  CMakeLists.txt
	pio run -t clean

fullclean: clean
	rm -rf .pio/ managed_components/ .dummy/  CMakeLists.txt
	rm -f $(HOME)/.platformio/idf-env.json
	rm -rf $(HOME)/.platformio/packages/framework-arduinoespressif32
	rm -rf $(HOME)/.platformio/packages/framework-arduinoespressif32-libs
	rm -rf $(HOME)/.platformio/platforms/espressif32

