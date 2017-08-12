<?php

namespace AveWd\Console\Command;

class check_normal_orders extends check_orders
{
    protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка не отправленных на аптеку приходов ( кроме интернет-заказов )');
        $this->setHelp('Здесь будет описание комманды' . PHP_EOL . PHP_EOL);
        $this->mailSubject = 'Мониторинг приходных накладных';
    }
}

/** @var \Symfony\Component\Console\Application $app */
$app->add(new check_normal_orders('check-normal-orders'));