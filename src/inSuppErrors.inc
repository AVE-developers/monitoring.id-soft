<?php

namespace AveWd;

use AveWd\Console\Command\Command;
use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Console\Input\InputOption;

class inSuppErrors extends Command
{
    public function __construct(string $name = null)
    {
        parent::__construct($name);
    }

    protected function configure()
    {
        parent::configure();
        $this->setDescription('Проверка не отправленных на аптеку приходов ( кроме интернет-заказов )');
        $this->setHelp('Здесь будет описание комманды' . PHP_EOL . PHP_EOL);
        $this->setDefinition(new InputDefinition([
            new InputOption('email', 'e', InputOption::VALUE_IS_ARRAY | InputOption::VALUE_REQUIRED,
                'Адрес электронной почты для отправки результата')]));
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {

    }
}

$app->add(new inSuppErrors('check-normal-orders'));