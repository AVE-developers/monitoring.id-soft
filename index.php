<?php

namespace AveWd;

use Symfony\Component\Console\Application;


set_include_path(realpath(__DIR__ . '/vendor/'));
require_once 'autoload.php';

// Устанавливаем русский язык
setlocale(LC_ALL, 'ru_RU.UTF-8');
\Carbon\Carbon::setLocale('ru');

require_once realpath(__DIR__.'/src/myConsoleCommand.inc');
require_once realpath(__DIR__.'/src/BadInternetOrders.inc');

$app = new Application('AVE WatchDog', '1.0.0');
$app->add(new BadInternetOrder('check-internet-orders'));
$app->run();