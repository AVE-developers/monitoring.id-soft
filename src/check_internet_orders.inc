<?php

namespace AveWd\Console\Command;

class check_internet_orders extends check_orders
{
    protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка не отправленных на аптеку интернет-заказов');
        $this->setHelp('Здесь будет описание комманды' . PHP_EOL . PHP_EOL);
        $this->mailSubject = 'Монитор транспорта накладных';
    }
}

/** @var \Symfony\Component\Console\Application $app */
$app->add(new check_internet_orders('check-internet-orders'));