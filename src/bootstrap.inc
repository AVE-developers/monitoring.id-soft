<?php

namespace AveWd;

setlocale(LC_ALL, 'ru_RU.UTF-8');

set_include_path(__DIR__ . '/../../vendor');

require_once 'autoload.php';

use Carbon\Carbon;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\Command\HelpCommand;

class_alias('\RedBeanPHP\R', '\R');
\R::setup('sqlite::memory:');
register_shutdown_function(function () {
    \R::close();
});


Carbon::setLocale('ru');
Carbon::setUtf8(true);

require_once __DIR__ . '/check_orders.inc';
require_once __DIR__ . '/myZIP.inc';

$app = new Application('monitoring.id-soft', '1.0.0');

require_once __DIR__ . '/check_internet_orders.inc';
require_once __DIR__ . '/check_normal_orders.inc';
