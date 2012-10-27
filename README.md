Оболочка для игры TimeZero.

**Внимание: Находится в разработке!**

![Screenshot TimeZero Shell on Linux](http://ompldr.org/vZXRldA/timezero-shell.jpg)


Сборка и настройка
==================
Для сборки программы вам необнохимо наличие следующих библиотек (dev версий для Debian)

 - GTK2 (minimum 2.16)
 - WebKit 1.4+ (best 1.6)
 - json-c
 - Adobe Flash


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

Воспользуйтесь встроеной функцией Github'а [Issues](https://github.com/gordio/timezero-shell/issues).


> Иногда клиент сразу при запуске закрывается, почему?

Это известный баг, но отследить и исправить пока не удается.


> Почему иногда перестает открыватся инвентарь и показывается неверное время перехода?

Подобное поведение наблюдается и в браузерах, поэтому сделан вывод: проблема в Flash'е или WebKit'e.


> Мне не нравится ваш код, и сделайте эту штуку вот так.

Пожалуйста, уважаемые. Я принимаю патчи, пулы, и предложения в Issues.



ЛИЦЕНЗИЯ — CC BY-NC-SA 3.0
--------------------------

### Вы можете свободно:

**делиться** — копировать, распространять и передавать другим лицам данное произведение

**изменять** (создавать производные произведения) — чтобы приспособить это произведение к своим задачам


### При обязательном соблюдении следующих условий:

**Attribution (Атрибуция)** — Вы должны атрибутировать произведение (указывать автора и источник) в порядке, предусмотренном автором или лицензиаром (но только так, чтобы никоим образом не подразумевалось, что они поддерживают вас или использование вами данного произведения).

**Некоммерческое использование** — Вы не можете использовать это произведение в коммерческих целях.

**Сохранение условий (Share Alike)** — Если вы изменяете, преобразуете или берёте за основу это произведение, вы можете распространять результат только по такой же или подобной лицензии, что и у данного произведения.

### И понимая, что:

**Отказ** — Любое из перечисленных выше условий может быть отменено, если вы получили на это разрешение от правообладателя.

**Общественное достояние** — Там, где в соответствии с применимым законодательством это произведение или любые его элементы находятся в общественном достоянии, данная лицензия никоим образом не влияет на этот статус.

**Прочие права** — Данная лицензия ни в какой ситуации не затрагивает следующие права:
 - Ваши права на **добросовестное поведение (fair dealing)**, или добросовестное использование (fair use), или другие применимые исключения и ограничения авторского права;
 - **Личные неимущественные** права авторов;
 - Права других лиц могут быть затронуты непосредственно данным произведением или тем, как оно используется, например, права на **публичность** или конфиденциальность.

**Уведомление** — Для повторного использования или распространения вы должны чётко дать понять другим данные лицензионные условия этого произведения.

- [English](http://creativecommons.org/licenses/by-nc-sa/3.0/)
- [Russian](http://creativecommons.org/licenses/by-nc-sa/3.0/deed.ru)
- [Ukrain](http://creativecommons.org/licenses/by-nc-sa/3.0/deed.uk)
