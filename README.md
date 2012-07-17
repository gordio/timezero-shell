Описание
========
Оболочка для игры TimeZero.

__ Внимание! Находится в разработке! __

** Переписать все! **


Установка и запуск
==================
Зависит от библиотек:

 - GTK2
 - WebKit 1.4+ (рекомендуется 1.6)
 - json-c
 - Запуск:

        # Приблизительный размер скачиваемый/устанавливаемый 130МБ
        md ~/.timezero
        cd ~/.timezero
        wget -q http://ompldr.org/vZHAwdw/update
        chmod u+x update
        ./update

        # Сборка оболочки
        git clone http://github.com/gordio/timezero-shell.git
        cd timezero-shell
        ./waf configure
        ./waf
        build/timezero-shell

