Описание
========
Оболочка для игры TimeZero.

** Внимание! Находится в разработке! **

![Screenshot TimeZero Shell on Linux](http://ompldr.org/vZXRldA/timezero-shell.jpg)


Сборка и настройка
==================
Для сборки программы вам необнохимо наличие следующих библиотек (dev версий для Debian)

 - GTK2 (minimum 2.16)
 - WebKit 1.4+ (best 1.6)
 - json-c

#### Установка и сборка
Все нижеперечисленные команды выполняются от обычного пользователя.

	# Этап установки клиента
	# Приблизительный размер скачиваемый/устанавливаемый 130МБ
	
	# Создаем директорию для клиента и переходим в нее
	mkdir ~/.timezero
	cd ~/.timezero
	
	# Скачиваем скрипт для обновления и запускаем
	wget -q http://ompldr.org/vZHAwdw/update -O update
	chmod u+x update
	./update


	# Клонируем последний исходный код
	git clone http://github.com/gordio/timezero-shell.git
	
	# Входим в директорию с проектом
	cd timezero-shell
	
	# Настраиваем окружение (проверка нужных библиотек)
	./waf configure
	
	# Сборка программы
	./waf
	
	# Установка
	sudo cp build/timezero-shell /usr/local/bin

	# Копируем файлы оболочки в директорию клиента
	./install.sh

#### Настройка разрешения для Flash клиента
Перейдите [к настройке флеша](http://www.macromedia.com/support/documentation/en/flashplayer/help/settings_manager04.html) и добавьте директорию /home/<имя пользователя>/.timezero/ в список разрешенных зон.

> Always trust files in these locations: => Edit locations... => Add location...
