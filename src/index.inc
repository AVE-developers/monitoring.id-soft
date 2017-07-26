<?php

namespace AveWd;

use Symfony\Component\Console\Application;


set_include_path(realpath(__DIR__ . '/../vendor/'));
require_once 'autoload.php';

setlocale(LC_ALL, 'ru_RU.UTF-8');
\Carbon\Carbon::setLocale('ru');
\Carbon\Carbon::setUtf8(true);

require_once realpath(__DIR__ . '/myConsoleCommand.inc');
require_once realpath(__DIR__ . '/myZIP.inc');

$app = new Application('AVE WatchDog', '1.0.0');

require_once realpath(__DIR__ . '/BadInternetOrders.inc');
require_once realpath(__DIR__ . '/inSuppErrors.inc');
