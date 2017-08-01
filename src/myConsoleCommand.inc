<?php

namespace AveWd\Console\Command;

use Carbon\Carbon;
use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
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
        $this->setDefinition(new InputDefinition([
            new InputOption('email', 'e', InputOption::VALUE_IS_ARRAY | InputOption::VALUE_REQUIRED,
                'Адрес электронной почты для отправки результата'),
            new InputOption('only-work-time', 'w', InputOption::VALUE_NONE,
                'Работать только в рабочее время с 9-18 с понедельника по пятницу'),
        ]));
    }

    protected function initialize(InputInterface $input, OutputInterface $output)
    {
        parent::initialize($input,$output);

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

        if ($input->getOption('only-work-time'))
            if (!Carbon::now()->isWeekday() || (Carbon::now()->hour < 9 || Carbon::now()->hour > 18)) {
                    die('Не работаем в нерабочее время' . str_repeat(PHP_EOL, 2));
                }
        if ($input->getOption('verbose')) \R::fancyDebug(true);

        static $PDOtryToConnect=5;
        try {
            $output->writeln('Готовим PDO: осталось попыток: '.$PDOtryToConnect);
            $this->_pg__ave_skd = new \PDO($this->config->database->ave_skd->dsn . ' connect_timeout=3 options=\'--client_encoding=UTF-8\'',
                $this->config->database->ave_skd->username, $this->config->database->ave_skd->password);
            $this->_pg__repl    = new \PDO($this->config->database->transport->dsn . ' connect_timeout=3 options=\'--client_encoding=WIN1251\'',
                $this->config->database->transport->username, $this->config->database->transport->password);
            $output->writeln('Настраиваем драйвер БД');
            // Генерировать исключение при ошибках
            $this->_pg__ave_skd->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
            $this->_pg__repl->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
            // Преобразовать пустые строки в NULL
            $this->_pg__ave_skd->setAttribute(\PDO::ATTR_ORACLE_NULLS, \PDO::NULL_EMPTY_STRING);
            $this->_pg__repl->setAttribute(\PDO::ATTR_ORACLE_NULLS, \PDO::NULL_EMPTY_STRING);
            // Преобразовывать числовые значения в строки во время выборки. Значение типа bool.
            $this->_pg__ave_skd->setAttribute(\PDO::ATTR_STRINGIFY_FETCHES, false);
            $this->_pg__repl->setAttribute(\PDO::ATTR_STRINGIFY_FETCHES, false);
            // Режим выборки по умолчанию
            $this->_pg__ave_skd->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_NAMED);
            $this->_pg__repl->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_NAMED);
            $PDOtryToConnect = 5;
        } catch (\PDOException $PDOException) {
            if(--$PDOtryToConnect) $this->interact($input,$output);
            else die($PDOException);
        }
    }
    protected function interact(InputInterface $input, OutputInterface $output)
    {
        parent::interact($input,$output);
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {
    }
}