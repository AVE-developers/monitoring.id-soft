<?php

namespace AveWd\Console\Command;

use AveWd\myZIP;
use Carbon\Carbon;
use Symfony\Component\Console\Input\InputDefinition;
use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Input\InputOption;
use Symfony\Component\Console\Output\OutputInterface;
use Symfony\Component\Yaml\Yaml;

class PrihodFetchBean
{
    private $ldigjlfgflglf;

    public function __construct()
    {
        $this->ldigjlfgflglf = \R::dispense('prihod');
    }

    public function __set(string $name, $value): void
    {
        $this->ldigjlfgflglf->{$name} = $value;
    }

    public function __destruct()
    {
        \R::store($this->ldigjlfgflglf);
    }
}

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
        $this->config = Yaml::parse(file_get_contents(realpath(__DIR__ . '/../etc/confg.yml')), Yaml::PARSE_OBJECT_FOR_MAP);
        $this->setDefinition(new InputDefinition([
            new InputOption('email', 'e', InputOption::VALUE_IS_ARRAY | InputOption::VALUE_REQUIRED,
                'Адрес электронной почты для отправки результата'),
            new InputOption('only-work-time', 'w', InputOption::VALUE_NONE,
                'Работать только в рабочее время с 9-18 с понедельника по пятницу'),
        ]));
    }

    protected function initialize(InputInterface $input, OutputInterface $output)
    {
        parent::initialize($input, $output);

        // Проверка заполенния обязательных параметров комманды
        $arr_itr_Options = (new \ArrayObject($this->getDefinition()->getOptions()))->getIterator();
        while ($arr_itr_Options->valid()) {
            if ($this->getDefinition()->getOption($arr_itr_Options->current()->getName())->isValueRequired()) {
                if (!$input->getOption($arr_itr_Options->current()->getName())) {
                    die(implode(PHP_EOL, ['Не верные параметры команды',
                            'Не указан: ' . $arr_itr_Options->current()->getDescription()])
                        . str_repeat(PHP_EOL, 2));
                }
            }
            $arr_itr_Options->next();
        }

        if ($input->getOption('only-work-time'))
            if (!Carbon::now()->isWeekday() || (Carbon::now()->hour < 9 || Carbon::now()->hour > 18)) {
                die('Не работаем в нерабочее время' . str_repeat(PHP_EOL, 2));
            }
        if ($input->getOption('verbose')) \R::fancyDebug(true);

        static $PDOtryToConnect = 5;
        try {
            $output->writeln('Готовим PDO: осталось попыток: ' . $PDOtryToConnect);
            $this->skd  = new \PDO($this->config->database->ave_skd->dsn . ' connect_timeout=3 options=\'--client_encoding=UTF-8\'',
                $this->config->database->ave_skd->username, $this->config->database->ave_skd->password);
            $this->repl = new \PDO($this->config->database->transport->dsn . ' connect_timeout=3 options=\'--client_encoding=WIN1251\'',
                $this->config->database->transport->username, $this->config->database->transport->password);
            $output->writeln('Настраиваем драйвер БД');
            // Генерировать исключение при ошибках
            $this->skd->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
            $this->repl->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);
            // Преобразовать пустые строки в NULL
            $this->skd->setAttribute(\PDO::ATTR_ORACLE_NULLS, \PDO::NULL_EMPTY_STRING);
            $this->repl->setAttribute(\PDO::ATTR_ORACLE_NULLS, \PDO::NULL_EMPTY_STRING);
            // Преобразовывать числовые значения в строки во время выборки. Значение типа bool.
            $this->skd->setAttribute(\PDO::ATTR_STRINGIFY_FETCHES, false);
            $this->repl->setAttribute(\PDO::ATTR_STRINGIFY_FETCHES, false);
            // Режим выборки по умолчанию
            $this->skd->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_NAMED);
            $this->repl->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_NAMED);
            $PDOtryToConnect = 5;
        } catch (\Throwable $throwable) {
            if (--$PDOtryToConnect) $this->interact($input, $output);
            else die($throwable);
        }
    }

    protected function interact(InputInterface $input, OutputInterface $output)
    {
        parent::interact($input, $output);
    }

    protected function execute(InputInterface $input, OutputInterface $output)
    {
        $this->repl->query(
            file_get_contents(__DIR__ . '/../sql/pg/getAptek.sql', false))
            ->fetchAll(\PDO::FETCH_FUNC, function () {
                $aptek              = \R::dispense('aptek');
                $aptek->id_instance = func_get_arg(0);
                $aptek->inactive    = func_get_arg(1);
                $aptek->is_active   = func_get_arg(2);
                $aptek->is_closed   = func_get_arg(3);
                \R::store($aptek);
            });
        $this->getPrihod();
    }

    public function getPackets(Carbon $mddoc): void
    {
        try {
            $this->repl->query($sql = (new \Twig_Environment(new \Twig_Loader_Filesystem(realpath(__DIR__ . '/../sql/pg')), ['auto_reload' => true, 'optimizations' => 0,]))
                ->render('getPackets.sql.twig', ['ndocs' => \R::getCol('SELECT DISTINCT n_doc FROM prihod ORDER BY n_doc'), 'mddoc' => $mddoc]))
                ->fetchAll(\PDO::FETCH_FUNC, function () {
                    $prihod          = \R::dispense('insupp');
                    $prihod->tname   = func_get_arg(0);
                    $prihod->ndoc    = func_get_arg(1);
                    $prihod->enqueue = func_get_arg(2);
                    $prihod->data    = func_get_arg(3);
                    $prihod->msg     = func_get_arg(4);
                    $prihod->npack   = func_get_arg(5);
                    \R::store($prihod);
                });
        } catch (\Twig_Error_Syntax $exception) {
            die($exception->getSourceContext()->getPath() . '#' . $exception->getTemplateLine() . ': ' . $exception->getRawMessage());
        } catch (\Throwable $throwable) {
            die($throwable);
        } finally {
            myZIP::$zPath    = __DIR__ . '/../tmp/sql.' . Carbon::now()->timestamp . '.zip';
            myZIP::$fPath    = __DIR__ . '/../tmp/'.$this->getName().'.sql';
            myZIP::$password = strtoupper(dechex(crc32($sql)));
            file_put_contents(myZIP::$fPath, $sql);
            myZIP::doZip();
        }
    }

    public function getPrihod(): void
    {
        try {
            ($this->skd->query(file_get_contents(__DIR__ . '/../sql/pg/'.$this->getName().'.sql')))
                ->fetchAll(\PDO::FETCH_CLASS | \PDO::FETCH_PROPS_LATE, '\AveWd\Console\Command\PrihodFetchBean');
        } catch (\Throwable $throwable) {
            die($throwable);
        } finally {
            $this->getPackets(Carbon::createFromTimestamp(\R::getCell('SELECT min(d_doc) FROM prihod GROUP BY d_doc'), 'Europe/Moscow')->subDay()->startOfDay());
        }
    }
}