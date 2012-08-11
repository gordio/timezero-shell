#!/bin/bash

TZ_DIR="~/.timezero"

echo "Copy resources files…"
cp -r resources/img $TZ_DIR
cp resources/private.wav $TZ_DIR
cp resources/to.wav $TZ_DIR

echo "Install theme"
cp -r resources/TimeZero ~/.themes/


# Приблизительный размер скачиваемый/устанавливаемый 130МБ
# Создаем директорию для клиента и переходим в нее
mkdir ~/.timezero
cd ~/.timezero

# Скачиваем скрипт для обновления и запускаем
wget -q http://ompldr.org/vZHAwdw/update -O update
chmod u+x update
./update
