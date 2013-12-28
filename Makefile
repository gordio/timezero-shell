TZ_DIR   := "${HOME}/.timezero"
PROJNAME := $(notdir $(PWD))
SOURCES  := $(wildcard src/*.c)
SOURCES  += $(wildcard src/*/*.c)
OBJECTS  := $(SOURCES:.c=.o)

#_ver   := $(shell git shortlog | grep -E '^[ ]+\w+' | wc -l)
_ver0   := $(shell git describe HEAD 2>/dev/null)
_ver1   := $(shell git describe --tags HEAD 2>/dev/null)
_ver2   := $(shell git rev-parse --verify --short HEAD 2>/dev/null)
VERSION := $(or $(_ver0),$(_ver1),$(_ver2))

CFLAGS  += `pkg-config --cflags gtk+-2.0 webkit-1.0 json-c` -std=c99
CFLAGS  += -Isrc
CFLAGS  += -g -Wall -Wextra  -pedantic
CFLAGS  += -Wno-unused-function -Wno-unused-parameter
CFLAGS  += -DENABLE_CLI_COLORS -DENABLE_NLS
CFLAGS  += -DVERSION=\"$(VERSION)\"
LDFLAGS += `pkg-config --libs gtk+-2.0 webkit-1.0 json-c`
ASTYLE_ARGS += --style=kr --indent=tab --suffix=none


.SUFFIXES: .c .h .o
.PHONY: all clear clean tags

all: $(PROJNAME)

$(PROJNAME): $(OBJECTS)
	@echo -e "\e[1;32m LINK\e[m" $(PROJNAME)
	@$(CC) $(OBJECTS) $(LDFLAGS) -o "$(PROJNAME)"

%.o: %.c %.h
	@echo -e "\e[1m   CC\e[m" $<
	@$(CC) -c $< $(CFLAGS) -o $@

indent:
	@astyle $(ASTYLE_ARGS) $(SOURCES)

clear:
	@echo -e "\e[1;31m   RM\e[m" $(OBJECTS)
	@rm -f $(OBJECTS)

clean: clear
	@echo -e "\e[1;31m   RM\e[m" "$(PROJNAME)"
	@rm -f "$(PROJNAME)"

install: $(PROJNAME)
	@echo -e "\e[1m > Copy resources\e[m"
	@echo -n " - Make directory..."
	@mkdir -p ${TZ_DIR} && echo " done."
	@echo -n " - Copy images..."
	@cp -r resources/img ${TZ_DIR} && echo " done."
	@echo -n " - Copy sounds..."
	@cp resources/*.wav ${TZ_DIR} && echo " done."
	@echo -n " - Copy style..."
	@cp resources/gtkrc ${TZ_DIR} && echo " done."
	@echo
	@# Приблизительный размер скачиваемый/устанавливаемый 130МБ
	@echo -e "\e[1m > Install/update TimeZero Client\e[m"
	@echo -n " - Download update script...";
	@cd ${TZ_DIR};wget -q http://game.timezero.ru/tzupdate3.pl -O update && echo " done."
	@echo -n " - Fix update script problems"
	@cd ${TZ_DIR};sed -i 's/die "Wrong client/#&/' update && echo " done."
	@echo " - Start updater script...";
	@cd ${TZ_DIR};chmod u+x update; ./update || echo "Some errors detected."
	@echo -e "\e[1m * Finished\e[m"
