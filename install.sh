#!/bin/bash

TZ_DIR="~/.timezero"

echo "Copy data files…"
cp -r data/img $TZ_DIR
cp data/private.wav $TZ_DIR
cp data/to.wav $TZ_DIR

echo "Install theme"
cp -r data/TimeZero ~/.themes/


# Приблизительный размер скачиваемый/устанавливаемый 130МБ
# Создаем директорию для клиента и переходим в нее
mkdir ~/.timezero
cd ~/.timezero

# Скачиваем скрипт для обновления и запускаем
wget -q http://ompldr.org/vZHAwdw/update -O update
chmod u+x update
./update
