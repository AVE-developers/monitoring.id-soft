<?php

namespace AveWd;


use function PHPSTORM_META\type;
use Symfony\Component\Console\Application;
use Symfony\Component\Console\Command\Command;
use Symfony\Component\Console\Helper\FormatterHelper;
use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;

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