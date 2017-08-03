<?php
namespace AveWd;

setlocale(LC_ALL, 'ru_RU.UTF-8');

set_include_path(__DIR__ . '/../vendor/');

require_once 'autoload.php';

use Carbon\Carbon;
use Symfony\Component\Console\Application;

class_alias('\RedBeanPHP\R','\R');
\R::setup('sqlite::memory:');
register_shutdown_function(function(){
    \R::close();
});



Carbon::setLocale('ru');
Carbon::setUtf8(true);

require_once __DIR__ . '/myConsoleCommand.inc';
require_once __DIR__ . '/myZIP.inc';

$app = new Application('AVE WatchDog', '1.0.0');

require_once __DIR__ . '/BadInternetOrders.inc';
require_once __DIR__ . '/inSuppErrors.inc';

