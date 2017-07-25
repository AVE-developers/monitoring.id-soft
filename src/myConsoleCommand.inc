<?php

namespace AveWd\Console\Command;

use Symfony\Component\Console\Helper\FormatterHelper;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Yaml\Yaml;

class Command extends \Symfony\Component\Console\Command\Command
{
    public $formatter;
    protected $config;

    public function __construct($name = null)
    {
        parent::__construct($name);
    }

    protected function configure()
    {
        parent::configure();
        $this->formatter = new FormatterHelper();
        $this->config = Yaml::parse(file_get_contents(realpath(__DIR__ . '/../etc/confg.yml')),Yaml::PARSE_OBJECT_FOR_MAP);
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        $output->getFormatter()->setStyle('info',
            new \Symfony\Component\Console\Formatter\OutputFormatterStyle('yellow', null, ['bold', 'blink']));
        $output->getFormatter()->setStyle('err',
            new \Symfony\Component\Console\Formatter\OutputFormatterStyle('red', null, ['bold', 'blink']));
        $output->getFormatter()->setStyle('num',
            new \Symfony\Component\Console\Formatter\OutputFormatterStyle('green', null, ['bold', 'blink']));
        $output->getFormatter()->setStyle('green',
            $output->getFormatter()->getStyle('num'));

        // Проверка заполенния обязательных параметров комманды
        $arr_itr_Options = (new \ArrayObject($this->getDefinition()->getOptions()))->getIterator();
        while ($arr_itr_Options->valid()) {
            if ($this->getDefinition()->getOption($arr_itr_Options->current()->getName())->isValueRequired()) {
                if (!$input->getOption($arr_itr_Options->current()->getName())) {
                    $output->writeln($this->formatter->formatBlock([
                        'Не верные параметры команды',
                        'Не указан: ' . $arr_itr_Options->current()->getDescription(),
                        'Изучите документацию к команде:',
                        PHP_BINARY . ' -f ' . __FILE__ . ' help ' . $this->getName()
                    ], 'err'));
                    die;
                }
            }
            $arr_itr_Options->next();
        }
    }
}