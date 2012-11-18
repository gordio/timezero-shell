#!/usr/bin/make -f

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

CFLAGS  += `pkg-config --cflags gtk+-2.0 webkit-1.0 json` -std=c99
CFLAGS  += -g -Wall -Wextra  -pedantic
CFLAGS  += -Wno-unused-function -Wno-unused-parameter
CFLAGS  += -DENABLE_CLI_COLORS -DENABLE_NLS
CFLAGS  += -DVERSION=\"$(VERSION)\"
LDFLAGS += `pkg-config --libs gtk+-2.0 webkit-1.0 json`


.SUFFIXES: .c .h .o
.PHONY: all clear clean tags

all: $(PROJNAME)

$(PROJNAME): $(OBJECTS)
	@echo -e "\e[1;32m LINK\e[m" $(PROJNAME)
	@$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o "$(PROJNAME)"

%.o: %.c %.h
	@echo -e "\e[1m   CC\e[m" $<
	@$(CC) $(CFLAGS) -c -o $@ $<

clear:
	@echo -e "\e[1;31m   RM\e[m" $(OBJECTS)
	@rm -f $(OBJECTS)

clean: clear
	@echo -e "\e[1;31m   RM\e[m" "$(PROJNAME)"
	@rm -f "$(PROJNAME)"

install: $(PROJNAME)
	@echo -e "\e[1m > Copy resources\e[m"
	@echo -n "Make directory..."
	@mkdir -p ${TZ_DIR} && echo " done."
	@echo -n "Copy images..."
	@cp -r resources/img ${TZ_DIR} && echo " done."
	@echo -n "Copy sounds..."
	@cp resources/*.wav ${TZ_DIR} && echo " done."
	@echo -n "Copy style..."
	@cp resources/gtkrc ${TZ_DIR} && echo " done."
	@echo
	@# Приблизительный размер скачиваемый/устанавливаемый 130МБ
	@echo -e "\e[1m > Install/update TimeZero Client\e[m"
	@echo "Download update script...";
	@cd ${TZ_DIR};wget -q http://ompldr.org/vZHAwdw/update -O update
	@echo "Start updater script...";
	@cd ${TZ_DIR};chmod u+x update; ./update || echo "Some errors detected."
	@echo -e "\e[1m * Finished\e[m"
