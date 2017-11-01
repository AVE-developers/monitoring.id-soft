<?php

namespace AveWd\Console\Command;

class check_internet_orders extends check_orders
{
    protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка не отправленных на аптеку интернет-заказов');
        $this->setHelp(
            implode(PHP_EOL,
                [
                    'Выбираем плохие приходы вида интернет-заказ.',
                    'Ищем пакеты этих документов в транспортной базе данных.',
                    'Выбираем информацию о состоянии аптек, упомянутых в выбранных документах.',
                    'Собираем все вместе в консолидированной таблице.',
                    'Сохраняем запрос к транспортной базе, который показывает состояние пакетов по выбранным документам, в файл *.sql и архивируем его в формат zip. Архив шифруется.',
                    'Отправляем письмо по указанным в команде адресам.'
                ]
            )
        );
        $this->mailSubject = 'Монитор транспорта интернет-заказов';
    }
}

/** @global \Symfony\Component\Console\Application $app */
$app->add(new check_internet_orders('check-internet-orders'));