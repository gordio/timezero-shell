#!/bin/bash

TZ_DIR="~/.timezero"

echo " Copy resources"
cp -r resources/img $TZ_DIR
cp resources/private.wav $TZ_DIR
cp resources/to.wav $TZ_DIR

echo "Install resources"
cp resources/gtkrc $TZ_DIR


# Приблизительный размер скачиваемый/устанавливаемый 130МБ
# Создаем директорию для клиента и переходим в нее
mkdir $TZ_DIR
cd $TZ_DIR

# Скачиваем скрипт для обновления и запускаем
echo "Install/update TimeZero Client"
wget -q http://ompldr.org/vZHAwdw/update -O update
chmod u+x update
./update
