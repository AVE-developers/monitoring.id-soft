monitoring.id-soft 1.0.0
========================

* [`check-internet-orders`](#check-internet-orders)
* [`check-normal-orders`](#check-normal-orders)
* [`help`](#help)
* [`list`](#list)

`check-internet-orders`
-----------------------

Проверка не отправленных на аптеку интернет-заказов

### Usage

* `check-internet-orders [-e|--email EMAIL] [-w|--only-work-time]`

Выбираем плохие приходы вида интернет-заказ.
Ищем пакеты этих документов в транспортной базе данных.
Выбираем информацию о состоянии аптек, упомянутых в выбранных документах.
Собираем все вместе в консолидированной таблице.
Сохраняем запрос к транспортной базе, который показывает состояние пакетов по выбранным документам, в файл *.sql и архивируем его в формат zip. Архив шифруется.
Отправляем письмо по указанным в команде адресам.

### Options

#### `--email|-e`

Адрес электронной почты для отправки результата

* Accept value: yes
* Is value required: yes
* Is multiple: yes
* Default: `array ()`

#### `--only-work-time|-w`

Работать только в рабочее время с 9-20

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--help|-h`

Display this help message

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--quiet|-q`

Do not output any message

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--verbose|-v|-vv|-vvv`

Increase the verbosity of messages: 1 for normal output, 2 for more verbose output and 3 for debug

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--version|-V`

Display this application version

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--ansi`

Force ANSI output

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--no-ansi`

Disable ANSI output

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--no-interaction|-n`

Do not ask any interactive question

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

`check-normal-orders`
---------------------

Проверка не отправленных на аптеку приходов ( кроме интернет-заказов )

### Usage

* `check-normal-orders [-e|--email EMAIL] [-w|--only-work-time]`

Выбираем плохие приходы вида не интернет-заказ.
Ищем пакеты этих документов в транспортной базе данных.
Выбираем информацию о состоянии аптек, упомянутых в выбранных документах.
Собираем все вместе в консолидированной таблице.
Сохраняем запрос к транспортной базе, который показывает состояние пакетов по выбранным документам, в файл *.sql и архивируем его в формат zip. Архив шифруется.
Отправляем письмо по указанным в команде адресам.

### Options

#### `--email|-e`

Адрес электронной почты для отправки результата

* Accept value: yes
* Is value required: yes
* Is multiple: yes
* Default: `array ()`

#### `--only-work-time|-w`

Работать только в рабочее время с 9-20

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--help|-h`

Display this help message

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--quiet|-q`

Do not output any message

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--verbose|-v|-vv|-vvv`

Increase the verbosity of messages: 1 for normal output, 2 for more verbose output and 3 for debug

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--version|-V`

Display this application version

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--ansi`

Force ANSI output

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--no-ansi`

Disable ANSI output

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--no-interaction|-n`

Do not ask any interactive question

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

`help`
------

Displays help for a command

### Usage

* `help [--format FORMAT] [--raw] [--] [<command_name>]`

The help command displays help for a given command:

  php C:\Users\Администратор\Documents\robotStarter\monitoring.id-soft\run.php help list

You can also output the help in other formats by using the --format option:

  php C:\Users\Администратор\Documents\robotStarter\monitoring.id-soft\run.php help --format=xml list

To display the list of available commands, please use the list command.

### Arguments

#### `command_name`

The command name

* Is required: no
* Is array: no
* Default: `'help'`

### Options

#### `--format`

The output format (txt, xml, json, or md)

* Accept value: yes
* Is value required: yes
* Is multiple: no
* Default: `'txt'`

#### `--raw`

To output raw command help

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--help|-h`

Display this help message

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--quiet|-q`

Do not output any message

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--verbose|-v|-vv|-vvv`

Increase the verbosity of messages: 1 for normal output, 2 for more verbose output and 3 for debug

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--version|-V`

Display this application version

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--ansi`

Force ANSI output

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--no-ansi`

Disable ANSI output

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--no-interaction|-n`

Do not ask any interactive question

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

`list`
------

Lists commands

### Usage

* `list [--raw] [--format FORMAT] [--] [<namespace>]`

The list command lists all commands:

  php C:\Users\Администратор\Documents\robotStarter\monitoring.id-soft\run.php list

You can also display the commands for a specific namespace:

  php C:\Users\Администратор\Documents\robotStarter\monitoring.id-soft\run.php list test

You can also output the information in other formats by using the --format option:

  php C:\Users\Администратор\Documents\robotStarter\monitoring.id-soft\run.php list --format=xml

It's also possible to get raw list of commands (useful for embedding command runner):

  php C:\Users\Администратор\Documents\robotStarter\monitoring.id-soft\run.php list --raw

### Arguments

#### `namespace`

The namespace name

* Is required: no
* Is array: no
* Default: `NULL`

### Options

#### `--raw`

To output raw command list

* Accept value: no
* Is value required: no
* Is multiple: no
* Default: `false`

#### `--format`

The output format (txt, xml, json, or md)

* Accept value: yes
* Is value required: yes
* Is multiple: no
* Default: `'txt'`