<?php

namespace AveWd\Console\Command;

use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Yaml\Yaml;

class Command extends \Symfony\Component\Console\Command\Command
{
    protected $config;

    public function __construct($name = null)
    {
        parent::__construct($name);
    }

    protected function configure()
    {
        parent::configure();
        $this->config = Yaml::parse(file_get_contents(realpath(__DIR__ . '/../etc/confg.yml')),Yaml::PARSE_OBJECT_FOR_MAP);
    }
    protected function initialize(InputInterface $input, OutputInterface $output)
    {
        parent::initialize($input,$output);
    }
    protected function interact(InputInterface $input, OutputInterface $output)
    {
        parent::interact($input,$output);
        // Проверка заполенния обязательных параметров комманды
        $arr_itr_Options = (new \ArrayObject($this->getDefinition()->getOptions()))->getIterator();
        while ($arr_itr_Options->valid()) {
            if ($this->getDefinition()->getOption($arr_itr_Options->current()->getName())->isValueRequired()) {
                if (!$input->getOption($arr_itr_Options->current()->getName())) {
                    die(implode(PHP_EOL,['Не верные параметры команды',
                        'Не указан: ' . $arr_itr_Options->current()->getDescription()])
                        .str_repeat(PHP_EOL,2));
                }
            }
            $arr_itr_Options->next();
        }
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {




    }
}