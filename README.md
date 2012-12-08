TimeZero Shell
==============
GTK оболочка для игры TimeZero. Первоначально разрабатываемая для Linux.

**Лицензия**: MIT на исходный код, и BY-NC-SA на графические файлы в директории `resources`

**Автор**: Гордиенко Олег

**Состояние**: В разработке (редко уделяется внимание)

**Версия**: 0.1-dev


![Screenshot: TimeZero Shell on Linux](http://ompldr.org/vZ2x5OQ)


Сборка и настройка
==================
Для сборки программы вам необнохимо наличие следующих библиотек (dev версий для Debian)

 - GTK2 - 2.16+ (dev on 2.24.12)
 - WebKit-gtk - 1.6+ (dev on 1.8.3-r200)
 - json-c
 - Adobe Flash - 9+ (dev on 11.2.202.251)


Установка и сборка
------------------
Все нижеперечисленные команды выполняются от обычного пользователя.

	make
	make install


Настройка разрешений для Flash клиента
--------------------------------------
Перейдите [к настройке флеша](http://www.macromedia.com/support/documentation/en/flashplayer/help/settings_manager04.html) и добавьте директорию /home/<имя пользователя>/.timezero/ в список разрешенных зон.

> Always trust files in these locations: => Edit locations... => Add location...


FAQ или Часто задаваемые вопросы
--------------------------------

> Где я могу посмотреть список планов и их выполнение?

В файде `TASKS`, а так же в исходном коде по меткам TODO, FIXME, NOTE

> Как уведомить о баге, или предложить улучшение?

Воспользуйтесь встроеной функцией Github'а [Issues](http://code.gordio.pp.ua/timezero-shell/issues?status=new&status=open).

> Иногда клиент сразу при запуске закрывается, почему?

Это известный баг, но отследить и исправить пока не удается.

> Почему иногда перестает открыватся инвентарь и показывается неверное время перехода?

Подобное поведение наблюдается и в браузерах, поэтому сделан вывод: проблема в Flash'е или WebKit'e.

> Мне не нравится ваш код, и сделайте эту штуку вот так.

Пожалуйста, уважаемые. Я принимаю патчи, пулы, и предложения в Issues.
